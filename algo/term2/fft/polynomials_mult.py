from typing import List

def solve(P: List[int], k: int, m: int) -> List[int]:
    n: int = len(P)
    res: List[int] = [0] * (n + k)

    for i in range(n):
        res[i] = (res[i] - P[i]) % m
        res[i + k] = (res[i + k] + P[i]) % m

    return res

def main():
    n, k, m = map(int, input().split())
    P = list(map(int, input().split()))
    result = solve(P, k, m)
    print(' '.join(map(str, result)))

if __name__ == "__main__":
    main()
