from typing import Callable, Dict


# На вход подаются две последовательности s и t.
# Выведите глобальное выравнивание этих последовательностей, используя:
# +1 за совпадение символов, −1 за несовпадение символов и −2 за пропуски.
#
# Выведите последовательности на отдельных строках, используя знак "-" для пропусков.
def match_score(a: str, b: str, match: int, mismatch: int) -> int:
    return match if a == b else mismatch


# На вход подаются две последовательности аминокислот s и t.
# Выведите глобальное выравнивание этих последовательностей, используя:
# матрицу BLOSUM62 для совпадающих/несовпадающих символов и −5 за пропуски.
#
# Выведите последовательности на отдельных строках, используя знак "-" для пропусков.
def blosum62_score(a: str, b: str, mx: Dict[str, Dict[str, int]]) -> int:
    return mx[a][b]


# @formatter:off
BLOSUM62 = {
    'A': {'A': 4,  'C': 0,  'D': -2, 'E': -1, 'F': -2, 'G': 0,  'H': -2, 'I': -1, 'K': -1, 'L': -1, 'M': -1, 'N': -2, 'P': -1, 'Q': -1, 'R': -1, 'S': 1,  'T': 0,  'V': 0,  'W': -3, 'Y': -2},
    'C': {'A': 0,  'C': 9,  'D': -3, 'E': -4, 'F': -2, 'G': -3, 'H': -3, 'I': -1, 'K': -3, 'L': -1, 'M': -1, 'N': -3, 'P': -3, 'Q': -3, 'R': -3, 'S': -1, 'T': -1, 'V': -1, 'W': -2, 'Y': -2},
    'D': {'A': -2, 'C': -3, 'D': 6,  'E': 2,  'F': -3, 'G': -1, 'H': -1, 'I': -3, 'K': -1, 'L': -4, 'M': -3, 'N': 1,  'P': -1, 'Q': 0,  'R': -2, 'S': 0,  'T': -1, 'V': -3, 'W': -4, 'Y': -3},
    'E': {'A': -1, 'C': -4, 'D': 2,  'E': 5,  'F': -3, 'G': -2, 'H': 0,  'I': -3, 'K': 1,  'L': -3, 'M': -2, 'N': 0,  'P': -1, 'Q': 2,  'R': 0,  'S': 0,  'T': -1, 'V': -2, 'W': -3, 'Y': -2},
    'F': {'A': -2, 'C': -2, 'D': -3, 'E': -3, 'F': 6,  'G': -3, 'H': -1, 'I': 0,  'K': -3, 'L': 0,  'M': 0,  'N': -3, 'P': -4, 'Q': -3, 'R': -3, 'S': -2, 'T': -2, 'V': -1, 'W': 1,  'Y': 3},
    'G': {'A': 0,  'C': -3, 'D': -1, 'E': -2, 'F': -3, 'G': 6,  'H': -2, 'I': -4, 'K': -2, 'L': -4, 'M': -3, 'N': 0,  'P': -2, 'Q': -2, 'R': -2, 'S': 0,  'T': -2, 'V': -3, 'W': -2, 'Y': -3},
    'H': {'A': -2, 'C': -3, 'D': -1, 'E': 0,  'F': -1, 'G': -2, 'H': 8,  'I': -3, 'K': -1, 'L': -3, 'M': -2, 'N': 1,  'P': -2, 'Q': 0,  'R': 0,  'S': -1, 'T': -2, 'V': -3, 'W': -2, 'Y': 2},
    'I': {'A': -1, 'C': -1, 'D': -3, 'E': -3, 'F': 0,  'G': -4, 'H': -3, 'I': 4,  'K': -3, 'L': 2,  'M': 1,  'N': -3, 'P': -3, 'Q': -3, 'R': -3, 'S': -2, 'T': -1, 'V': 3,  'W': -3, 'Y': -1},
    'K': {'A': -1, 'C': -3, 'D': -1, 'E': 1,  'F': -3, 'G': -2, 'H': -1, 'I': -3, 'K': 5,  'L': -2, 'M': -1, 'N': 0,  'P': -1, 'Q': 1,  'R': 2,  'S': 0,  'T': -1, 'V': -2, 'W': -3, 'Y': -2},
    'L': {'A': -1, 'C': -1, 'D': -4, 'E': -3, 'F': 0,  'G': -4, 'H': -3, 'I': 2,  'K': -2, 'L': 4,  'M': 2,  'N': -3, 'P': -3, 'Q': -2, 'R': -2, 'S': -2, 'T': -1, 'V': 1,  'W': -2, 'Y': -1},
    'M': {'A': -1, 'C': -1, 'D': -3, 'E': -2, 'F': 0,  'G': -3, 'H': -2, 'I': 1,  'K': -1, 'L': 2,  'M': 5,  'N': -2, 'P': -2, 'Q': 0,  'R': -1, 'S': -1, 'T': -1, 'V': 1,  'W': -1, 'Y': -1},
    'N': {'A': -2, 'C': -3, 'D': 1,  'E': 0,  'F': -3, 'G': 0,  'H': 1,  'I': -3, 'K': 0,  'L': -3, 'M': -2, 'N': 6,  'P': -2, 'Q': 0,  'R': 0,  'S': 1,  'T': 0,  'V': -3, 'W': -4, 'Y': -2},
    'P': {'A': -1, 'C': -3, 'D': -1, 'E': -1, 'F': -4, 'G': -2, 'H': -2, 'I': -3, 'K': -1, 'L': -3, 'M': -2, 'N': -2, 'P': 7,  'Q': -1, 'R': -2, 'S': -1, 'T': -1, 'V': -2, 'W': -4, 'Y': -3},
    'Q': {'A': -1, 'C': -3, 'D': 0,  'E': 2,  'F': -3, 'G': -2, 'H': 0,  'I': -3, 'K': 1,  'L': -2, 'M': 0,  'N': 0,  'P': -1, 'Q': 5,  'R': 1,  'S': 0,  'T': -1, 'V': -2, 'W': -2, 'Y': -1},
    'R': {'A': -1, 'C': -3, 'D': -2, 'E': 0,  'F': -3, 'G': -2, 'H': 0,  'I': -3, 'K': 2,  'L': -2, 'M': -1, 'N': 0,  'P': -2, 'Q': 1,  'R': 5,  'S': -1, 'T': -1, 'V': -3, 'W': -3, 'Y': -2},
    'S': {'A': 1,  'C': -1, 'D': 0,  'E': 0,  'F': -2, 'G': 0,  'H': -1, 'I': -2, 'K': 0,  'L': -2, 'M': -1, 'N': 1,  'P': -1, 'Q': 0,  'R': -1, 'S': 4,  'T': 1,  'V': -2, 'W': -3, 'Y': -2},
    'T': {'A': 0,  'C': -1, 'D': -1, 'E': -1, 'F': -2, 'G': -2, 'H': -2, 'I': -1, 'K': -1, 'L': -1, 'M': -1, 'N': 0,  'P': -1, 'Q': -1, 'R': -1, 'S': 1,  'T': 5,  'V': 0,  'W': -2, 'Y': -2},
    'V': {'A': 0,  'C': -1, 'D': -3, 'E': -2, 'F': -1, 'G': -3, 'H': -3, 'I': 3,  'K': -2, 'L': 1,  'M': 1,  'N': -3, 'P': -2, 'Q': -2, 'R': -3, 'S': -2, 'T': 0,  'V': 4,  'W': -3, 'Y': -1},
    'W': {'A': -3, 'C': -2, 'D': -4, 'E': -3, 'F': 1,  'G': -2, 'H': -2, 'I': -3, 'K': -3, 'L': -2, 'M': -1, 'N': -4, 'P': -4, 'Q': -2, 'R': -3, 'S': -3, 'T': -2, 'V': -3, 'W': 11, 'Y': 2},
    'Y': {'A': -2, 'C': -2, 'D': -3, 'E': -2, 'F': 3,  'G': -3, 'H': 2,  'I': -1, 'K': -2, 'L': -1, 'M': -1, 'N': -2, 'P': -3, 'Q': -1, 'R': -2, 'S': -2, 'T': -2, 'V': -1, 'W': 2,  'Y': 7},
}
# @formatter:on

