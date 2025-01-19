#include "set.hpp"
#include "set.hpp"

#include <cassert>
#include <type_traits>
#include <functional>
#include <concepts>

#include "simple_test.h"

// Раскомментируйте для запуска тестов
// #define SET_BASIC_ONLY  // только первая часть, без SET_DEEP_COPY, SET_MOVES и т.д.
// #define SET_BASIC
// #define SET_DEEP_COPY
// #define SET_MOVES
// #define SET_WITH_COMPARATOR
// #define SET_ITERATION
// #define SET_ERASE

// Задание нужно выполнять последовательно:
// 1) базовая часть
// 2) добавить копирование и перемещение
// 3) добавить компаратор
// 4) добавить итератор
// 5) добавить erase

// Но в отладочных целях можно частично отключать подзадачи.
// В таком случае закомментируйте #define внутри этих #ifdef-ов

#ifdef SET_WITH_COMPARATOR
#define SET_COMPARATOR_WITH_DEEP_COPY
#define SET_COMPARATOR_WITH_MOVES
#endif

#ifdef SET_ITERATION
#define SET_ITERATION_WITH_DEEP_COPY
#define SET_ITERATION_WITH_MOVES
#define SET_ITERATION_WITH_COMPARATOR
#endif

#ifdef SET_ERASE
#define SET_ERASE_WITH_COMPARATOR
#define SET_ERASE_WITH_ELEMENT
#define SET_ERASE_WITH_ITERATION
#endif

////////////////////////////////////////////////////////////////////////////////

// для CI-тестов
#ifdef SET_BASIC_ONLY
#define SET_BASIC
#endif

////////////////////////////////////////////////////////////////////////////////

// этот хак устроен следующим образом:
// condition передаётся как параметр шаблонной лямбды, что позволяет внутри
// делать отложенную условную компиляцию:
// - if constexpr включает-выключает блоки кода
// - шаблоны, зависящие от COMPILE_TIME_CONDITION, инстанцируются только при использовании
//   (то есть, строго внутри if-constexpr)
#define COMPILE_TIME_CONDITION_BEGIN(condition)                  \
    {                                                            \
        constexpr bool the_compile_time_condtion = (condition);  \
        ([]<bool COMPILE_TIME_CONDITION>() {                     \
            if constexpr (COMPILE_TIME_CONDITION) {              \
    // end macro

#define COMPILE_TIME_CONDITION_END()                             \
            } else {                                             \
                FAIL() << "feature not supported";               \
            }                                                    \
        }.operator()<(the_compile_time_condtion)>());            \
    }                                                            \
    // end macro

////////////////////////////////////////////////////////////////////////////////

namespace compile_time {

/// умеем ли копировать и перемещать?

template<class S> constexpr bool has_copy_helper =
    std::is_copy_constructible_v<S> && std::is_copy_assignable_v<S>;

template<class S> constexpr bool has_move_helper =
    std::is_move_constructible_v<S> && std::is_move_assignable_v<S>;

[[maybe_unused]]
constexpr bool has_copy = has_copy_helper<exam::set<int>>;

[[maybe_unused]]
constexpr bool has_move = has_move_helper<exam::set<int>>;

/// можно ли указывать компаратор параметром шаблона?

template< template<typename, typename...> class ST>
struct has_custom_comparator_helper : std::false_type {};

template< template<typename, typename...> class ST>
requires requires { ST<int, std::less<>>(); }
struct has_custom_comparator_helper<ST> : std::true_type {};

[[maybe_unused]]
constexpr bool has_custom_comparator = has_custom_comparator_helper<exam::set>();

/// умеем ли итерировать?

template<class S> constexpr bool has_iterator_helper =
    requires (typename S::element& e) { ++e; e++; };

[[maybe_unused]]
constexpr bool has_iterator = has_iterator_helper<exam::set<int>>;

/// есть ли erase?

template<class S> constexpr bool has_erase_by_value_helper =
    requires (S& s, const typename S::value_type& v) { s.erase(v); };

template<class S> constexpr bool has_erase_by_element_helper =
    requires (S& s, const typename S::element& e) { s.erase(e); };

[[maybe_unused]]
constexpr bool has_erase_by_value = has_erase_by_value_helper<exam::set<int>>;

[[maybe_unused]]
constexpr bool has_erase_by_element = has_erase_by_element_helper<exam::set<int>>;

}  // namespace compile_time

////////////////////////////////////////////////////////////////////////////////

// этот макрос определяет тип внутри скобок COMPILE_TIME_CONDITION_BEGIN....END
// трюк состоит в том, что этот шаблон фиктивно зависит от условия компиляции
// и поэтому не приводит к немедленной ошибке

