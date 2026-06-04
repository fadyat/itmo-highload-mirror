NEG_INF = float('-inf')


def gotoh(s: str, t: str, rho: int, sigma: int) -> tuple[str, str]:
    """
    Global alignment of two sequences with affine gap penalties
    (Gotoh's algorithm).

    Uses three DP tables M/X/Y, one per possible last operation
    (match/mismatch, gap in t, gap in s). This makes it possible
    to distinguish opening a new gap from extending an existing
    one in O(nm) time and memory.

    Scoring:
        +1     - matching symbols
        -1     - mismatching symbols
        rho    - gap opening
        sigma  - each symbol inside a gap (including the first),
                 i.e. a gap of length k costs rho + k*sigma.

    Returns two aligned strings (gaps denoted by '-').
    """

    n, m = len(s), len(t)

    M = [[NEG_INF] * (m + 1) for _ in range(n + 1)]
    X = [[NEG_INF] * (m + 1) for _ in range(n + 1)]
    Y = [[NEG_INF] * (m + 1) for _ in range(n + 1)]

    M[0][0] = 0
    for i in range(1, n + 1):
        X[i][0] = rho + sigma * i
    for j in range(1, m + 1):
        Y[0][j] = rho + sigma * j

    for i in range(1, n + 1):
        for j in range(1, m + 1):
            match = 1 if s[i - 1] == t[j - 1] else -1

            M[i][j] = max(M[i - 1][j - 1], X[i - 1][j - 1], Y[i - 1][j - 1]) + match
            X[i][j] = max(M[i - 1][j] + rho + sigma, X[i - 1][j] + sigma)
            Y[i][j] = max(M[i][j - 1] + rho + sigma, Y[i][j - 1] + sigma)

    score = max(M[n][m], X[n][m], Y[n][m])
    if score == M[n][m]:
        state = 'M'
    elif score == X[n][m]:
        state = 'X'
    else:
        state = 'Y'

    a_s, a_t = [], []
    i, j = n, m
    while i > 0 or j > 0:
        if state == 'M':
            a_s.append(s[i - 1])
            a_t.append(t[j - 1])
            match = 1 if s[i - 1] == t[j - 1] else -1
            prev = M[i][j] - match
            i -= 1
            j -= 1
            if prev == M[i][j]:
                state = 'M'
            elif prev == X[i][j]:
                state = 'X'
            else:
                state = 'Y'
        elif state == 'X':
            a_s.append(s[i - 1])
            a_t.append('-')
            if i > 0 and X[i][j] == X[i - 1][j] + sigma:
                state = 'X'
            else:
                state = 'M'
            i -= 1
        else:
            a_s.append('-')
            a_t.append(t[j - 1])
            if j > 0 and Y[i][j] == Y[i][j - 1] + sigma:
                state = 'Y'
            else:
                state = 'M'
            j -= 1

    return ''.join(reversed(a_s)), ''.join(reversed(a_t))


def main() -> None:
    s, t = input(), input()
    rho, sigma = map(int, input().split())
    print(*gotoh(s, t, rho, sigma), sep='\n')


if __name__ == '__main__':
    main()
