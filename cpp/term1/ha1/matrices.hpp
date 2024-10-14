#pragma once

#include <cstddef>
#include <string>

class matrix {
public:
    size_t rows;
    size_t cols;
    double *data;

    matrix(size_t rows, size_t cols);

    explicit matrix(const std::string &path);

    friend std::ostream &operator<<(std::ostream &os, const matrix &m);

    matrix(const matrix &other);

    matrix &operator=(const matrix &other);

    ~matrix();
};

matrix add(const matrix &a, const matrix &b);

matrix mult(const matrix &a, const matrix &b);
