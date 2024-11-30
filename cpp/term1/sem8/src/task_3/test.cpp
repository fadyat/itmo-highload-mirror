#include "priority_queue.hpp"
#include "priority_queue.hpp"

#include <cstddef>
#include <utility>
#include <type_traits>
#include <string>

#include "../simple_test.h"


TEST(priority_queue, type_check) {
    using Element = std::string;
    using Queue = cls08::priority_queue<Element>;

    // default constructible, destructible
    Queue queue;
    const Queue& const_queue = queue;

    // copy constructible
    Queue queue2 = const_queue;
    // copy assignable
    queue2 = const_queue;

    // const Element& front() const
    static_assert(std::is_same_v<const Element&, decltype(const_queue.front())>);
    // нет неконстантой версии (или она тождественна константной)
    static_assert(std::is_same_v<const Element&, decltype(queue.front())>);
    // size_t size() const
    static_assert(std::is_same_v<size_t, decltype(const_queue.size())>);
}

TEST(priority_queue, empty) {
    cls08::priority_queue<int> queue;
    EXPECT_EQ(0u, queue.size());
}

TEST(priority_queue, repeated_values) {
    cls08::priority_queue<int> queue;

    ASSERT_EQ(0u, queue.size());
    for (size_t i = 0; i != 10; ++i) {
        queue.push(123);
        EXPECT_EQ(i+1, queue.size());
        ASSERT_TRUE(queue.size());
        EXPECT_EQ(123, queue.front()) << "at step " << i;
    }

    ASSERT_EQ(10u, queue.size());
    for (size_t i = 0; i != 10; ++i) {
        ASSERT_TRUE(queue.size());
        EXPECT_EQ(123, queue.front()) << "at step " << i;
        queue.pop();
        EXPECT_EQ(9-i, queue.size());
    }
}

TEST(priority_queue, ascending) {
    cls08::priority_queue<int> queue;

    size_t const N = 10;
    // входные данные
    int const inputs[N]  = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
    // максимальный элемент после очередной вставки
    int const nth_max[N] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
    // элементы в порядке извлечения
    int const ordered[N] = {90, 80, 70, 60, 50, 40, 30, 20, 10, 0};

    ASSERT_EQ(0u, queue.size());
    for (size_t i = 0; i != N; ++i) {
        queue.push(inputs[i]);
        EXPECT_EQ(i+1, queue.size());
        ASSERT_TRUE(queue.size());
        EXPECT_EQ(nth_max[i], queue.front()) << "at step " << i;
    }

    ASSERT_EQ(N, queue.size());
    for (size_t i = 0; i != N; ++i) {
        ASSERT_TRUE(queue.size());
        EXPECT_EQ(ordered[i], queue.front()) << "at step " << i;
        queue.pop();
        EXPECT_EQ(N-1-i, queue.size());
    }
}

TEST(priority_queue, descending) {
    cls08::priority_queue<int> queue;

    size_t const N = 10;
    // входные данные
    int const inputs[N]  = {90, 80, 70, 60, 50, 40, 30, 20, 10, 0};
    // максимальный элемент после очередной вставки
    int const nth_max[N] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90};
    // элементы в порядке извлечения
    int const ordered[N] = {90, 80, 70, 60, 50, 40, 30, 20, 10, 0};

    ASSERT_EQ(0u, queue.size());
    for (size_t i = 0; i != N; ++i) {
        queue.push(inputs[i]);
        EXPECT_EQ(i+1, queue.size());
        ASSERT_TRUE(queue.size());
        EXPECT_EQ(nth_max[i], queue.front()) << "at step " << i;
    }

    ASSERT_EQ(N, queue.size());
    for (size_t i = 0; i != N; ++i) {
        ASSERT_TRUE(queue.size());
        EXPECT_EQ(ordered[i], queue.front()) << "at step " << i;
        queue.pop();
        EXPECT_EQ(N-1-i, queue.size());
    }
}

TEST(priority_queue, random_order) {
    cls08::priority_queue<int> queue;

    size_t const N = 10;
    // входные данные
    int const inputs[N]  = {0, 10, 20, 80, 70, 60, 30, 40, 90, 50};
    // максимальный элемент после очередной вставки
    int const nth_max[N] = {0, 10, 20, 80, 80, 80, 80, 80, 90, 90};
    // элементы в порядке извлечения
    int const ordered[N] = {90, 80, 70, 60, 50, 40, 30, 20, 10, 0};

    ASSERT_EQ(0u, queue.size());
    for (size_t i = 0; i != N; ++i) {
        queue.push(inputs[i]);
        EXPECT_EQ(i+1, queue.size());
        ASSERT_TRUE(queue.size());
        EXPECT_EQ(nth_max[i], queue.front()) << "at step " << i;
    }

    ASSERT_EQ(N, queue.size());
    for (size_t i = 0; i != N; ++i) {
        ASSERT_TRUE(queue.size());
        EXPECT_EQ(ordered[i], queue.front()) << "at step " << i;
        queue.pop();
        EXPECT_EQ(N-1-i, queue.size());
    }
}