template<template<typename...> class TEMPLATE, class... Args>
using bind_template_params_t = TEMPLATE<Args...>;

template<bool Enabled> struct maybe_set {
    template<class... Args> using type = bind_template_params_t<exam::set, Args...>;
};

#define EXAM_SET /* <V, C> */ typename maybe_set<COMPILE_TIME_CONDITION>::template type /* <V, C> */

// по смыслу это exam::set<V, C>

////////////////////////////////////////////////////////////////////////////////

#ifdef SET_BASIC

TEST(set, basic_type_traits) {
    using S = exam::set<int>;
    const S cs;

    EXPECT_TRUE((std::is_same_v<size_t, decltype(cs.size())>));

    EXPECT_TRUE((std::is_same_v<int, S::key_type>));
    EXPECT_TRUE((std::is_same_v<int, S::value_type>));
    EXPECT_TRUE((std::is_same_v<size_t, S::size_type>));

    EXPECT_TRUE((std::is_same_v<S::element, decltype(cs.end())>));

    using E = S::element;
    EXPECT_TRUE((std::is_default_constructible_v<E>));
    EXPECT_TRUE((std::is_copy_constructible_v<E>));
    EXPECT_TRUE((std::is_copy_assignable_v<E>));
    EXPECT_TRUE((std::is_same_v<int, E::value_type>));
    EXPECT_TRUE((std::is_same_v<const int&, E::reference>));
    EXPECT_TRUE((std::is_same_v<const int&, E::const_reference>));
    EXPECT_TRUE((std::is_same_v<const int*, E::pointer>));
    EXPECT_TRUE((std::is_same_v<const int*, E::const_pointer>));
    EXPECT_TRUE(( requires(const E& e1, const E& e2) { {e1 == e2} -> std::same_as<bool>; } ));
    EXPECT_TRUE(( requires(const E& e1, const E& e2) { {e1 != e2} -> std::same_as<bool>; } ));
    EXPECT_TRUE(( requires(const E& e) { {*e} -> std::same_as<const int&>; }));
}

TEST(basic, default_element) {
    exam::set<int>::element e1, e2;
    EXPECT_TRUE(e1 == e2);
    EXPECT_FALSE(e1 != e2);
}

#ifdef SET_BASIC_ONLY
TEST(set, basic_only_no_copy) {
    using S = exam::set<int>;
    EXPECT_FALSE((std::is_copy_constructible_v<S>));
    EXPECT_FALSE((std::is_copy_assignable_v<S>));
}
#endif  // SET_BASIC_ONLY

TEST(basic, empty) {
    const exam::set<int> cs;
    EXPECT_TRUE(cs.empty());
    EXPECT_EQ(0u, cs.size());

    EXPECT_TRUE(cs.end() == cs.end());

    auto e = cs.find(100);
    EXPECT_TRUE(e == cs.end());
    EXPECT_FALSE(e != cs.end());
}

TEST(basic, insert_one) {
    using S = exam::set<int>;
    S s;

    const int key = 100;

    auto [e, ok] = s.insert(key);
    EXPECT_TRUE(ok);
    EXPECT_FALSE(e == s.end());
    EXPECT_TRUE(e != s.end());
    EXPECT_EQ(key, *e);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(1u, s.size());

    EXPECT_TRUE(s.end() == s.end());

    auto [e1, ok1] = s.insert(key);
    EXPECT_FALSE(ok1);
    EXPECT_TRUE(e == e1);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(1u, s.size());

    auto e2 = s.find(key);
    EXPECT_TRUE(e == e2);

    EXPECT_TRUE(s.find(key + 1) == s.end());
    EXPECT_TRUE(s.find(key - 1) == s.end());
}

TEST(basic, insert_ascending) {
    using S = exam::set<int>;
    S s;

    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        auto [e, ok] = s.insert(k);
        EXPECT_TRUE(ok) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;
        EXPECT_TRUE(e == s.find(k)) << "key=" << k;
    }
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(7u, s.size());

    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        auto [e, ok] = s.insert(k);
        EXPECT_FALSE(ok) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;
        EXPECT_TRUE(e == s.find(k)) << "key=" << k;
        EXPECT_EQ(7u, s.size());
    }

    for (int k : {5, 15, 25, 35, 45, 65, 75}) {
        EXPECT_TRUE(s.find(k) == s.end()) << "key=" << k;
    }

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(0u, s.size());
    EXPECT_TRUE(s.find(10) == s.end());
    EXPECT_TRUE(s.find(70) == s.end());
}

