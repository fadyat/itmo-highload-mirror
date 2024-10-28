#include "shared_buffer.hpp"
#include <utility>

SharedBuffer::SharedBuffer() : data_(new char[1]), refCount_(new size_t(1)), size_(0) { data_[0] = '\0'; }

SharedBuffer::SharedBuffer(size_t size) {
    data_ = new char[size + 1]();
    data_[size] = '\0';
    refCount_ = new size_t(1);
    size_ = size;
}

SharedBuffer::SharedBuffer(const SharedBuffer &other) : data_(other.data_), refCount_(other.refCount_), size_(other.size_) {
    (*refCount_)++;
}

void SharedBuffer::swap(SharedBuffer &other) noexcept {
    std::swap(data_, other.data_);
    std::swap(refCount_, other.refCount_);
    std::swap(size_, other.size_);
}

SharedBuffer &SharedBuffer::operator=(const SharedBuffer &other) {
    if (this != &other) {
        SharedBuffer temp(other);
        swap(temp);
    }

    return *this;
}

SharedBuffer::~SharedBuffer() {
    if (refCount_ && --(*refCount_) == 0) {
        delete[] data_;
        delete refCount_;
    }
}

size_t SharedBuffer::getSize() const { return size_; }

char *SharedBuffer::getData() const { return data_; }

size_t SharedBuffer::useCount() const { return *refCount_; }
