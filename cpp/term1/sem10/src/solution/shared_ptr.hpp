#pragma once

#include <cstddef>
#include <utility>

namespace cls10 {

template <typename T>
class shared_ptr {
   private:
    T *ptr;
    size_t *ref_count;

    void release() {
        if (ref_count != nullptr) {
            --(*ref_count);
            if (*ref_count == 0) {
                delete ptr;
                delete ref_count;
            }
        }

        ptr = nullptr;
        ref_count = nullptr;
    }

   public:
    shared_ptr() noexcept : ptr(nullptr), ref_count(nullptr) {}
    shared_ptr(std::nullptr_t) noexcept : shared_ptr() {}
    explicit shared_ptr(T *ptr) : ptr(ptr), ref_count(new size_t(1)) {}

    shared_ptr(const shared_ptr &other) : ptr(other.ptr), ref_count(other.ref_count) {
        if (ref_count) {
            ++(*ref_count);
        }
    }
    shared_ptr &operator=(const shared_ptr &other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            if (ref_count) {
                ++(*ref_count);
            }
        }

        return *this;
    }

    shared_ptr(shared_ptr &&other) noexcept
        : ptr(std::exchange(other.ptr, nullptr)), ref_count(std::exchange(other.ref_count, nullptr)) {}

    shared_ptr &operator=(shared_ptr &&other) noexcept {
        if (this != &other) {
            release();
            ptr = std::exchange(other.ptr, nullptr);
            ref_count = std::exchange(other.ref_count, nullptr);
        }

        return *this;
    }

    ~shared_ptr() { release(); }

    T *get() const { return ptr; }
    T &operator*() const { return *ptr; }
    T *operator->() const { return ptr; }

    size_t use_count() const { return ref_count ? *ref_count : 0; }
    explicit operator bool() const { return ptr != nullptr; }

    void reset(T *new_ptr = nullptr) {
        release();
        ptr = new_ptr;
        ref_count = new_ptr ? new size_t(1) : nullptr;
    }
};

template <typename T, typename... Args>
inline shared_ptr<T> make_shared(Args &&...args) {
    return shared_ptr<T>(new T(std::forward<Args>(args)...));
}

}