TEST(basic, insert_descending) {
    using S = exam::set<int>;
    S s;

    for (int k : {70, 60, 50, 40, 30, 20, 10}) {
        auto [e, ok] = s.insert(k);
        EXPECT_TRUE(ok) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;
        EXPECT_TRUE(e == s.find(k)) << "key=" << k;
    }
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(7u, s.size());

    for (int k : {10, 20, 30, 40, 50}) {
        auto [e, ok] = s.insert(k);
        EXPECT_FALSE(ok) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;
        EXPECT_TRUE(e == s.find(k)) << "key=" << k;
        EXPECT_EQ(7u, s.size());
    }

    for (int k : {5, 15, 25, 35, 45, 65, 75}) {
        EXPECT_TRUE(s.find(k) == s.end()) << "key=" << k;
    }

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(0u, s.size());
    EXPECT_TRUE(s.find(10) == s.end());
    EXPECT_TRUE(s.find(70) == s.end());
}

TEST(basic, insert_random) {
    using S = exam::set<int>;
    S s;

    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        auto [e, ok] = s.insert(k);
        EXPECT_TRUE(ok) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;
        EXPECT_TRUE(e == s.find(k)) << "key=" << k;
    }
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(7u, s.size());

    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        auto [e, ok] = s.insert(k);
        EXPECT_FALSE(ok) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;
        EXPECT_TRUE(e == s.find(k)) << "key=" << k;
    }
    EXPECT_EQ(7u, s.size());

    for (int k : {5, 15, 25, 35, 45, 65, 75}) {
        EXPECT_TRUE(s.find(k) == s.end()) << "key=" << k;
    }

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(0u, s.size());
    EXPECT_TRUE(s.find(10) == s.end());
    EXPECT_TRUE(s.find(70) == s.end());
}

TEST(basic, copyable_key) {
    struct Key {
        int q;

        explicit Key(int q) : q(q) {}

        Key(const Key&) = default;
        Key(Key&& other) : q(std::exchange(other.q, 0)) {}

        Key& operator=(Key&) = delete;

        bool operator<(const Key& other) const { return q < other.q; }
    };

    exam::set<Key> s;
    Key k{1};
    s.insert(k);
    EXPECT_EQ(1, k.q);
    ASSERT_TRUE(s.find(k) != s.end());
    EXPECT_EQ(1, s.find(k)->q);
}

TEST(basic, moveable_key) {
    struct Key {
        int q;

        explicit Key(int q) : q(q) {}
        Key(Key&&) = default;

        Key& operator=(Key&) = delete;

        bool operator<(const Key& other) const { return q < other.q; }
    };

    exam::set<Key> s;
    s.insert(Key{1});
    ASSERT_TRUE(s.find(Key{1}) != s.end());
    EXPECT_EQ(1, s.find(Key{1})->q);
}

TEST(basic, superheavy_key) {
    struct Key {
        char buf[10000];
        bool operator<(const Key&) const { return true; }
    };
    // попробуйте не хранить непосредственно в объекте ничего лишнего
    EXPECT_LT(sizeof(exam::set<Key>), sizeof(Key));
}

TEST(basic, swap) {
    using S = exam::set<int>;
    S s1, s2;

    for (int k : {1, 2, 3}) {
        s1.insert(k);
    }
    for (int k : {3, 4, 5, 6}) {
        s2.insert(k);
    }
    ASSERT_EQ(3u, s1.size());
    ASSERT_EQ(4u, s2.size());

    swap(s1, s2);
    EXPECT_EQ(4u, s1.size());
    EXPECT_EQ(3u, s2.size());

    for (int k : {1, 2}) {
        EXPECT_TRUE(s1.find(k) == s1.end()) << "key=" << k;
        EXPECT_TRUE(s2.find(k) != s2.end()) << "key=" << k;
    }
    for (int k : {3}) {
        EXPECT_TRUE(s1.find(k) != s1.end()) << "key=" << k;
        EXPECT_TRUE(s2.find(k) != s2.end()) << "key=" << k;
    }
    for (int k : {4, 5, 6}) {
        EXPECT_TRUE(s1.find(k) != s1.end()) << "key=" << k;
        EXPECT_TRUE(s2.find(k) == s2.end()) << "key=" << k;
    }
}

#endif  // SET_BASIC

////////////////////////////////////////////////////////////////////////////////

#ifdef SET_DEEP_COPY

TEST(deep_copy, empty) {
    using S = exam::set<int>;
    const S s;
    S s2 = s;
    EXPECT_TRUE(s2.empty());
    s2 = s;
    EXPECT_TRUE(s2.empty());
}

