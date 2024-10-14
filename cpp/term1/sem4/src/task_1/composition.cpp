#include "composition.hpp"
#include <cstdarg>
#include <iostream>

double Compose(size_t n, ...) {
    va_list args;
    va_start(args, n);

    FuncT *funcs = new FuncT[n];
    for (size_t i = 0; i < n; ++i) {
        funcs[i] = va_arg(args, FuncT);
    }

    double result = va_arg(args, double);
    for (size_t i = n; i > 0; --i) {
        result = funcs[i - 1](result);
    }

    delete[] funcs;
    va_end(args);

    return result;
}

