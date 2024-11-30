#pragma once

#include <algorithm>
#include <functional>
#include <vector>

namespace cls08 {

template <typename T, typename Comparator = std::less<T>>
class priority_queue {
  private:
    std::vector<T> data;
    Comparator comparator;

  public:
    priority_queue() = default;
    explicit priority_queue(const Comparator &comparator) : comparator(comparator) {}

    priority_queue(const priority_queue &other) = default;
    priority_queue &operator=(const priority_queue &other) = default;
    ~priority_queue() = default;

    void push(const T &value) {
        data.push_back(value);
        std::push_heap(data.begin(), data.end(), comparator);
    }

    void pop() {
        std::pop_heap(data.begin(), data.end(), comparator);
        data.pop_back();
    }

    const T &front() const { return data.front(); }

    size_t size() const { return data.size(); }
};

}
