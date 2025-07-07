from typing import List

def main():
    n, k, m = map(int, input().split())
    p = list(map(int, input().split()))

    t = max(n - k, 1)
    a = [0] * t

    for i in range(n - 1, k - 1, -1):
        coeff = p[i] % m
        if coeff == 0:
            continue
        a[i - k] = coeff
        p[i] = (p[i] - coeff) % m
        p[i - k] = (p[i - k] + coeff) % m

    r = p[:k]

    if len(p) < k:
        r += [0] * (k - len(p))

    print(" ".join(str(x % m) for x in a))
    print(" ".join(str(x % m) for x in r))


if __name__ == "__main__":
    main()
