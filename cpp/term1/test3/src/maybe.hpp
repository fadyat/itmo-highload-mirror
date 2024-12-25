#pragma once

#include <type_traits>
#include <utility>

namespace control_03 {

inline constexpr std::in_place_t in_place_t{};

template <typename T>
class maybe {
   private:
    alignas(T) unsigned char storage_[sizeof(T)];
    bool has_value_ = false;

    T* ptr() noexcept { return reinterpret_cast<T*>(&storage_); }
    const T* ptr() const noexcept { return reinterpret_cast<const T*>(&storage_); }

    void destroy() noexcept {
        if (has_value_) {
            ptr()->~T();
            has_value_ = false;
        }
    }

   public:
    maybe() noexcept = default;
    ~maybe() { destroy(); }

    template <typename... Args>
    maybe(std::in_place_t, Args&&... args) {
        new (ptr()) T(std::forward<Args>(args)...);
        has_value_ = true;
    }

    maybe(const maybe& other) {
        if (other.has_value()) {
            new (ptr()) T(*other.ptr());
            has_value_ = true;
        }
    }

    maybe(maybe&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if (other.has_value()) {
            new (ptr()) T(std::move(*other.ptr()));
            has_value_ = true;
            other.destroy();
        }
    }

    template <typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
    maybe(U&& value) : has_value_(true) {
        new (ptr()) T(std::forward<U>(value));
    }

    maybe& operator=(const maybe& other) {
        if (this != &other) {
            if (other.has_value()) {
                if (has_value()) {
                    *ptr() = *other.ptr();
                } else {
                    new (ptr()) T(*other.ptr());
                    has_value_ = true;
                }
            } else {
                destroy();
            }
        }

        return *this;
    }

    maybe& operator=(maybe&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
        if (this != &other) {
            if (other.has_value()) {
                if (has_value()) {
                    *ptr() = std::move(*other.ptr());
                } else {
                    new (ptr()) T(std::move(*other.ptr()));
                    has_value_ = true;
                }

                other.destroy();
            } else {
                destroy();
            }
        }

        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_constructible_v<T, U>>>
    maybe& operator=(U&& value) {
        if (has_value()) {
            *ptr() = std::forward<U>(value);
        } else {
            new (ptr()) T(std::forward<U>(value));
            has_value_ = true;
        }

        return *this;
    }

    T& value() { return *ptr(); }
    const T& value() const { return *ptr(); }

    bool has_value() const noexcept { return has_value_; }
    explicit operator bool() const noexcept { return has_value(); }

    void reset() noexcept { destroy(); }

    template <typename U>
    void reset(U&& value) {
        destroy();
        new (ptr()) T(std::forward<U>(value));
        has_value_ = true;
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        destroy();
        new (ptr()) T(std::forward<Args>(args)...);
        has_value_ = true;
    }
};

template <typename T>
class maybe<T*> {
   private:
    T* ptr = nullptr;

   public:
    maybe() noexcept = default;
    maybe(std::nullptr_t) noexcept : ptr(nullptr) {}
    ~maybe() = default;

    maybe(const maybe& other) noexcept : ptr(other.ptr) {}
    maybe(maybe&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }

    maybe(T* value) noexcept : ptr(value) {}

    maybe& operator=(const maybe& other) noexcept {
        ptr = other.ptr;
        return *this;
    }

    maybe& operator=(maybe&& other) noexcept {
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

    maybe& operator=(T* value) noexcept {
        ptr = value;
        return *this;
    }

    T* value() { return ptr; }
    T* value() const { return ptr; }

    bool has_value() const noexcept { return ptr != nullptr; }
    explicit operator bool() const noexcept { return ptr != nullptr; }

    void reset() noexcept { ptr = nullptr; }
    void reset(T* value) noexcept { ptr = value; }
};

}