# На вход подаются две последовательности s и t.
# Выведите глобальное выравнивание этих последовательностей, используя:
# +2 за совпадение символов, −1 за несовпадение символов и −2 за пропуски.
#
# Выведите последовательности на отдельных строках, используя знак "-" для пропусков.
#
# Строки могут быть очень длинные (len(s) ≤ 5000, len(t) ≤ 5000).
def needleman_wunch_score_row(s: str, t: str, match_fn: Callable[[str, str], int], gap: int) -> list[int]:
    n, m = len(s), len(t)
    previous = [j * gap for j in range(m + 1)]

    for i in range(1, n + 1):
        current = [i * gap] + [0] * m
        for j in range(1, m + 1):
            current[j] = max(
                previous[j - 1] + match_fn(s[i - 1], t[j - 1]),
                previous[j] + gap,
                current[j - 1] + gap
            )

        previous = current

    return previous


def needleman_wunsch(s: str, t: str, match_fn: Callable[[str, str], int], gap: int) -> tuple[str, str]:
    n, m = len(s), len(t)
    dp = [[0] * (m + 1) for _ in range(n + 1)]

    for i in range(n + 1):
        dp[i][0] = i * gap
    for j in range(m + 1):
        dp[0][j] = j * gap

    for i in range(1, n + 1):
        for j in range(1, m + 1):
            dp[i][j] = max(
                dp[i - 1][j - 1] + match_fn(s[i - 1], t[j - 1]),
                dp[i - 1][j] + gap,
                dp[i][j - 1] + gap
            )

    i, j = n, m
    a, b = [], []
    while i or j:
        if i and j and dp[i][j] == dp[i - 1][j - 1] + match_fn(s[i - 1], t[j - 1]):
            a.append(s[i - 1])
            b.append(t[j - 1])
            i -= 1
            j -= 1
        elif i and dp[i][j] == dp[i - 1][j] + gap:
            a.append(s[i - 1])
            b.append('-')
            i -= 1
        else:
            a.append('-')
            b.append(t[j - 1])
            j -= 1

    return ''.join(reversed(a)), ''.join(reversed(b))


def hirschberg(s: str, t: str, match_fn: Callable[[str, str], int], gap: int) -> tuple[str, str]:
    if len(s) == 0:
        return '-' * len(t), t

    if len(t) == 0:
        return s, '-' * len(s)

    if len(s) == 1 or len(t) == 1:
        return needleman_wunsch(s, t, match_fn, gap)

    mid = len(s) // 2

    left_score = needleman_wunch_score_row(s[:mid], t, match_fn, gap)
    right_score = needleman_wunch_score_row(s[mid:][::-1], t[::-1], match_fn, gap)
    split = max(range(len(t) + 1), key=lambda i: left_score[i] + right_score[len(t) - i])

    ls, lt = hirschberg(s[:mid], t[:split], match_fn, gap)
    rs, rt = hirschberg(s[mid:], t[split:], match_fn, gap)

    return ls + rs, lt + rt


def main():
    s, t = input(), input()
    print(
        *hirschberg(s, t, lambda a, b: match_score(a, b, +2, -1), gap=-2),
        sep='\n'
    )


if __name__ == '__main__':
    main()
