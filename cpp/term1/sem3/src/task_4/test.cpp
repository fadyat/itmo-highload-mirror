#include "funcs.hpp"
#include "funcs.hpp"

#include "../simple_test.h"


TEST(inc, simple) {
    EXPECT_EQ(0, inc_first());
    EXPECT_EQ(1, inc_second());
    EXPECT_EQ(2, inc_first());
    EXPECT_EQ(3, inc_second());
    EXPECT_EQ(4, inc_second());
    EXPECT_EQ(5, inc_second());
    EXPECT_EQ(6, inc_first());
}

TESTING_MAIN()