TEST(deep_copy, copy_ctor) {
    using S = exam::set<int>;
    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());

    S s2 = s;
    ASSERT_EQ(7u, s.size());
    ASSERT_EQ(7u, s2.size());

    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        auto e = s.find(k);
        EXPECT_TRUE(e != s.end()) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;

        auto e2 = s2.find(k);
        EXPECT_TRUE(e2 != s2.end()) << "key=" << k;
        EXPECT_EQ(k, *e2) << "key=" << k;
    }

    // контейнеры независимы
    s.insert(0);
    s2.insert(80);
    s2.insert(90);
    EXPECT_EQ(8u, s.size());
    EXPECT_EQ(9u, s2.size());

    EXPECT_TRUE(s.find(0) != s.end());
    EXPECT_TRUE(s.find(80) == s.end());
    EXPECT_TRUE(s.find(90) == s.end());

    EXPECT_TRUE(s2.find(0) == s2.end());
    EXPECT_TRUE(s2.find(80) != s2.end());
    EXPECT_TRUE(s2.find(90) != s2.end());

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_FALSE(s2.empty());
    EXPECT_EQ(9u, s2.size());
}

TEST(deep_copy, copy_assign) {
    using S = exam::set<int>;
    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());

    s = std::as_const(s);  // self-assign
    ASSERT_EQ(7u, s.size());

    S s2;
    for (int k : {1, 2, 3}) {
        s2.insert(k);
    }
    ASSERT_EQ(3u, s2.size());

    s2 = s;

    ASSERT_EQ(7u, s.size());
    ASSERT_EQ(7u, s2.size());

    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        auto e = s.find(k);
        EXPECT_TRUE(e != s.end()) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;

        auto e2 = s2.find(k);
        EXPECT_TRUE(e2 != s2.end()) << "key=" << k;
        EXPECT_EQ(k, *e2) << "key=" << k;
    }
    for (int k : {1, 2, 3}) {
        EXPECT_TRUE(s.find(k) == s.end()) << "key=" << k;
        EXPECT_TRUE(s2.find(k) == s2.end()) << "key=" << k;
    }

    // контейнеры независимы
    s.insert(0);
    s2.insert(80);
    s2.insert(90);

    EXPECT_TRUE(s.find(0) != s.end());
    EXPECT_TRUE(s.find(80) == s.end());
    EXPECT_TRUE(s.find(90) == s.end());

    EXPECT_TRUE(s2.find(0) == s2.end());
    EXPECT_TRUE(s2.find(80) != s2.end());
    EXPECT_TRUE(s2.find(90) != s2.end());

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_FALSE(s2.empty());
    EXPECT_EQ(9u, s2.size());
}
#endif  // SET_DEEP_COPY

////////////////////////////////////////////////////////////////////////////////

#ifdef SET_MOVES
TEST(moves, empty) {
    using S = exam::set<int>;
    S s;
    S s2 = std::move(s);
    EXPECT_TRUE(s2.empty());
    s2 = std::move(s);
    EXPECT_TRUE(s2.empty());
}

TEST(moves, move_ctor) {
    using S = exam::set<int>;
    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());

    S s2 = std::move(s);
    ASSERT_EQ(0u, s.size());
    ASSERT_EQ(7u, s2.size());

    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        EXPECT_TRUE(s.find(k) == s.end()) << "key=" << k;

        auto e2 = s2.find(k);
        EXPECT_TRUE(e2 != s2.end()) << "key=" << k;
        EXPECT_EQ(k, *e2) << "key=" << k;
    }

    // контейнеры независимы
    s.insert(0);
    s2.insert(80);
    s2.insert(90);
    EXPECT_EQ(1u, s.size());
    EXPECT_EQ(9u, s2.size());

    EXPECT_TRUE(s.find(0) != s.end());
    EXPECT_TRUE(s.find(80) == s.end());
    EXPECT_TRUE(s.find(90) == s.end());

    EXPECT_TRUE(s2.find(0) == s2.end());
    EXPECT_TRUE(s2.find(80) != s2.end());
    EXPECT_TRUE(s2.find(90) != s2.end());

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_FALSE(s2.empty());
    EXPECT_EQ(9u, s2.size());
}

