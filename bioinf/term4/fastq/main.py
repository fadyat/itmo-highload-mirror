import dataclasses
from typing import Iterable, Iterator, Optional


@dataclasses.dataclass(frozen=True)
class GcRecord:
    gc: int
    length: int

    def __add__(self, other: 'GcRecord') -> 'GcRecord':
        return GcRecord(self.gc + other.gc, self.length + other.length)

    def __iadd__(self, other):
        return self + other

    def __str__(self) -> str:
        pct = round(self.gc / self.length * 100, 2) if self.length else 0
        return f"{self.gc}/{self.length} ({pct}%)"


@dataclasses.dataclass(frozen=True)
class FastqRecord:
    """
    FASTQ format reference:
    https://en.wikipedia.org/wiki/FASTQ_format
    """

    id: str
    seq: str
    _raw_qual: tuple[int, ...]

    @property
    def phred_qual(self) -> tuple[int, ...]:
        return tuple(0 if b in 'Nn' else q for b, q in zip(self.seq, self._raw_qual))

    def cut(self, trim_pos: int) -> Optional['FastqRecord']:
        if trim_pos <= 0:
            return None
        return FastqRecord(self.id, self.seq[:trim_pos], self._raw_qual[:trim_pos])

    @property
    def gc(self) -> GcRecord:
        return GcRecord(sum(1 for b in self.seq if b in 'GCgc'), len(self.seq))


def parse_fastq(iterable: Iterator[str]) -> Iterable[FastqRecord]:
    for id_line, seq_line, _, qual_line in zip(*[iter(iterable)] * 4):
        yield FastqRecord(
            id_line.strip()[1:],
            seq_line.strip(),
            tuple(ord(c) - 33 for c in qual_line.strip()),
        )


class PhredStats:
    def __init__(self, quality: int = 0, reads: int = 0, phred_pos: int = 9):
        self.quality = quality
        self.reads = reads
        self.phred_pos = phred_pos

    def update(self, record: Optional[FastqRecord]):
        if record is None:
            return
        phred = record.phred_qual
        if len(phred) > self.phred_pos:
            self.reads += 1
            self.quality += phred[self.phred_pos]

    @property
    def avg_phred(self) -> int:
        return round(self.quality / self.reads) if self.reads else 0


class FastqStats:
    def __init__(self, with_trimmed_stats: bool = False):
        self.num_reads = 0
        self.total_bases = 0
        self.min_len = float('inf')
        self.max_len = 0
        self.gc = GcRecord(0, 0)
        self.phred_stats = PhredStats()

        self.trimmed_stats: Optional['FastqStats'] = FastqStats() if with_trimmed_stats else None

    def update(self, record: FastqRecord, trimmed: Optional[FastqRecord]):
        l = len(record.seq)
        self.num_reads += 1
        self.total_bases += l
        self.min_len = min(self.min_len, l)
        self.max_len = max(self.max_len, l)
        self.gc += record.gc
        self.phred_stats.update(record)

        if trimmed is not None and self.trimmed_stats:
            self.trimmed_stats.update(trimmed, None)

    @property
    def avg_len(self) -> int:
        return round(self.total_bases / self.num_reads) if self.num_reads else 0

    def __str__(self) -> str:
        lines = [
            f"Number of reads: {self.num_reads}",
            f"Minimum length: {self.min_len}",
            f"Average length: {self.avg_len}",
            f"Maximum length: {self.max_len}",
            f"GC: {self.gc}",
            f"Phred stats: {self.phred_stats.avg_phred}"
        ]
        if self.trimmed_stats:
            lines.append(f"Trimmed reads: {self.num_reads - self.trimmed_stats.num_reads}")
            lines.append("Trimmed stats:")
            for l in str(self.trimmed_stats).splitlines():
                lines.append("\t" + l)
        return "\n".join(lines)


class SlidingWindowTrimmer:
    """
    Sliding Window Quality Trimmer.

    This trimmer scans a FastqRecord's Phred quality scores using a fixed-size window.
    - Computes the sum of qualities within the window.
    - If the sum falls below `window_size * quality_threshold`, the read is trimmed at that position.
    - After finding the trim position, the read is further trimmed from the end if bases fall below `quality_threshold`.
    """

    def __init__(self, window_size: int, quality_threshold: int):
        self.window_size = window_size
        self.quality_threshold = quality_threshold
        self.total_required_quality = self.quality_threshold * self.window_size

    def trim(self, record: FastqRecord) -> Optional[FastqRecord]:
        quals = record.phred_qual
        if len(quals) < self.window_size:
            return None

        window_quality = sum(quals[:self.window_size])
        if window_quality < self.total_required_quality:
            return None

        trim_pos = len(quals)
        for i in range(len(quals) - self.window_size):
            window_quality = window_quality - quals[i] + quals[i + self.window_size]
            if window_quality < self.total_required_quality:
                trim_pos = i + self.window_size
                break

        while trim_pos > 1 and quals[trim_pos - 1] < self.quality_threshold:
            trim_pos -= 1

        if trim_pos < 1:
            return None
        return record.cut(trim_pos) if trim_pos < len(quals) else record


class MinLenTrimmer:
    """
    Minimum Length Trimmer.

    This trimmer filters reads by length:
    - If the read length >= specified minimum, it is kept unchanged.
    - If the read length < minimum, it is discarded (returns None).
    """

    def __init__(self, length: int):
        self.length = length

    def trim(self, record: FastqRecord) -> Optional[FastqRecord]:
        return record if len(record.seq) >= self.length else None


def main():
    filename = "reads.fastq.txt"
    window_trimmer = SlidingWindowTrimmer(window_size=5, quality_threshold=30)
    min_len_trimmer = MinLenTrimmer(length=60)

    window_stats = FastqStats(with_trimmed_stats=True)
    length_stats = FastqStats(with_trimmed_stats=True)

    with open(filename) as f:
        for record in parse_fastq(f):
            window_trimmed = window_trimmer.trim(record)
            window_stats.update(record, window_trimmed)

            if window_trimmed:
                length_trimmed = min_len_trimmer.trim(window_trimmed)
                length_stats.update(window_trimmed, length_trimmed)

    print(window_stats)
    print("=" * 40)
    print(length_stats)


if __name__ == '__main__':
    main()
