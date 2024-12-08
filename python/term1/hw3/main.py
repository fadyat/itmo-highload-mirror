import numpy as np

from matrix import Matrix, MatrixV2, MatrixV3


def generate_content() -> tuple[np.ndarray, np.ndarray]:
    return (
        np.random.randint(0, 10, (10, 10)),
        np.random.randint(0, 10, (10, 10)),
    )


def matrix_v1(c1: np.ndarray, c2: np.ndarray):
    mx1 = Matrix(c1.tolist())
    mx2 = Matrix(c2.tolist())

    np.savetxt("artifacts/3.1/matrix+.txt", (mx1 + mx2).data, fmt="%d")
    np.savetxt("artifacts/3.1/matrix*.txt", (mx1 * mx2).data, fmt="%d")
    np.savetxt("artifacts/3.1/matrix@.txt", (mx1 @ mx2).data, fmt="%d")


def matrix_v2(c1: np.ndarray, c2: np.ndarray):
    mx1 = MatrixV2(c1.tolist())
    mx2 = MatrixV2(c2.tolist())

    (mx1 + mx2).save("artifacts/3.2/matrix+.txt")
    (mx1 * mx2).save("artifacts/3.2/matrix*.txt")
    (mx1 @ mx2).save("artifacts/3.2/matrix@.txt")


def matrix_v3():
    A = MatrixV3([[1, 2], [3, 4]])
    B = MatrixV3([[5, 6], [7, 8]])
    C = MatrixV3([[3, 4], [1, 2]])
    D = MatrixV3([[5, 6], [7, 8]])

    assert hash(A) == hash(C) and A != C and B == D and A @ B != C @ D, "Hash collision"

    A.save("artifacts/3.3/A.txt")
    B.save("artifacts/3.3/B.txt")
    C.save("artifacts/3.3/C.txt")
    D.save("artifacts/3.3/D.txt")
    (A @ B).save("artifacts/3.3/AB.txt")
    (MatrixV2(C.data) @ MatrixV2(D.data)).save("artifacts/3.3/CD.txt")
    with open("artifacts/3.3/hash.txt", "w") as f:
        f.write(f"{hash(A @ B)}\n{hash(C @ D)}")


def main():
    np.random.seed(0)
    c1, c2 = generate_content()
    matrix_v1(c1, c2)
    matrix_v2(c1, c2)
    matrix_v3()


if __name__ == "__main__":
    main()
