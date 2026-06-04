P = {('A', 'U'), ('U', 'A'), ('G', 'C'), ('C', 'G')}


def nussin(s: str) -> str:
    n = len(s)
    if n < 2:
        return '.' * n

    dp = [[0] * n for _ in range(n)]

    def _can_pair(_i: int, _j: int) -> bool:
        return _j - _i >= 2 and (s[_i], s[_j]) in P

    for k in range(1, n):
        for i in range(n - k):
            j = i + k
            best = dp[i + 1][j]

            if _can_pair(i, j):
                best = max(best, dp[i + 1][j - 1] + 1)

            for t in range(i + 1, j):
                if _can_pair(i, t):
                    best = max(best, dp[i + 1][t - 1] + 1 + dp[t + 1][j])

            dp[i][j] = best

    structure = ['.'] * n
    stack = [(0, n - 1)]
    while stack:
        i, j = stack.pop()
        if i >= j:
            continue

        if dp[i][j] == dp[i + 1][j]:
            stack.append((i + 1, j))
            continue

        for t in range(i + 1, j + 1):
            if not _can_pair(i, t):
                continue

            inner = dp[i + 1][t - 1] if t - 1 >= i + 1 else 0
            right = dp[t + 1][j] if t + 1 <= j else 0

            if dp[i][j] == inner + 1 + right:
                structure[i] = '('
                structure[t] = ')'
                stack.append((i + 1, t - 1))
                stack.append((t + 1, j))
                break

    return ''.join(structure)


def main():
    s = input().strip().upper().replace('T', 'U')
    print(nussin(s))


if __name__ == '__main__':
    main()
