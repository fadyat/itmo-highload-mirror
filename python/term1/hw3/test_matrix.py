import numpy as np
import pytest

from matrix import Matrix


def test_matrix_addition_success():
    mx1 = Matrix([[1, 2], [3, 4]])
    mx2 = Matrix([[5, 6], [7, 8]])
    result = mx1 + mx2
    assert result.data == [[6, 8], [10, 12]]
    assert result.data == np.add(mx1.data, mx2.data).tolist()


def test_matrix_addition_failure():
    with pytest.raises(ValueError) as e:
        mx1 = Matrix([[1, 2], [3, 4]])
        mx2 = Matrix([[5, 6]])
        _ = mx1 + mx2
        assert str(e.value) == "Matrix shapes are not equal"


def test_matrix_multiplication_success():
    mx1 = Matrix([[1, 2], [3, 4]])
    mx2 = Matrix([[5, 6], [7, 8]])
    result = mx1 * mx2
    assert result.data == [[5, 12], [21, 32]]
    assert result.data == np.multiply(mx1.data, mx2.data).tolist()


def test_matrix_multiplication_failure():
    with pytest.raises(ValueError) as e:
        mx1 = Matrix([[1, 2], [3, 4]])
        mx2 = Matrix([[5, 6]])
        _ = mx1 * mx2
        assert str(e.value) == "Matrix shapes are not equal"


def test_matrix_matmul_success():
    mx1 = Matrix([[1, 2], [3, 4]])
    mx2 = Matrix([[5, 6], [7, 8]])
    result = mx1 @ mx2
    assert result.data == [[19, 22], [43, 50]]
    assert result.data == np.matmul(mx1.data, mx2.data).tolist()


def test_matrix_matmul_failure():
    with pytest.raises(ValueError) as e:
        mx1 = Matrix([[1, 2], [3, 4]])
        mx2 = Matrix([[5, 6]])
        _ = mx1 @ mx2
        assert str(e.value) == "Matrix shapes are not compatible for multiplication"


def test_random_matrix_success():
    mx1 = Matrix(np.random.randint(0, 10, (10, 10)).tolist())
    mx2 = Matrix(np.random.randint(0, 10, (10, 10)).tolist())
    assert (mx1 + mx2).data == np.add(mx1.data, mx2.data).tolist()
    assert (mx1 * mx2).data == np.multiply(mx1.data, mx2.data).tolist()
    assert (mx1 @ mx2).data == np.matmul(mx1.data, mx2.data).tolist()
