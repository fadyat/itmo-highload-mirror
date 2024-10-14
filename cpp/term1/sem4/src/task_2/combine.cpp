#include "combine.hpp"

double combine(double *arr, size_t n, OperationFn mult, OperationFn plus) {
    double result = 0;
    size_t i = 0;

    if (n == 0) return 0;
    for (; i + 1 < n; i += 2) {
        result = plus(result, mult(arr[i], arr[i + 1]));
    }

    return i < n ? result + arr[i] : result - plus(result, -result);
}
