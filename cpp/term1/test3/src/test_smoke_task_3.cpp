#include "maybe.hpp"
#include "maybe.hpp"

#include <cassert>
#include <string>
#include <iostream>
#include <functional>

#include "test_utils.hpp"

#include "simple_test.h"

TEST(maybe_emplace, basic_check)
{
    struct no_copy
    {
        enum
        {
            one_arg,
            two_args,
            ref,
            str,

            total
        };

        no_copy()                   : type(total) {}
        no_copy(int)                : type(one_arg) {}
        no_copy(int, double const&) : type(two_args) {}
        no_copy(double&)            : type(ref) {}
        no_copy(std::string)        : type(str) {}

        no_copy(no_copy const&) = delete;
        no_copy& operator=(no_copy const&) = delete;
        no_copy(no_copy&&) = delete;
        no_copy& operator=(no_copy&&) = delete;

        int type = total;
    };

    control_03::maybe<no_copy> y;
    EXPECT_FALSE(y.has_value());

    y.emplace(42);
    EXPECT_TRUE(y.has_value());
    EXPECT_EQ(y.value().type, no_copy::one_arg);

    y.emplace(42, 42.);
    EXPECT_EQ(y.value().type, no_copy::two_args);

    double tmp;
    y.emplace(std::ref(tmp));
    EXPECT_TRUE(y.has_value());
    EXPECT_EQ(y.value().type, no_copy::ref);

    y.emplace();
    EXPECT_EQ(y.value().type, no_copy::total);
    EXPECT_TRUE(y.has_value());
}

namespace frw_ns {
    DEFINE_COUNTED_CLASS(EmplaceForwardTestType)

    struct EmplaceForwardMaybeType {
        static inline size_t LvalueCount = 0;
        static inline size_t RvalueCount = 0;

        EmplaceForwardMaybeType(int, const EmplaceForwardTestType &) { ++LvalueCount; }

        EmplaceForwardMaybeType(int, EmplaceForwardTestType &&) { ++RvalueCount; }
    };
}

TEST(maybe_emplace, check_emplace_perfect_forward)
{
    using namespace frw_ns;
    {
        control_03::maybe<EmplaceForwardMaybeType> mb;
        mb.emplace(42, EmplaceForwardTestType{});

        EmplaceForwardTestType local;
        mb.emplace(42, local);
        mb.emplace(42, std::move(local));
    }

    EXPECT_EQ(EmplaceForwardTestType::counters().default_ctor, 2);
    EXPECT_EQ(EmplaceForwardTestType::counters().copy_ctor, 0);
    EXPECT_EQ(EmplaceForwardTestType::counters().copy_assign, 0);
    EXPECT_EQ(EmplaceForwardTestType::counters().move_ctor, 0);
    EXPECT_EQ(EmplaceForwardTestType::counters().move_assign, 0);
    EXPECT_EQ(EmplaceForwardTestType::counters().dtor, 2);

    EXPECT_EQ(EmplaceForwardMaybeType::LvalueCount, 1);
    EXPECT_EQ(EmplaceForwardMaybeType::RvalueCount, 2);
}

namespace test_destroy
{
    struct test
    {
        static int count;

        test()  { ++count; }
        test(test const&) { ++count; }

        ~test() { --count; }

        static bool no_leaks()
        {
            return count == 0;
        }
    };

    int test::count = 0;
} // test_destroy

TEST(maybe_emplace, check_correct_destroy)
{
    using test_destroy::test;

    {
        control_03::maybe<test> mb0;
        mb0.emplace();
        control_03::maybe<test> mb1(test{});
        mb1 = mb0;
    }

    EXPECT_TRUE(test::no_leaks());
}

TEST(maybe_inplace_construct, empty_string) {
    control_03::maybe<std::string> mbs{control_03::in_place_t};
    EXPECT_TRUE(mbs.has_value());
    EXPECT_EQ(mbs.value().length(), 0UL);
}

TEST(maybe_inplace_construct, string) {
    control_03::maybe<std::string> mbs{control_03::in_place_t, 256, 'a'};
    EXPECT_TRUE(mbs.has_value());
    EXPECT_EQ(mbs.value().length(), 256UL);
    EXPECT_EQ(mbs.value()[3], 'a');
}

TEST(maybe_inplace_construct, mb_mb) {
    control_03::maybe<control_03::maybe<std::string>> mbs{control_03::in_place_t, control_03::in_place_t};
    EXPECT_TRUE(mbs.has_value());
    EXPECT_TRUE(mbs.value().has_value());
    EXPECT_EQ(mbs.value().value().length(), 0UL);
}

TEST(maybe_inplace_construct, inplace_obj) {
    (void)&control_03::in_place_t;
}


TESTING_MAIN()