TEST(moves, move_assign) {
    using S = exam::set<int>;
    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());

    S s2;
    for (int k : {1, 2, 3}) {
        s2.insert(k);
    }
    ASSERT_EQ(3u, s2.size());

    s2 = std::move(s);

    ASSERT_EQ(0u, s.size());
    ASSERT_EQ(7u, s2.size());

    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        EXPECT_TRUE(s.find(k) == s.end()) << "key=" << k;

        auto e2 = s2.find(k);
        EXPECT_TRUE(e2 != s2.end()) << "key=" << k;
        EXPECT_EQ(k, *e2) << "key=" << k;
    }
    for (int k : {1, 2, 3}) {
        EXPECT_TRUE(s.find(k) == s.end()) << "key=" << k;
        EXPECT_TRUE(s2.find(k) == s2.end()) << "key=" << k;
    }

    // контейнеры независимы
    s.insert(0);
    s2.insert(80);
    s2.insert(90);

    EXPECT_TRUE(s.find(0) != s.end());
    EXPECT_TRUE(s.find(80) == s.end());
    EXPECT_TRUE(s.find(90) == s.end());

    EXPECT_TRUE(s2.find(0) == s2.end());
    EXPECT_TRUE(s2.find(80) != s2.end());
    EXPECT_TRUE(s2.find(90) != s2.end());

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_FALSE(s2.empty());
    EXPECT_EQ(9u, s2.size());
}
#endif  // SET_MOVES

////////////////////////////////////////////////////////////////////////////////

#ifdef SET_WITH_COMPARATOR
TEST(comparator, custom_key) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_custom_comparator)
    struct Key {
        int q;
    };
    struct Cmp {
        bool operator()(const Key& a, const Key& b) const { return a.q < b.q; }
    };

    using S = EXAM_SET<Key, Cmp>;  // exam::set<Key, Cmp>;
    S s;
    for (int k : {20, 10, 30}) {
        s.insert({k});
    }
    EXPECT_EQ(3u, s.size());
    for (int k : {20, 10, 30}) {
        auto e = s.find({k});
        ASSERT_TRUE(e != s.end());
        EXPECT_EQ(k, e->q);
    }
COMPILE_TIME_CONDITION_END()
}

TEST(comparator, default_constructor) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_custom_comparator)
    using S = EXAM_SET<int, std::greater<>>;  // exam::set<int, std::greater<>>;
    S s;
    for (int k : {30, 10, 20, 50, 40}) {
        s.insert(k);
    }
    EXPECT_EQ(5u, s.size());
    for (int k : {10, 20, 30, 40, 50}) {
        EXPECT_TRUE(s.find(k) != s.end()) << "key=" << k;
    }
COMPILE_TIME_CONDITION_END()
}

#ifdef SET_COMPARATOR_WITH_DEEP_COPY
TEST(comparator, default_constructor_and_copy) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_copy && compile_time::has_custom_comparator)
    using S = EXAM_SET<int, std::greater<>>;  // exam::set<int, std::greater<>>;
    S s;
    for (int k : {30, 10, 20, 50, 40}) {
        s.insert(k);
    }
    ASSERT_EQ(5u, s.size());
    for (int k : {10, 20, 30, 40, 50}) {
        ASSERT_TRUE(s.find(k) != s.end());
    }

    S s2 = s;
    s.clear();
    EXPECT_EQ(5u, s2.size());
    for (int k : {10, 20, 30, 40, 50}) {
        EXPECT_TRUE(s2.find(k) != s2.end()) << "key=" << k;
    }

    S s3;
    s3 = s2;
    s2.clear();
    EXPECT_EQ(5u, s3.size());
    for (int k : {10, 20, 30, 40, 50}) {
        EXPECT_TRUE(s3.find(k) != s3.end()) << "key=" << k;
    }
COMPILE_TIME_CONDITION_END()
}
#endif  // SET_COMPARATOR_WITH_DEEP_COPY - inside SET_WITH_COMPARATOR

#ifdef SET_COMPARATOR_WITH_MOVES
TEST(comparator, default_constructor_and_move) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_move && compile_time::has_custom_comparator)
    using S = EXAM_SET<int, std::greater<>>;  // exam::set<int, std::greater<>>;
    S s;
    for (int k : {30, 10, 20, 50, 40}) {
        s.insert(k);
    }
    ASSERT_EQ(5u, s.size());
    for (int k : {10, 20, 30, 40, 50}) {
        ASSERT_TRUE(s.find(k) != s.end());
    }

    S s2 = std::move(s);
    EXPECT_EQ(5u, s2.size());
    EXPECT_EQ(0u, s.size());
    for (int k : {10, 20, 30, 40, 50}) {
        EXPECT_TRUE(s2.find(k) != s2.end()) << "key=" << k;
    }

    S s3;
    s3 = std::move(s2);
    EXPECT_EQ(5u, s3.size());
    EXPECT_EQ(0u, s2.size());
    for (int k : {10, 20, 30, 40, 50}) {
        EXPECT_TRUE(s3.find(k) != s3.end()) << "key=" << k;
    }
COMPILE_TIME_CONDITION_END()
}
#endif  // SET_COMPARATOR_WITH_MOVES - inside SET_WITH_COMPARATOR

