#include "functions.hpp"
#include "functions.hpp"

#include <vector>
#include <string>
#include <forward_list>

#include "../simple_test.h"

namespace {

struct Initial {};

struct Src {
    Src() = delete;
    Src(const Src&) = delete;
    Src& operator = (const Src&) = delete;

    explicit Src(Initial) {}
};

struct Dst {
    Dst() = delete;
    Dst(const Dst&) = delete;
    Dst& operator = (const Dst&) = delete;

    explicit Dst(Initial) {}
    Dst& operator = (const Src&) { return *this; }
};

}  // namespace


TEST(fill, simple) {
    int array[3] {};

    cls08::fill(std::begin(array), std::end(array), 10);
    for (auto elem : array) {
        EXPECT_EQ(10, elem);
    }

    // вырожденный случай
    cls08::fill(std::begin(array), std::begin(array), 20);
    for (auto elem : array) {
        EXPECT_EQ(10, elem);
    }
}

TEST(fill, geterogenous) {
    std::string array[3] = {};

    const char* const v = "abc";
    cls08::fill(std::begin(array), std::end(array), v);
    for (const auto& elem : array) {
        EXPECT_EQ(v, elem);
    }
}

TEST(fill, do_not_copy) {
    // тест на компилируемость.
    // убедимся, что мы случайно не выполняем никаких лишних действий
    // (конструкторы копирования)

    std::forward_list<Dst> list;  // итераторы с категорией ForwardIterator
    const Src value{Initial{}};
    cls08::fill(std::begin(list), std::end(list), value);
}

TEST(generate, simple) {
    int arr[4] {1, 2, 3, 4};

    cls08::generate(std::begin(arr), std::begin(arr) + 3, []()->int { return 0; });
    EXPECT_EQ(0, arr[0]);
    EXPECT_EQ(0, arr[1]);
    EXPECT_EQ(0, arr[2]);
    EXPECT_EQ(4, arr[3]);
}

TEST(generate, geterogenous) {
    std::string arr[4] = {"alfa", "beta", "gamma", "delta"};
    cls08::generate(std::begin(arr), std::begin(arr) + 3, []()->const char* { return "foo"; });
    EXPECT_EQ("foo", arr[0]);
    EXPECT_EQ("foo", arr[1]);
    EXPECT_EQ("foo", arr[2]);
    EXPECT_EQ("delta", arr[3]);
}

TEST(generate, proper_call_order) {
    // проверяем, что функциональный объект может быть любого типа (в данном случае - лямбда)
    // и что он вызывается в правильном порядке
    int arr[4] {};
    int counter = 100;
    cls08::generate(std::begin(arr), std::end(arr), [&counter]() { return counter++; });
    EXPECT_EQ(100, arr[0]);
    EXPECT_EQ(101, arr[1]);
    EXPECT_EQ(102, arr[2]);
    EXPECT_EQ(103, arr[3]);
    EXPECT_EQ(104, counter);
}

TEST(generate, forward_iterator_type) {
    // тест на компилируемость
    std::forward_list<int> list;
    cls08::generate(std::begin(list), std::end(list), []() { return 0; });
}

TEST(copy, simple) {
    const int src[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    int dst[8] = {};
    cls08::copy(src, src+4, dst+2);
    EXPECT_EQ(0, dst[0]);
    EXPECT_EQ(0, dst[1]);
    EXPECT_EQ(10, dst[2]);
    EXPECT_EQ(20, dst[3]);
    EXPECT_EQ(30, dst[4]);
    EXPECT_EQ(40, dst[5]);
    EXPECT_EQ(0, dst[6]);
    EXPECT_EQ(0, dst[7]);
}

TEST(copy, geterogenous) {
    const char* const src[] = {"3", "2", "1"};
    std::vector<std::string> dst;
    cls08::copy(std::rbegin(src), std::rend(src), std::back_inserter(dst));
    EXPECT_EQ((std::vector<std::string>{"1", "2", "3"}), dst);
}

TEST(copy, do_not_copy) {
    // тест на компилируемость.
    // убедимся, что мы случайно не выполняем никаких лишних действий
    // (конструкторы копирования)

    const std::forward_list<Src> src;
    std::forward_list<Dst> dst;
    cls08::copy(std::begin(src), std::end(src), std::begin(dst));
}

TEST(for_each, simple) {
    int arr[] {1, 2, 3};
    cls08::for_each(std::begin(arr), std::end(arr), [](int& x) { x *= 10; });
    EXPECT_EQ(10, arr[0]);
    EXPECT_EQ(20, arr[1]);
    EXPECT_EQ(30, arr[2]);
}

TEST(for_each, proper_call_order) {
    int arr[] {1, 2, 3};
    int counter = 1;
    cls08::for_each(
        std::begin(arr), std::end(arr),
        [&counter](int& x) {
            x = x*10 + counter; counter++;
        }
    );
    EXPECT_EQ(11, arr[0]);
    EXPECT_EQ(22, arr[1]);
    EXPECT_EQ(33, arr[2]);
    EXPECT_EQ(4, counter);
}

TEST(for_each, constant_series) {
    const int arr[] {1, 2, 3};
    int sum = 0;
    cls08::for_each(std::begin(arr), std::end(arr), [&sum](const int& x) { sum += x; });
    EXPECT_EQ(6, sum);
}

TEST(for_each, do_not_copy) {
    // тест на компилируемость.
    // убедимся, что мы случайно не выполняем никаких лишних действий
    // (конструкторы копирования)

    std::forward_list<Src> src;
    cls08::for_each(std::begin(src), std::end(src), [](Src&) {});
    cls08::for_each(std::begin(src), std::end(src), [](const Src&) {});
}

TESTING_MAIN()
