#include "maybe.hpp"
#include "maybe.hpp"

#include <cassert>
#include <string>
#include <iostream>
#include <functional>

#include "simple_test.h"

using namespace std::string_literals;

TEST(maybe_ptr, basic_check) {
    std::string some = "hello";
    control_03::maybe<std::string*> mbs;
    EXPECT_EQ(sizeof(mbs), sizeof(std::string*));

    mbs = nullptr;
    EXPECT_FALSE(mbs.has_value());

    mbs.reset(&some);
    EXPECT_EQ(mbs.value(), &some);
    EXPECT_TRUE(mbs.has_value());
    EXPECT_TRUE(mbs.value());
    EXPECT_EQ(mbs.value()->length(), 5UL);

    mbs.reset();
    EXPECT_FALSE(mbs.has_value());

    // проверяем валидость значения после опустошения maybe
    EXPECT_EQ(some.length(), 5UL);
}

TEST(maybe_ptr, const_value) {
    std::string some = "hello";
    const control_03::maybe<std::string*> mbs{&some};
    EXPECT_TRUE(mbs.has_value());
    *mbs.value() += ", world"s;
    EXPECT_TRUE(mbs.has_value());

    EXPECT_EQ(mbs.value()->length(), 12UL);
}

TESTING_MAIN()
