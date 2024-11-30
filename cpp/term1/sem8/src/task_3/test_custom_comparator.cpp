#include "priority_queue.hpp"
#include "priority_queue.hpp"

#include <cstddef>
#include <type_traits>
#include <functional>
#include <string>

#include "../simple_test.h"

TEST(priority_queue_with_comparator, type_check) {
    using Element = std::string;
    using Comparator = std::greater<>;
    using Queue = cls08::priority_queue<Element, Comparator>;

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

TEST(priority_queue_with_comparator, explicit_less) {
    cls08::priority_queue<std::string, std::less<>> queue;
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

TEST(priority_queue_with_comparator, greater) {
    cls08::priority_queue<std::string, std::greater<>> queue;
    queue.push("foo");
    queue.push("bar");
    queue.push("buz");
    queue.push("buz");  // дважды
    queue.push("xyz");

    ASSERT_EQ(5u, queue.size());

    EXPECT_EQ("bar", queue.front());
    queue.pop();
    EXPECT_EQ("buz", queue.front());
    queue.pop();
    EXPECT_EQ("buz", queue.front());
    queue.pop();
    EXPECT_EQ("foo", queue.front());
    queue.pop();
    EXPECT_EQ("xyz", queue.front());
    queue.pop();
    EXPECT_EQ(0u, queue.size());
}

TEST(priority_queue_with_comparator, comparator_with_args) {
    // трюк с шаблонной лямбдой, чтобы корректно компилировался if constexpr
    [](auto) {
        using Comparator = std::function<bool(const std::string&, const std::string&)>;
        using Queue = cls08::priority_queue<std::string, Comparator>;

        EXPECT_TRUE((std::is_constructible_v<Queue, Comparator>))
            << "нужен конструктор с аргументом: Queue queue {cmp};";
        EXPECT_FALSE((std::is_convertible_v<Comparator, Queue>))
            << "конструктор должен быть explicit, неявные приведения запрещены: Queue queue = cmp;";

        if constexpr(std::is_constructible_v<Queue, Comparator>) {
            Queue queue{ [](const auto& x, const auto& y) { return x[1] < y[1]; } };

            queue.push("foo");
            queue.push("bar");
            queue.push("buz");
            queue.push("buz");  // дважды
            queue.push("xyz");

            ASSERT_EQ(5u, queue.size());

            EXPECT_EQ("xyz", queue.front());  // 'y'
            queue.pop();
            EXPECT_EQ("buz", queue.front());  // 'u'
            queue.pop();
            EXPECT_EQ("buz", queue.front());  // 'u'
            queue.pop();
            EXPECT_EQ("foo", queue.front());  // 'o'
            queue.pop();
            EXPECT_EQ("bar", queue.front());  // 'a'
            queue.pop();
            EXPECT_EQ(0u, queue.size());
        }
    } (0);
}

TEST(priority_queue_with_comparator, comparator_with_args_copy_contents) {
    // трюк с шаблонной лямбдой, чтобы корректно компилировался if constexpr
    [](auto) {
        using Comparator = std::function<bool(const std::string&, const std::string&)>;
        using Queue = cls08::priority_queue<std::string, Comparator>;

        EXPECT_TRUE((std::is_constructible_v<Queue, Comparator>))
            << "нужен конструктор с аргументом: Queue queue {cmp};";
        EXPECT_FALSE((std::is_convertible_v<Comparator, Queue>))
            << "конструктор должен быть explicit, неявные приведения запрещены: Queue queue = cmp;";
        EXPECT_TRUE((std::is_copy_constructible_v<Queue>)) << "нужен конструктор копирования";
        EXPECT_TRUE((std::is_copy_assignable_v<Queue>)) << "нужен оператор присваивания";

        if constexpr(std::is_constructible_v<Queue, Comparator>) {
            Queue q1{ [](const auto& x, const auto& y) { return x[1] < y[1]; } };
            q1.push("foo");
            q1.push("bar");
            ASSERT_EQ(2u, q1.size());

            Queue q2 = q1;  // копирование
            q2.push("buz");
            ASSERT_EQ(3u, q2.size());

            // очередь с другим компаратором и другими данными
            bool allowed = true;
            Queue q3{
                [&allowed](const auto& x, const auto& y) {
                    EXPECT_TRUE(allowed);
                    return x > y;
                }
            };
            q3.push("foobarbuz");
            q3.push("buzbarfoo");
            EXPECT_EQ(2u, q3.size());

            q3 = q2;  // присваивание - в том числе, и компаратора!
            // с этого момента другой компаратор не используется
            allowed = false;

            q3.push("buz");  // дважды
            q3.push("xyz");
            ASSERT_EQ(5u, q3.size());

            ASSERT_EQ(2u, q1.size());
            EXPECT_EQ("foo", q1.front());  // 'o'
            q1.pop();
            EXPECT_EQ("bar", q1.front());  // 'a'
            q1.pop();
            EXPECT_EQ(0u, q1.size());

            ASSERT_EQ(3u, q2.size());
            EXPECT_EQ("buz", q2.front());  // 'u'
            q2.pop();
            EXPECT_EQ("foo", q2.front());  // 'o'
            q2.pop();
            EXPECT_EQ("bar", q2.front());  // 'a'
            q2.pop();
            EXPECT_EQ(0u, q2.size());

            ASSERT_EQ(5u, q3.size());
            EXPECT_EQ("xyz", q3.front());  // 'y'
            q3.pop();
            EXPECT_EQ("buz", q3.front());  // 'u'
            q3.pop();
            EXPECT_EQ("buz", q3.front());  // 'u'
            q3.pop();
            EXPECT_EQ("foo", q3.front());  // 'o'
            q3.pop();
            EXPECT_EQ("bar", q3.front());  // 'a'
            q3.pop();
            EXPECT_EQ(0u, q3.size());
        }
    } (0);
}


TESTING_MAIN()
