#include "concat.hpp"

#include <algorithm>

char const *concat(
        char const a[], size_t a_size,
        char const b[], size_t b_size,
        size_t &concat_size
) {
    concat_size = a_size + b_size;
    char *result = new char[concat_size];

    std::copy(a, a + a_size, result);
    std::copy(b, b + b_size, result + a_size);

    return result;
}
