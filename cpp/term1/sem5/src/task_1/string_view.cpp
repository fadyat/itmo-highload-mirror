#include "string_view.hpp"
#include <cstring>

StringView::StringView(const char *str) : data_(str), size_(strlen(str)) {}

StringView::StringView(const std::string &str) : data_(str.data()), size_(str.size()) {}

StringView::StringView(const char *str, size_t size) : data_(str), size_(size) {}

StringView &StringView::operator=(const char *str) {
    data_ = str;
    size_ = strlen(str);
    return *this;
}

StringView &StringView::operator=(const std::string &str) {
    data_ = str.data();
    size_ = str.size();
    return *this;
}

StringView &StringView::operator=(const StringView &str) {
    data_ = str.data_;
    size_ = str.size_;
    return *this;
}

const char *StringView::data() const { return data_; }

size_t StringView::size() const { return size_; }

StringView StringView::substr(size_t start, size_t end) const {
    start = std::min(start, size_);
    end = std::min(end, size_);

    if (start > end) {
        return StringView(data_ + start, 0);
    }

    return StringView(data_ + start, end - start);
}

const char &StringView::operator[](size_t idx) const { return data_[idx]; }

bool StringView::operator==(const StringView &str) const { return size_ == str.size_ && memcmp(data_, str.data_, size_) == 0; }

size_t StringView::find(const StringView &str) const {
    if (str.size_ == 0) {
        return 0;
    }

    if (size_ < str.size_) {
        return npos;
    }

    for (size_t i = 0; i < size_ - str.size_ + 1; ++i) {
        if (substr(i, i + str.size_) == str) {
            return i;
        }
    }

    return npos;
}

size_t StringView::find(const char str) const { return find(StringView(&str, 1)); }

bool StringView::startsWith(const StringView &str) const { return substr(0, str.size_) == str; }

bool StringView::startsWith(const char str) const { return startsWith(StringView(&str, 1)); }

bool StringView::endsWith(const StringView &str) const { return substr(size_ - str.size_, size_) == str; }

bool StringView::endsWith(const char str) const { return endsWith(StringView(&str, 1)); }

size_t StringView::findFirstOf(const StringView &str, size_t start) const {
    for (size_t i = start; i < size_; ++i) {
        if (str.find(data_[i]) != npos) {
            return i;
        }
    }

    return npos;
}

size_t StringView::findFirstOf(const char str, size_t start) const { return findFirstOf(StringView(&str, 1), start); }

size_t StringView::findFirstNotOf(const StringView &str, size_t start) const {
    for (size_t i = start; i < size_; ++i) {
        if (str.find(data_[i]) == npos) {
            return i;
        }
    }

    return npos;
}

size_t StringView::findFirstNotOf(const char str, size_t start) const { return findFirstNotOf(StringView(&str, 1), start); }

void StringView::removePrefix(size_t count) {
    data_ += count;
    size_ -= count;
}