TEST(comparator, heavy_comparator) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_custom_comparator)
    struct heavy_comparator : std::less<> {
        char megabuf[100500];

        static int& ctors() { static int c = 0; return c; }
        static int& alive() { static int c = 0; return c; }

        heavy_comparator() { ++alive(); ++ctors(); }
        heavy_comparator(const heavy_comparator&) { ++alive(); ++ctors(); }
        ~heavy_comparator() { --alive(); }
    };

    {
        using S = EXAM_SET<int, heavy_comparator>;  // exam::set<int, heavy_comparator>;
        S s;
        EXPECT_EQ(1, heavy_comparator::alive());
        EXPECT_EQ(1, heavy_comparator::ctors());

        for (int k = 0; k != 100; ++k) {
            s.insert(k);
        }
        EXPECT_EQ(1, heavy_comparator::alive());
        EXPECT_EQ(1, heavy_comparator::ctors());

        auto e1 = s.find(1);
        auto e10 = s.find(10);
        auto e100 = s.find(100);
        EXPECT_TRUE(e1 != s.end());
        EXPECT_TRUE(e10 != s.end());
        EXPECT_TRUE(e100 == s.end());
        EXPECT_EQ(1, heavy_comparator::alive());
        EXPECT_EQ(1, heavy_comparator::ctors());
    }
    EXPECT_EQ(0, heavy_comparator::alive());
COMPILE_TIME_CONDITION_END()
}

struct less_by_modulo {
    int modulo;
    explicit less_by_modulo(int m) : modulo(m) {}
    bool operator()(int a, int b) const { return a % modulo < b % modulo; }
};

TEST(comparator, parametrized_constructor) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_custom_comparator)
    using S = EXAM_SET<int, less_by_modulo>;  // exam::set<int, less_by_modulo>;
    S s{less_by_modulo{10}};
    for (int k : {13, 11, 12, 15, 14, 21, 22, 23, 24, 25}) {
        s.insert(k);
    }
    EXPECT_EQ(5u, s.size());
    for (int k : {11, 12, 13, 14, 15}) {
        auto e = s.find(k);
        ASSERT_TRUE(e != s.end());
        EXPECT_EQ(k, *e);

        auto e2 = s.find(k + 20);
        EXPECT_TRUE(e == e2);
    }
COMPILE_TIME_CONDITION_END()
}

TEST(comparator, parametrized_constructor_and_swap) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_custom_comparator)
    using S = EXAM_SET<int, less_by_modulo>;  // exam::set<int, less_by_modulo>;
    S s{less_by_modulo{10}};
    for (int k : {13, 11, 12, 15, 14}) {
        s.insert(k);
    }
    ASSERT_EQ(5u, s.size());

    S s2{less_by_modulo{2}};
    s2.insert(0);
    EXPECT_EQ(1u, s2.size());

    swap(s, s2);
    EXPECT_EQ(1u, s.size());
    EXPECT_EQ(5u, s2.size());

    for (int k : {6, 7, 8, 9, 10}) {
        s.insert(k);
        s2.insert(k);
    }
    EXPECT_EQ(2u, s.size());  // 0 == 6 == 8 == 10, 7 == 9 (mod 2)
    EXPECT_EQ(10u, s2.size());
COMPILE_TIME_CONDITION_END()
}

#ifdef SET_COMPARATOR_WITH_DEEP_COPY
TEST(comparator, parametrized_constructor_and_copy) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_copy && compile_time::has_custom_comparator)
    using S = EXAM_SET<int, less_by_modulo>;  // exam::set<int, less_by_modulo>;
    S s{less_by_modulo{10}};
    for (int k : {13, 11, 12, 15, 14}) {
        s.insert(k);
    }
    ASSERT_EQ(5u, s.size());

    S s2 = s;
    s.clear();
    EXPECT_EQ(5u, s2.size());
    for (int k : {11, 12, 13, 14, 15}) {
        auto e = s2.find(k);
        ASSERT_TRUE(e != s2.end());
        EXPECT_EQ(k, *e);

        auto e2 = s2.find(k + 20);
        EXPECT_TRUE(e == e2);
    }

    S s3{less_by_modulo{2}};
    s3 = s2;
    EXPECT_EQ(5u, s3.size());
    for (int k : {11, 12, 13, 14, 15}) {
        auto e = s3.find(k);
        ASSERT_TRUE(e != s3.end());
        EXPECT_EQ(k, *e);

        auto e2 = s3.find(k + 20);
        EXPECT_TRUE(e == e2);
    }
COMPILE_TIME_CONDITION_END()
}
#endif  // SET_COMPARATOR_WITH_DEEP_COPY - inside SET_WITH_COMPARATOR

