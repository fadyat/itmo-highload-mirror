#include "int_vector.hpp"
#include "int_vector.hpp"

#include "../simple_test.h"


TEST(vector, simple) {
    IntVector v;

    pushBack(v, 42);
    pushBack(v, 24);
    pushBack(v, 66);

    EXPECT_EQ(42, v.data[0]);
    EXPECT_EQ(24, v.data[1]);
    EXPECT_EQ(66, v.data[2]);
    EXPECT_EQ(3ul, v.size);
    EXPECT_EQ(4ul, v.capacity_);

    popBack(v);
    EXPECT_EQ(42, v.data[0]);
    EXPECT_EQ(24, v.data[1]);
    EXPECT_EQ(2ul, v.size);
    EXPECT_EQ(4ul, v.capacity_);

    popBack(v);
    EXPECT_EQ(42, v.data[0]);
    EXPECT_EQ(1ul, v.size);
    EXPECT_EQ(4ul, v.capacity_);

    int *old_ptr = v.data;
    deallocate(v);
    EXPECT_TRUE(v.data == nullptr || v.data == old_ptr);
    EXPECT_EQ(0ul, v.size);
    EXPECT_EQ(0ul, v.capacity_);
}

TEST(vector, push_pop_push) {
    IntVector v;

    pushBack(v, 1000);
    EXPECT_EQ(1000, v.data[0]);
    EXPECT_EQ(1ul, v.size);
    EXPECT_EQ(1ul, v.capacity_);

    popBack(v);
    pushBack(v, 1001);

    EXPECT_EQ(1001, v.data[0]);
    EXPECT_EQ(1ul, v.size);
    EXPECT_EQ(1ul, v.capacity_);

    deallocate(v);
}

TESTING_MAIN()
