#pragma once

#include <memory>
#include <utility>

namespace cls10 {

template <typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
   private:
    T *ptr;
    Deleter deleter;

   public:
    unique_ptr() noexcept : ptr(nullptr) {}
    unique_ptr(std::nullptr_t) noexcept : ptr(nullptr) {}

    unique_ptr(const unique_ptr &other) = delete;
    unique_ptr &operator=(const unique_ptr &other) = delete;

    explicit unique_ptr(T *ptr) : ptr(ptr) {}
    unique_ptr(unique_ptr &&other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {}
    unique_ptr &operator=(unique_ptr &&other) noexcept {
        if (this != &other) {
            reset(other.release());
        }

        return *this;
    }

    ~unique_ptr() { reset(); }

    T &operator*() const { return *ptr; }
    T *operator->() const { return ptr; }
    T *get() const { return ptr; }

    explicit operator bool() const { return ptr != nullptr; }

    T *release() { return std::exchange(ptr, nullptr); }
    void reset(T *new_ptr = nullptr) { deleter(std::exchange(ptr, new_ptr)); }
    void swap(unique_ptr &other) noexcept { std::swap(ptr, other.ptr); }
};

template <typename T, typename... Args>
inline unique_ptr<T> make_unique(Args &&...args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}
