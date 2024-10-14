#pragma once

#include <cstddef>

struct IntVector {
    int *data = nullptr;
    size_t size = 0;
    size_t capacity_ = 0;
};

void pushBack(IntVector &v, int value);

void popBack(IntVector &v);

void deallocate(IntVector &v);
