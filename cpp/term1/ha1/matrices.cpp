#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <utility>

#include "matrices.hpp"


matrix::matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
    this->data = new double[rows * cols]();
}

matrix::matrix(const matrix &other) : matrix(other.rows, other.cols) {
    std::copy(other.data, other.data + rows * cols, data);
}

void swap(matrix &first, matrix &second) noexcept {
    using std::swap;

    swap(first.rows, second.rows);
    swap(first.cols, second.cols);
    swap(first.data, second.data);
}

matrix &matrix::operator=(const matrix &other) {
    if (this == &other) {
        return *this;
    }

    matrix cpy(other);
    swap(*this, cpy);

    return *this;
}

matrix::~matrix() {
    delete[] this->data;
}

matrix::matrix(const std::string &path) {
    std::ifstream file(path.data());
    if (!file.is_open()) {
        throw std::runtime_error("cannot open file: " + std::string(path));
    }

    file >> rows >> cols;
    data = new double[rows * cols]();

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            file >> data[i * cols + j];
        }
    }
}

std::ostream &operator<<(std::ostream &os, const matrix &m) {
    os << m.rows << " " << m.cols << std::endl;
    for (size_t i = 0; i < m.rows; i++) {
        for (size_t j = 0; j < m.cols; j++) {
            os << m.data[i * m.cols + j] << " ";
        }
        os << std::endl;
    }

    return os;
}

matrix add(const matrix &a, const matrix &b) {
    if (a.rows != b.rows) {
        throw std::runtime_error("not matching rows number");
    }

    if (a.cols != b.cols) {
        throw std::runtime_error("not matching cols number");
    }

    auto c = matrix(a.rows, a.cols);
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            c.data[i * a.cols + j] = a.data[i * a.cols + j] + b.data[i * a.cols + j];
        }
    }

    return c;
}

matrix mult(const matrix &a, const matrix &b) {
    if (a.cols != b.rows) {
        throw std::runtime_error("not matching rows cols number");
    }

    // here we're using naive approach, can be optimized
    // https://en.wikipedia.org/wiki/Matrix_multiplication_algorithm#Iterative_algorithm
    auto c = matrix(a.rows, b.cols);
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < b.cols; j++) {
            c.data[i * b.cols + j] = 0;
            for (size_t k = 0; k < a.cols; k++) {
                c.data[i * b.cols + j] += a.data[i * a.cols + k] * b.data[k * b.cols + j];
            }
        }
    }

    return c;
}
