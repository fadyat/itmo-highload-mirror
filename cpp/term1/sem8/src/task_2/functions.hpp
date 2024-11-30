#pragma once

namespace cls08 {

template <typename Iterator, typename T>
void fill(Iterator begin, Iterator end, const T &value) {
    for (auto it = begin; it != end; ++it) {
        *it = value;
    }
}

template <typename Iterator, typename Generator>
void generate(Iterator begin, Iterator end, Generator generator) {
    for (auto it = begin; it != end; ++it) {
        *it = generator();
    }
}

template <typename SrcIterator, typename DstIterator>
void copy(SrcIterator src_begin, SrcIterator src_end, DstIterator dst_begin) {
    for (auto it = src_begin; it != src_end; ++it) {
        *dst_begin = *it;
        ++dst_begin;
    }
}

template <typename Iterator, typename UnaryFunction>
void for_each(Iterator begin, Iterator end, UnaryFunction f) {
    for (auto it = begin; it != end; ++it) {
        f(*it);
    }
}

}
