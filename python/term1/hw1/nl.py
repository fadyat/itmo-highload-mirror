import sys
from contextlib import contextmanager
from typing import Optional, TextIO, Generator


class NLException(Exception):
    pass


@contextmanager
def input_stream(file: Optional[str]) -> Generator[TextIO, None, None]:
    if file:
        with open(file, "r") as f:
            yield f
    else:
        yield sys.stdin


def nl(file: Optional[str]):
    try:
        with input_stream(file) as f:
            for i, line in enumerate(f):
                print(f"{i + 1:6}\t{line}", end="")
    except FileNotFoundError:
        raise NLException(f"nl: {file}: No such file or directory")
    except IsADirectoryError:
        raise NLException(f"nl: {file}: Is a directory")


def main():
    file = sys.argv[1] if len(sys.argv) > 1 else None

    try:
        nl(file)
    except NLException as e:
        print(e, file=sys.stderr)
        exit(1)


if __name__ == "__main__":
    main()
