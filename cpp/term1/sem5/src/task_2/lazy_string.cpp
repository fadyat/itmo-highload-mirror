#include "lazy_string.hpp"
#include "shared_buffer.hpp"

#include <algorithm>
#include <cstring>

LazyString::LazyString(const char *str) {
    if (str == nullptr) {
        data_ = new SharedBuffer();
    } else {
        data_ = new SharedBuffer(strlen(str));
        std::copy(str, str + strlen(str), data_->getData());
    }
}

LazyString::LazyString(const LazyString &other) : data_(new SharedBuffer(*other.data_)) {}

void LazyString::swap(LazyString &other) { std::swap(data_, other.data_); }

LazyString &LazyString::operator=(const LazyString &other) {
    if (this != &other) {
        LazyString temp(other);
        swap(temp);
    }

    return *this;
}

LazyString::~LazyString() { delete data_; }

bool LazyString::operator==(const LazyString &other) const {
    return data_->getSize() == other.data_->getSize() &&
           std::equal(data_->getData(), data_->getData() + data_->getSize(), other.data_->getData());
}

LazyString LazyString::operator+(const char *str) {
    LazyString result(*this);
    result += str;
    return result;
}

LazyString &LazyString::operator+=(const char *str) {
    if (str == nullptr || strlen(str) == 0) {
        return *this;
    }

    SharedBuffer *extended = new SharedBuffer(data_->getSize() + strlen(str));
    std::copy(data_->getData(), data_->getData() + data_->getSize(), extended->getData());
    std::copy(str, str + strlen(str), extended->getData() + data_->getSize());

    delete data_;
    data_ = extended;
    return *this;
}

size_t LazyString::getSize() const { return data_->getSize(); }

const char *LazyString::cStr() const { return data_->getData(); }

size_t LazyString::useCount() const { return data_->useCount(); }

char LazyString::at(size_t index) const {
    if (index >= data_->getSize()) {
        throw std::out_of_range("index out of range");
    }

    return data_->getData()[index];
}

LazyString::CharWrapper::CharWrapper(LazyString *str, size_t index) : str_(str), index_(index) {}

LazyString::CharWrapper::operator char() const { return str_->at(index_); }

LazyString::CharWrapper &LazyString::CharWrapper::operator=(char c) {
    if (str_->useCount() > 1) {
        SharedBuffer *data = new SharedBuffer(str_->data_->getSize());
        std::copy(str_->data_->getData(), str_->data_->getData() + str_->data_->getSize(), data->getData());

        delete str_->data_;
        str_->data_ = data;
    }

    str_->data_->getData()[index_] = c;
    return *this;
}

LazyString::CharWrapper &LazyString::CharWrapper::operator++() {
    if (str_->useCount() > 1) {
        SharedBuffer *data = new SharedBuffer(str_->data_->getSize());
        std::copy(str_->data_->getData(), str_->data_->getData() + str_->data_->getSize(), data->getData());

        delete str_->data_;
        str_->data_ = data;
    }

    ++str_->data_->getData()[index_];
    return *this;
}

char LazyString::CharWrapper::operator++(int) {
    char result = str_->at(index_);
    ++(*this);
    return result;
}

bool LazyString::CharWrapper::operator==(char c) const { return str_->at(index_) == c; }

bool LazyString::CharWrapper::operator!=(char c) const { return str_->at(index_) != c; }

LazyString::CharWrapper LazyString::operator[](size_t index) { return CharWrapper(this, index); }
