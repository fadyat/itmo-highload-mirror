#pragma once

#include <cstddef>

using OperationFn = double (*)(double, double);

double combine(double *arr, size_t n, OperationFn mult, OperationFn plus);