TEST(priority_queue, mixed_push_and_pop) {
    cls08::priority_queue<int> queue;
    for (int i = -10; i != 0; ++i) {
        queue.push(i);
    }
    ASSERT_EQ(10u, queue.size());
    ASSERT_EQ(-1, queue.front());  // -1, -2, ..., -10

    auto take = [&queue]() {
        ASSERT_TRUE(queue.size());
        int x = queue.front();
        queue.pop();
        return x;
    };

    // извлекаем два наибольших элемента и кладём обратно их сумму
    // и так, пока не кончится очередь
    int x, y;

    // -1, -2, -3, -4, -5, -6, -7, -8, -9, -10
    x = take(); EXPECT_EQ(-1, x);
    y = take(); EXPECT_EQ(-2, y);
    queue.push(x + y); EXPECT_EQ(9u, queue.size());
    // -3, -3, -4, -5, -6, -7, -8, -9, -10
    x = take(); EXPECT_EQ(-3, x);
    y = take(); EXPECT_EQ(-3, y);
    queue.push(x + y); EXPECT_EQ(8u, queue.size());
    // -4, -5, -6, -6, -7, -8, -9, -10
    x = take(); EXPECT_EQ(-4, x);
    y = take(); EXPECT_EQ(-5, y);
    queue.push(x + y); EXPECT_EQ(7u, queue.size());
    // -6, -6, -7, -8, -9, -9, -10
    x = take(); EXPECT_EQ(-6, x);
    y = take(); EXPECT_EQ(-6, y);
    queue.push(x + y); EXPECT_EQ(6u, queue.size());
    // -7, -8, -9, -9, -10, -12
    x = take(); EXPECT_EQ(-7, x);
    y = take(); EXPECT_EQ(-8, y);
    queue.push(x + y); EXPECT_EQ(5u, queue.size());
    // -9, -9, -10, -12, -15
    x = take(); EXPECT_EQ(-9, x);
    y = take(); EXPECT_EQ(-9, y);
    queue.push(x + y); EXPECT_EQ(4u, queue.size());
    // -10, -12, -15, -18
    x = take(); EXPECT_EQ(-10, x);
    y = take(); EXPECT_EQ(-12, y);
    queue.push(x + y); EXPECT_EQ(3u, queue.size());
    // -15, -18, -22
    x = take(); EXPECT_EQ(-15, x);
    y = take(); EXPECT_EQ(-18, y);
    queue.push(x + y); EXPECT_EQ(2u, queue.size());
    // -22, -33
    x = take(); EXPECT_EQ(-22, x);
    y = take(); EXPECT_EQ(-33, y);
    queue.push(x + y); EXPECT_EQ(1u, queue.size());
    // -55
    x = take(); EXPECT_EQ(-55, x);
    EXPECT_EQ(0u, queue.size());
}

TEST(priority_queue, strings) {
    cls08::priority_queue<std::string> queue;
    queue.push("foo");
    queue.push("bar");
    queue.push("buz");
    queue.push("buz");  // дважды
    queue.push("xyz");

    ASSERT_EQ(5u, queue.size());

    EXPECT_EQ("xyz", queue.front());
    queue.pop();
    EXPECT_EQ("foo", queue.front());
    queue.pop();
    EXPECT_EQ("buz", queue.front());
    queue.pop();
    EXPECT_EQ("buz", queue.front());
    queue.pop();
    EXPECT_EQ("bar", queue.front());
    queue.pop();
    EXPECT_EQ(0u, queue.size());
}

TEST(priority_queue, copy_contents) {
    cls08::priority_queue<int> q1;
    for (int i : {10, 20, 30, 40, 50}) q1.push(i);
    ASSERT_EQ(5u, q1.size());
    ASSERT_EQ(50, q1.front());

    // копирование
    cls08::priority_queue<int> q2 = q1;
    EXPECT_EQ(5u, q1.size());
    EXPECT_EQ(50, q1.front());

    EXPECT_EQ(5u, q2.size());
    EXPECT_EQ(50, q2.front());

    // данные у каждого независимы
    q1.pop();
    q1.pop();
    q2.push(60);
    EXPECT_EQ(3u, q1.size());
    EXPECT_EQ(30, q1.front());

    EXPECT_EQ(6u, q2.size());
    EXPECT_EQ(60, q2.front());

    // присваивание
    q2 = q1;
    EXPECT_EQ(3u, q1.size());
    EXPECT_EQ(30, q1.front());

    EXPECT_EQ(3u, q2.size());
    EXPECT_EQ(30, q2.front());
}

TESTING_MAIN()
