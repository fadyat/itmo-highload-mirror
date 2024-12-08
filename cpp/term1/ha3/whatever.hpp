#pragma once

#include <memory>
#include <type_traits>
#include <typeinfo>

namespace utils {

class bad_whatever_cast : public std::bad_cast {
  public:
    const char *what() const noexcept override { return "bad whatever cast"; }
};

class whatever {
  private:
    struct placeholder {
        virtual ~placeholder() = default;
        virtual const std::type_info &type() const = 0;
        virtual std::unique_ptr<placeholder> clone() const = 0;
    };

    template <typename T>
    struct holder : placeholder {
        T value;

        holder(const T &value) : value(value) {}
        const std::type_info &type() const override { return typeid(T); }
        std::unique_ptr<placeholder> clone() const override { return std::make_unique<holder<T>>(value); }
    };

    std::unique_ptr<placeholder> ptr;

  public:
    whatever() : ptr(nullptr) {}

    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, whatever>>>
    whatever(T &&value) : ptr(std::make_unique<holder<std::decay_t<T>>>(std::move(value))) {}

    whatever(const whatever &other) : ptr(other.empty() ? nullptr : other.ptr->clone()) {}

    template <typename T>
    whatever &operator=(T &value) {
        whatever tmp(value);
        swap(*this, tmp);
        return *this;
    }

    whatever &operator=(const whatever &other) {
        if (this != &other) {
            whatever tmp(other);
            swap(*this, tmp);
        }

        return *this;
    }

    friend void swap(whatever &a, whatever &b) { std::swap(a.ptr, b.ptr); }
    bool empty() const { return ptr == nullptr; }
    void clear() { ptr.reset(); }

    template <typename T>
    friend T *whatever_cast(whatever *w);

    template <typename T>
    friend const T *whatever_cast(const whatever *w);

    template <typename T>
    friend T whatever_cast(whatever &w);

    template <typename T>
    friend T whatever_cast(const whatever &w);
};

template <typename T>
T *whatever_cast(whatever *w) {
    if (w == nullptr || w->empty() || w->ptr->type() != typeid(T)) {
        return nullptr;
    }

    return &static_cast<whatever::holder<T> *>(w->ptr.get())->value;
}

template <typename T>
T whatever_cast(whatever &w) {
    auto ptr = whatever_cast<std::remove_reference_t<T>>(&w);
    return ptr ? *ptr : throw bad_whatever_cast();
}

template <typename T>
const T *whatever_cast(const whatever *w) {
    return whatever_cast<T>(const_cast<whatever *>(w));
}

template <typename T>
T whatever_cast(const whatever &w) {
    return whatever_cast<const std::remove_reference_t<T> &>(const_cast<whatever &>(w));
}

}
