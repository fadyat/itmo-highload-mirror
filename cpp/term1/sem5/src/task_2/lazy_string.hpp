#pragma once

#include "shared_buffer.hpp"
#include <cstddef>

class LazyString {
  private:
    SharedBuffer *data_;

    void swap(LazyString &other);

  public:
    LazyString(const char *str = nullptr);
    LazyString(const LazyString &other);
    LazyString &operator=(const LazyString &other);
    ~LazyString();

    bool operator==(const LazyString &other) const;
    LazyString operator+(const char *str);
    LazyString &operator+=(const char *str);

    size_t getSize() const;
    const char *cStr() const;
    size_t useCount() const;
    char at(size_t index) const;

    class CharWrapper {
      private:
        LazyString *str_;
        size_t index_;

      public:
        CharWrapper(LazyString *str, size_t index);
        CharWrapper &operator=(char c);

        operator char() const;
        CharWrapper &operator++();
        char operator++(int);

        bool operator==(char c) const;
        bool operator!=(char c) const;
    };

    CharWrapper operator[](size_t index);
};
