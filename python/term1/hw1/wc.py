import sys
from contextlib import contextmanager
from dataclasses import dataclass
from typing import Optional, Generator, TextIO


class WCException(Exception):
    pass


@contextmanager
def input_stream(file: Optional[str]) -> Generator[TextIO, None, None]:
    if file:
        with open(file, "r") as f:
            yield f
    else:
        yield sys.stdin


@dataclass
class Stats:
    lines: int = 0
    words: int = 0
    bytes: int = 0
    filename: Optional[str] = None

    def __add__(self, other: "Stats") -> "Stats":
        return Stats(
            lines=self.lines + other.lines,
            words=self.words + other.words,
            bytes=self.bytes + other.bytes,
            filename=self.filename,
        )

    def __str__(self) -> str:
        base = f"{self.lines:8}{self.words:8}{self.bytes:8}"
        return f"{base} {self.filename}" if self.filename else base


def wc(file: Optional[str]) -> Stats:
    stats = Stats(filename=file)

    try:
        with input_stream(file) as f:
            for line in f:
                stats.lines += line.endswith("\n")
                stats.words += len(line.split())
                stats.bytes += len(line)
    except FileNotFoundError:
        raise WCException(f"wc: {file}: No such file or directory")
    except IsADirectoryError:
        raise WCException(f"wc: {file}: Is a directory")

    return stats


def main():
    files = [None] if len(sys.argv) == 1 else sys.argv[1:]

    exit_code = 0
    total_stats = Stats(filename="total")
    for file in files:
        try:
            stats = wc(file)
        except WCException as e:
            print(e, file=sys.stderr)
            exit_code = 1
            continue

        total_stats += stats
        print(stats)

    if len(files) > 1:
        print(total_stats)

    exit(exit_code)


if __name__ == "__main__":
    main()
