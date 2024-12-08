from typing import List

import numpy as np
from numpy.lib.mixins import NDArrayOperatorsMixin


class Matrix:
    def __init__(self, data: List[List[int]]):
        self.data = data
        self.shape = (len(data), len(data[0]))

    def __add__(self, other: "Matrix") -> "Matrix":
        if self.shape != other.shape:
            raise ValueError("Matrix shapes are not equal")

        return Matrix(
            [
                [self.data[i][j] + other.data[i][j] for j in range(self.shape[1])]
                for i in range(self.shape[0])
            ]
        )

    def __mul__(self, other: "Matrix") -> "Matrix":
        if self.shape != other.shape:
            raise ValueError("Matrix shapes are not equal")

        return Matrix(
            [
                [self.data[i][j] * other.data[i][j] for j in range(self.shape[1])]
                for i in range(self.shape[0])
            ]
        )

    def __matmul__(self, other: "Matrix") -> "Matrix":
        if self.shape[1] != other.shape[0]:
            raise ValueError("Matrix shapes are not compatible for multiplication")

        return Matrix(
            [
                [
                    sum(
                        self.data[i][k] * other.data[k][j] for k in range(self.shape[1])
                    )
                    for j in range(other.shape[1])
                ]
                for i in range(self.shape[0])
            ]
        )


class MatrixMixin:
    def __init__(self, data: List[List[int]]):
        self._data = data
        self._shape = (len(data), len(data[0]))

    @property
    def data(self):
        return self._data

    @property
    def shape(self):
        return self._shape

    @data.setter
    def data(self, value: List[List[int]]):
        if not isinstance(value, List) or not isinstance(value[0], List):
            raise ValueError("Data should be a 2D list")

        self._data = value

    def __str__(self):
        return str(self._data)

    def save(self, file_path: str):
        np.savetxt(file_path, self.data, fmt="%d")


class MatrixV2(MatrixMixin, NDArrayOperatorsMixin):
    _HANDLED_TYPES = (np.ndarray, list)

    # https://numpy.org/neps/nep-0013-ufunc-overrides.html#proposed-interface
    def __array_ufunc__(self, ufunc, method, *inputs, **kwargs):
        out = kwargs.get("out", ())
        for x in inputs + out:
            if not isinstance(x, self._HANDLED_TYPES + (MatrixV2,)):
                return NotImplemented

        inputs = tuple(x.data if isinstance(x, MatrixV2) else x for x in inputs)
        if out:
            kwargs["out"] = tuple(x.data if isinstance(x, MatrixV2) else x for x in out)

        result = getattr(ufunc, method)(*inputs, **kwargs)
        if type(result) is tuple:
            return tuple(MatrixV2(x.tolist()) for x in result)
        elif method == "at":
            return None
        else:
            return MatrixV2(result.tolist())


class MatrixHashMixin:
    def __hash__(self) -> int:
        """
        Some stupid hash function, which returns sum of all elements in matrix.
        :return: str - hash value of matrix
        """

        return sum(sum(row) for row in self.data)  # type: ignore


class MatrixV3(MatrixHashMixin, MatrixV2):
    _cache: dict[tuple[int, int], "MatrixV3"] = {}

    def __matmul__(self, other: "MatrixV3") -> "MatrixV3":
        hash_self, hash_other = hash(self), hash(other)
        key = (hash_self, hash_other)

        if key in self._cache:
            return self._cache[key]

        result = MatrixV3(super().__matmul__(other).data)
        self._cache[key] = result
        return result
