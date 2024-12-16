
#include "queue.hpp"

#include <iterator>
#include <iostream>

#include <numeric>
#include <type_traits>

#include "../simple_test.h"

using container_t = cls09::queue<int>;
using it_t = container_t::iterator;
using cit_t = container_t::const_iterator;


TEST(iterator, type_traits) {
    using it_traits = std::iterator_traits<it_t>;
    using cit_traits = std::iterator_traits<cit_t>;

    EXPECT_TRUE((std::is_same_v<it_traits::iterator_category, std::forward_iterator_tag>));
    EXPECT_TRUE((std::is_same_v<it_traits::value_type, int>));
    EXPECT_TRUE((std::is_same_v<it_traits::reference, int&>));
    EXPECT_TRUE((std::is_same_v<it_traits::pointer, int*>));
    EXPECT_TRUE((std::is_integral_v<it_traits::difference_type>));

    EXPECT_TRUE((std::is_same_v<cit_traits::iterator_category, std::forward_iterator_tag>));
    EXPECT_TRUE((std::is_same_v<cit_traits::value_type, int>));
    EXPECT_TRUE((std::is_same_v<cit_traits::reference, const int&>));
    EXPECT_TRUE((std::is_same_v<cit_traits::pointer, const int*>));
    EXPECT_TRUE((std::is_integral_v<cit_traits::difference_type>));

    EXPECT_TRUE((std::is_convertible_v<it_t, cit_t>));  // добавление константности - можно
    EXPECT_FALSE((std::is_convertible_v<cit_t, it_t>));  // снятие константности - нельзя

    EXPECT_TRUE((std::is_default_constructible_v<it_t>));
    EXPECT_TRUE((std::is_default_constructible_v<cit_t>));
    EXPECT_TRUE((std::is_copy_constructible_v<it_t>));
    EXPECT_TRUE((std::is_copy_constructible_v<cit_t>));
    EXPECT_TRUE((std::is_copy_assignable_v<it_t>));
    EXPECT_TRUE((std::is_copy_assignable_v<cit_t>));
}

TEST(iterator, begin_end_types) {
    container_t q;
    const auto& c = q;

    EXPECT_TRUE((std::is_same_v<decltype(q.begin()), it_t>));
    EXPECT_TRUE((std::is_same_v<decltype(q.end()), it_t>));
    EXPECT_TRUE((std::is_same_v<decltype(c.begin()), cit_t>));
    EXPECT_TRUE((std::is_same_v<decltype(c.end()), cit_t>));
    EXPECT_TRUE((std::is_same_v<decltype(c.cbegin()), cit_t>));
    EXPECT_TRUE((std::is_same_v<decltype(c.cend()), cit_t>));
}

TEST(iterator, empty) {
    container_t q;
    const auto& c = q;
    EXPECT_EQ(q.begin(), q.end());
    EXPECT_EQ(c.begin(), c.end());
    EXPECT_EQ(c.cbegin(), c.cend());
    EXPECT_EQ(q.begin(), c.begin());
    EXPECT_EQ(c.begin(), c.cbegin());

    EXPECT_EQ(0, std::distance(q.begin(), q.end()));
    EXPECT_EQ(0, std::distance(c.begin(), c.end()));
}

TEST(iterator, pointee) {
    struct XYZ {
        int x, y, z;
    };
    cls09::queue<XYZ> q;
    q.push(XYZ{1, 2, 3});
    ASSERT_EQ(1, q.size());
    ASSERT_NE(q.begin(), q.end());

    EXPECT_EQ(&q.front(), &*q.begin());

    EXPECT_EQ(1, q.begin()->x);
    EXPECT_EQ(2, std::as_const(q).begin()->y);
    EXPECT_EQ(3, q.cbegin()->z);
}

namespace {

// упрощённая реализация std::to_address, не требующая element_type

auto* to_address(auto it) {
    if constexpr (std::is_pointer_v<decltype(it)>) {
        return it;
    } else {
        return to_address(it.operator->());
    }
}

}  // namespace

TEST(iterator, keep_validity_with_push_and_pop) {
    container_t q;

    const int n = 5;

    // заполняем очередь
    {
        q.push(0);
        int* first_ptr = to_address(q.begin());
        EXPECT_NE(q.begin(), q.end());
        EXPECT_EQ(q.size(), std::distance(q.begin(), q.end()));

        for (int i = 1; i != n; ++i) {
            q.push(i);
            EXPECT_EQ(first_ptr, to_address(q.begin()));  // итератор начала не изменился
            EXPECT_EQ(q.size(), std::distance(q.begin(), q.end()));
        }
    }
    ASSERT_EQ(n, q.size());

    EXPECT_EQ(q.begin(), q.cbegin());
    EXPECT_EQ(q.end(), q.cend());

    // получаем содержимое всей очереди
    int* item_ptrs[n] = {};
    {
        auto it = q.begin();
        for (int i = 0; i != n; ++i, ++it) {
            item_ptrs[i] = to_address(it);
            ASSERT_NE(nullptr, item_ptrs[i]);
            EXPECT_EQ(i, *it);
            EXPECT_EQ(i, std::distance(q.begin(), it));
            EXPECT_EQ(n - i, std::distance(it, q.end()));
        }
    }

    // поэлементно удаляем
    {
        for (int i = 0; i != n; ++i) {
            ASSERT_EQ(n - i, q.size());
            ASSERT_EQ(q.size(), std::distance(q.begin(), q.end()));
            // проверяем, что хвост очереди остался неизменным
            auto it = q.begin();
            for (int j = i; j != n; ++j, ++it) {
                EXPECT_EQ(item_ptrs[j], to_address(it)) << i << " " << j;
            }
            q.pop();
        }
    }

    ASSERT_EQ(0, q.size());
    EXPECT_EQ(q.begin(), q.end());
}

TEST(iterator, stl_algorithms) {
    const int n = 5;

    container_t q;
    // заполним нулями
    for (int i = 0; i != n; ++i) {
        q.push(0);
    }

    EXPECT_EQ(0, std::accumulate(q.cbegin(), q.cend(), 0));

    std::iota(q.begin(), q.end(), 1);  // заполняем числами 1...n+1
    EXPECT_EQ((n + 1) * n / 2, std::accumulate(q.cbegin(), q.cend(), 0));
}

TEST(iterator, range_based_for) {
    const int n = 5;

    container_t q;
    // заполним нулями
    for (int i = 0; i != n; ++i) {
        q.push(0);
    }

    int t = 123;
    for (auto& v : q) {
        v = t++;
    }

    t = 123;
    for (const auto& v : std::as_const(q)) {
        EXPECT_EQ(t, v);
        t++;
    }
}

TESTING_MAIN()