#ifdef SET_COMPARATOR_WITH_MOVES
TEST(comparator, parametrized_constructor_and_move) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_move && compile_time::has_custom_comparator)
    using S = EXAM_SET<int, less_by_modulo>;  // exam::set<int, less_by_modulo>;
    S s{less_by_modulo{10}};
    for (int k : {13, 11, 12, 15, 14}) {
        s.insert(k);
    }
    ASSERT_EQ(5u, s.size());

    S s2 = std::move(s);
    EXPECT_EQ(5u, s2.size());
    for (int k : {11, 12, 13, 14, 15}) {
        auto e = s2.find(k);
        ASSERT_TRUE(e != s2.end());
        EXPECT_EQ(k, *e);

        auto e2 = s2.find(k + 20);
        EXPECT_TRUE(e == e2);
    }

    S s3{less_by_modulo{2}};
    s3 = std::move(s2);
    EXPECT_EQ(5u, s3.size());
    for (int k : {11, 12, 13, 14, 15}) {
        auto e = s3.find(k);
        ASSERT_TRUE(e != s3.end());
        EXPECT_EQ(k, *e);

        auto e2 = s3.find(k + 20);
        EXPECT_TRUE(e == e2);
    }
COMPILE_TIME_CONDITION_END()
}
#endif  // SET_COMPARATOR_WITH_MOVES - inside SET_WITH_COMPARATOR
#endif  // SET_WITH_COMPARATOR

////////////////////////////////////////////////////////////////////////////////

#ifdef SET_ITERATION
TEST(iteration, empty) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_iterator)
    using S = EXAM_SET<int>;
    const S cs;
    ASSERT_TRUE(cs.empty());

    EXPECT_TRUE(cs.begin() == cs.end());
COMPILE_TIME_CONDITION_END()
}

TEST(iteration, ascending) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_iterator)
    using S = EXAM_SET<int>;
    using E = typename S::element;

    S s;
    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());

    E e = s.begin();
    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        ASSERT_TRUE(e != s.end()) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;

        // проверяем работу пред- и постинкремента
        E e0 = e, e1 = e;
        E& re1 = ++e1;
        EXPECT_EQ(&e1, &re1);
        E old = e++;
        EXPECT_TRUE(old == e0);
        EXPECT_TRUE(e == e1);
    }
    EXPECT_TRUE(e == s.end());
COMPILE_TIME_CONDITION_END()
}

TEST(iteration, descending) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_iterator)
    using S = EXAM_SET<int>;
    using E = typename S::element;

    S s;
    for (int k : {70, 60, 50, 40, 30, 20, 10}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());

    E e = s.begin();
    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        ASSERT_TRUE(e != s.end()) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;

        // проверяем работу пред- и постинкремента
        E e0 = e, e1 = e;
        E& re1 = ++e1;
        EXPECT_EQ(&e1, &re1);
        E old = e++;
        EXPECT_TRUE(old == e0);
        EXPECT_TRUE(e == e1);
    }
    EXPECT_TRUE(e == s.end());
COMPILE_TIME_CONDITION_END()
}

TEST(iteration, random) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_iterator)
    using S = EXAM_SET<int>;
    using E = typename S::element;

    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());

    E e = s.begin();
    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        ASSERT_TRUE(e != s.end()) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;

        // проверяем работу пред- и постинкремента
        E e0 = e, e1 = e;
        E& re1 = ++e1;
        EXPECT_EQ(&e1, &re1);
        E old = e++;
        EXPECT_TRUE(old == e0);
        EXPECT_TRUE(e == e1);
    }
    EXPECT_TRUE(e == s.end());
COMPILE_TIME_CONDITION_END()
}


TEST(iteration, from_random_point) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_iterator)
    using S = EXAM_SET<int>;
    using E = typename S::element;

    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());

    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        E e = s.find(k);
        E n = s.find(k + 10);
        ++e;
        EXPECT_TRUE(e == n) << "key=" << k;
    }
COMPILE_TIME_CONDITION_END()
}


#ifdef SET_ITERATION_WITH_DEEP_COPY
TEST(iteration, run_over_copy) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_copy && compile_time::has_iterator)
    using S = EXAM_SET<int>;
    using E = typename S::element;

    std::unique_ptr<S> ps = std::make_unique<S>();
    S& s0 = *ps;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s0.insert(k);
    }
    ASSERT_EQ(7u, s0.size());

    S s{s0};

    ps.reset();

    E e = s.begin();
    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        ASSERT_TRUE(e != s.end()) << "key=" << k;
        EXPECT_EQ(k, *e) << "key=" << k;

        // проверяем работу пред- и постинкремента
        E e0 = e, e1 = e;
        E& re1 = ++e1;
        EXPECT_EQ(&e1, &re1);
        E old = e++;
        EXPECT_TRUE(old == e0);
        EXPECT_TRUE(e == e1);
    }
    EXPECT_TRUE(e == s.end());
