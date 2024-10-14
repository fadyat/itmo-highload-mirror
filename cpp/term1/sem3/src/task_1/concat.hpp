#pragma once

#include <cstddef>

char const *concat(
        char const a[], size_t a_size,
        char const b[], size_t b_size,
        size_t &concat_size
);
