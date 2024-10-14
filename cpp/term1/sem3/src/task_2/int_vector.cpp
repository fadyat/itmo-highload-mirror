#include "int_vector.hpp"

#include <algorithm>

void pushBack(IntVector &v, int value) {
    if (v.size == v.capacity_) {
        size_t new_capacity = v.capacity_ == 0 ? 1 : v.capacity_ * 2;
        int *new_data = new int[new_capacity];
        std::copy(v.data, v.data + v.size, new_data);
        delete[] v.data;

        v.data = new_data;
        v.capacity_ = new_capacity;
    }

    v.data[v.size] = value;
    v.size++;
}

void popBack(IntVector &v) {
    if (v.size > 0) {
        v.size--;
    }
}

void deallocate(IntVector &v) {
    delete[] v.data;
    v.data = nullptr;
    v.size = 0;
    v.capacity_ = 0;
}
