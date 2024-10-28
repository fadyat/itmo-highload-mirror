#pragma once

#include <cstddef>

class SharedBuffer {
  private:
    char *data_;
    size_t *refCount_;
    size_t size_;

    void swap(SharedBuffer &other) noexcept;

  public:
    SharedBuffer();
    explicit SharedBuffer(size_t size);
    SharedBuffer(const SharedBuffer &other);
    SharedBuffer &operator=(const SharedBuffer &other);
    ~SharedBuffer();

    size_t getSize() const;
    char *getData() const;
    size_t useCount() const;
};
