#include <iostream>
#include "array_utils.hpp"

int sum(const_int_pointer_t a, size_t s) {
    int sum = 0;
    for (size_t i = 0; i < s; ++i) {
        sum += a[i];
    }
    return sum;
}

size_t countNumbers(const_int_pointer_t a, size_t s) {
    int numbers[10] = {0};
    for (size_t i = 0; i < s; ++i) {
        numbers[a[i]] = 1;
    }

    return sum(numbers, 10);
}

void popZeros(const_int_pointer_t *a, size_t s) {
    size_t zero_count = 0;

    for (size_t i = 0; i < s; ++i) {
        const_int_pointer_t cur = a[i];

        if (cur == nullptr || *cur != 0) {
            std::swap(a[i - zero_count], a[i]);
        } else {
            ++zero_count;
        }
    }
}