COMPILE_TIME_CONDITION_END()
}
#endif  // SET_ITERATION_WITH_DEEP_COPY - inside SET_ITERATION

#ifdef SET_ITERATION_WITH_COMPARATOR
TEST(iteration, custom_comparator) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_iterator && compile_time::has_custom_comparator)
    using S = EXAM_SET<int, std::greater<>>;  // exam::set<int, std::greater<>>
    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }

    auto e = s.begin();
    for (int k : {70, 60, 50, 40, 30, 20, 10}) {
        ASSERT_TRUE(e != s.end()) << "key=" << k;
        EXPECT_EQ(k, *e);
        ++e;
    }
    EXPECT_TRUE(e == s.end());
COMPILE_TIME_CONDITION_END()
}
#endif // SET_ITERATION_WITH_COMPARATOR - inside SET_ITERATION

#endif  // SET_ITERATION

////////////////////////////////////////////////////////////////////////////////

#ifdef SET_ERASE

TEST(erase, by_value_empty) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_erase_by_value)
    using S = EXAM_SET<int>;
    S s;
    EXPECT_FALSE(s.erase(0));
COMPILE_TIME_CONDITION_END()
}

TEST(erase, by_value_mini) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_erase_by_value)
    using S = EXAM_SET<int>;
    S s;
    for (int k : {-50, -10, -30}) {
        s.insert(k);
    }
    s.erase(-10);
COMPILE_TIME_CONDITION_END()
}

TEST(erase, by_value) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_erase_by_value)
    using S = EXAM_SET<int>;
    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        EXPECT_TRUE(s.find(k) != s.end());
        EXPECT_TRUE(s.erase(k));
        EXPECT_FALSE(s.erase(k));  // второй раз нечего удалять
        EXPECT_TRUE(s.find(k) == s.end());

        int m = k + 1;
        EXPECT_TRUE(s.find(m) == s.end());
        EXPECT_FALSE(s.erase(m));
    }
    EXPECT_TRUE(s.empty());
COMPILE_TIME_CONDITION_END()
}

#ifdef SET_ERASE_WITH_COMPARATOR
TEST(erase, by_value_with_custom_comparator) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_erase_by_value && compile_time::has_custom_comparator)
    using S = EXAM_SET<int, std::greater<>>;  // exam::set<int, std::greater<>>
    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        EXPECT_TRUE(s.find(k) != s.end());
        EXPECT_TRUE(s.erase(k));
        EXPECT_FALSE(s.erase(k));  // второй раз нечего удалять
        EXPECT_TRUE(s.find(k) == s.end());

        int m = k + 1;
        EXPECT_TRUE(s.find(m) == s.end());
        EXPECT_FALSE(s.erase(m));
    }
    EXPECT_TRUE(s.empty());
COMPILE_TIME_CONDITION_END()
}
#endif  // SET_ERASE_WITH_COMPARATOR - inside SET_ERASE

#ifdef SET_ERASE_WITH_ELEMENT
TEST(erase, by_element) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_erase_by_element)
    using S = EXAM_SET<int>;
    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());
    for (int k : {10, 20, 30, 40, 50, 60, 70}) {
        auto e = s.find(k);
        auto e1 = s.find(k + 10);  // тайное знание о том, какой элемент должен быть следующим

        auto e2 = s.erase(e);
        EXPECT_TRUE(e1 == e2);
    }
    EXPECT_TRUE(s.empty());
COMPILE_TIME_CONDITION_END()
}
#endif  // SET_ERASE_WITH_ELEMENT - inside SET_ERASE

#ifdef SET_ERASE_WITH_ITERATION
TEST(erase, by_iterator) {
COMPILE_TIME_CONDITION_BEGIN(compile_time::has_erase_by_element && compile_time::has_iterator)
    using S = EXAM_SET<int>;
    S s;
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        s.insert(k);
    }
    ASSERT_EQ(7u, s.size());
    for (int k : {40, 20, 60, 10, 30, 50, 70}) {
        auto e = s.find(k);
        auto e1 = e; ++e1;

        auto e2 = s.erase(e);
        EXPECT_TRUE(e1 == e2);
    }
    EXPECT_TRUE(s.empty());
COMPILE_TIME_CONDITION_END()
}
#endif  // SET_ERASE_WITH_ITERATION - inside SET_ERASE

#endif  // SET_ERASE

////////////////////////////////////////////////////////////////////////////////

TESTING_MAIN()
