import sys
from collections import deque
from contextlib import contextmanager
from typing import TextIO, Optional, Generator, Iterable


class TailException(Exception):
    pass


@contextmanager
def input_stream(file: Optional[str]) -> Generator[TextIO, None, None]:
    if file:
        with open(file, "r") as f:
            yield f
    else:
        yield sys.stdin


def tail(file: Optional[str], max_len: int) -> Iterable[str]:
    try:
        with input_stream(file) as f:
            return deque(f, maxlen=max_len)
    except FileNotFoundError:
        raise TailException(f"tail: {file}: No such file or directory")
    except IsADirectoryError:
        return []


def main():
    files = [None] if len(sys.argv) == 1 else sys.argv[1:]
    lines_number = 17 if len(sys.argv) == 1 else 10

    exit_code = 0
    for i, file in enumerate(files):
        try:
            last_lines = tail(file, lines_number)

            if len(files) > 1:
                print(f"{'\n' if i > 0 else ''}==> {file} <==")

            for line in last_lines:
                print(line, end="")

        except TailException as e:
            print(e, file=sys.stderr)
            exit_code = 1

    exit(exit_code)


if __name__ == "__main__":
    main()
