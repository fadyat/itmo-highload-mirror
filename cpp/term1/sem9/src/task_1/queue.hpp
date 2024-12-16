#pragma once

#include <cstddef>
#include <memory>
#include <utility>

namespace cls09 {

template <typename T>
class queue {
  private:
    struct Node {
        T value;
        std::unique_ptr<Node> next;

        Node(const T &value, Node *nextNode = nullptr) : value(value), next(nextNode) {}
        Node(T &&value, Node *nextNode = nullptr) : value(std::move(value)), next(nextNode) {}
    };

    std::unique_ptr<Node> head_;
    Node *tail_;
    size_t size_;

    void clear() {
        while (head_ != nullptr) {
            head_ = std::move(head_->next);
        }

        tail_ = nullptr;
    }

  public:
    void push(const T &value) {
        auto new_node = std::make_unique<Node>(value, nullptr);
        Node *new_tail = new_node.get();

        if (head_ == nullptr) {
            head_ = std::move(new_node);
        } else {
            tail_->next = std::move(new_node);
        }

        tail_ = new_tail;
        size_++;
    }

    void push(T &&value) {
        auto new_node = std::make_unique<Node>(std::move(value), nullptr);
        Node *new_tail = new_node.get();

        if (head_ == nullptr) {
            head_ = std::move(new_node);
        } else {
            tail_->next = std::move(new_node);
        }

        tail_ = new_tail;
        size_++;
    }

    queue() : head_(nullptr), tail_(nullptr), size_(0) {}

    queue(const queue &other) : head_(nullptr), tail_(nullptr), size_(0) {
        for (Node *node = other.head_.get(); node != nullptr; node = node->next.get()) {
            push(node->value);
        }
    }

    queue(queue &&other) noexcept
        : head_(std::exchange(other.head_, nullptr)), tail_(std::exchange(other.tail_, nullptr)),
          size_(std::exchange(other.size_, 0)) {}

    ~queue() { clear(); }

    queue &operator=(const queue &other) {
        if (this == &other) {
            return *this;
        }

        queue tmp(other);
        std::swap(head_, tmp.head_);
        std::swap(tail_, tmp.tail_);
        std::swap(size_, tmp.size_);

        return *this;
    }

    queue &operator=(queue &&other) {
        if (this == &other) {
            return *this;
        }

        clear();
        head_ = std::exchange(other.head_, nullptr);
        tail_ = std::exchange(other.tail_, nullptr);
        size_ = std::exchange(other.size_, 0);

        return *this;
    }

    void pop() {
        head_ = std::move(head_->next);
        if (head_ == nullptr) {
            tail_ = nullptr;
        }

        size_--;
    }

    T &front() { return head_->value; }
    const T &front() const { return head_->value; }
    size_t size() const { return size_; }

  public:
    class const_iterator;

    class iterator {
      public:
        Node *current;

      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        iterator() = default;
        iterator(Node *node) : current(node) {}

        T &operator*() const { return current->value; }

        T *operator->() const { return &current->value; }

        iterator &operator++() {
            current = current->next.get();
            return *this;
        }

        bool operator==(const iterator &other) const { return current == other.current; }
        bool operator!=(const iterator &other) const { return current != other.current; }
        bool operator==(const const_iterator &other) const { return current == other.current; }
    };

    class const_iterator {
      public:
        const Node *current;

      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;

        const_iterator() = default;
        const_iterator(const Node *node) : current(node) {}
        const_iterator(const iterator &other) : current(other.current) {}

        const T &operator*() const { return current->value; }

        const T *operator->() const { return &current->value; }

        const_iterator &operator++() {
            current = current->next.get();
            return *this;
        }

        bool operator==(const const_iterator &other) const { return current == other.current; }
        bool operator!=(const const_iterator &other) const { return current != other.current; }
        bool operator==(const iterator &other) const { return current == other.current; }
    };

    iterator begin() { return iterator(head_.get()); }
    iterator end() { return iterator(nullptr); }

    const_iterator begin() const { return const_iterator(head_.get()); }
    const_iterator end() const { return const_iterator(nullptr); }

    const_iterator cbegin() const { return const_iterator(head_.get()); }
    const_iterator cend() const { return const_iterator(nullptr); }
};

}
