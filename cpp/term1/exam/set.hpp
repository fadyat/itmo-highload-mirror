#pragma once

#include <functional>
#include <memory>
#include <utility>

namespace {

template <typename T>
struct node {
    T val_;
    std::unique_ptr<node> left;
    std::unique_ptr<node> right;

    explicit node(const T& val) : val_(val) {}
    explicit node(T&& val) : val_(std::move(val)) {}
};

template <typename T>
struct element {
    using value_type = T;
    using reference = const T&;
    using const_reference = const T&;
    using pointer = const T*;
    using const_pointer = const T*;

    pointer node_;

    element(pointer ptr = nullptr) : node_(ptr) {}

    reference operator*() const { return *node_; }
    pointer operator->() const { return node_; }

    bool operator==(const element& other) const { return node_ == other.node_; }
    bool operator!=(const element& other) const { return !(*this == other); }
};

}

namespace exam {

template <typename TKey, typename Compare = std::less<TKey>>
class set {
   private:
    std::unique_ptr<node<TKey>> root_;
    size_t size_;
    Compare comp_;

    std::unique_ptr<node<TKey>> deep_copy(const std::unique_ptr<node<TKey>>& r) {
        if (!r) {
            return nullptr;
        }

        std::unique_ptr<node<TKey>> new_node = std::make_unique<node<TKey>>(r->val_);
        new_node->left = deep_copy(r->left);
        new_node->right = deep_copy(r->right);
        return new_node;
    }

   public:
    using key_type = TKey;
    using value_type = TKey;
    using size_type = size_t;
    using element = element<TKey>;

    set() : root_(nullptr), size_(0), comp_() {}
    explicit set(const Compare& comp) : root_(nullptr), size_(0), comp_(comp) {}

    ~set() { clear(); }

    set(const set& other) : root_(deep_copy(other.root_)), size_(other.size_), comp_(other.comp_) {}
    set(set&& other) : root_(std::move(other.root_)), size_(other.size_), comp_(std::move(other.comp_)) {
        other.size_ = 0;
    }

    set& operator=(const set& other) {
        if (this == &other) {
            return *this;
        }

        root_ = deep_copy(other.root_);
        size_ = other.size_;
        comp_ = other.comp_;
        return *this;
    }

    set& operator=(set&& other) {
        if (this == &other) {
            return *this;
        }

        root_ = std::move(other.root_);
        size_ = other.size_;
        comp_ = std::move(other.comp_);
        other.size_ = 0;
        return *this;
    }

    void clear() {
        root_.reset();
        size_ = 0;
    }

    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    element end() const { return element(); }

    element find(const key_type& key) const {
        node<TKey>* node = root_.get();
        while (node) {
            if (comp_(key, node->val_)) {
                node = node->left.get();
            } else if (comp_(node->val_, key)) {
                node = node->right.get();
            } else {
                return element(&node->val_);
            }
        }

        return end();
    }

    template <typename TKey_ = TKey>
    std::pair<element, bool> insert(TKey_&& value) {
        node<TKey>* current = root_.get();
        node<TKey>* parent = nullptr;
        while (current) {
            parent = current;
            if (comp_(value, current->val_)) {
                current = current->left.get();
            } else if (comp_(current->val_, value)) {
                current = current->right.get();
            } else {
                return {element(&current->val_), false};
            }
        }

        std::unique_ptr<node<TKey>> new_node = std::make_unique<node<TKey>>(std::forward<TKey_>(value));
        if (!parent) {
            root_ = std::move(new_node);
        } else if (comp_(value, parent->val_)) {
            parent->left = std::move(new_node);
        } else {
            parent->right = std::move(new_node);
        }

        ++size_;
        return {find(value), true};
    }

    friend void swap(set& lhs, set& rhs) {
        std::swap(lhs.root_, rhs.root_);
        std::swap(lhs.size_, rhs.size_);
        std::swap(lhs.comp_, rhs.comp_);
    }
};

}
