#include "shared_buffer.hpp"
#include "shared_buffer.hpp"

#include <cstddef>
#include <cstring>
#include <type_traits>

#include "../simple_test.h"

TEST(basic, type_check) {
    EXPECT_TRUE((std::is_default_constructible_v<SharedBuffer>));
    EXPECT_TRUE((std::is_copy_constructible_v<SharedBuffer>));
    EXPECT_TRUE((std::is_constructible_v<SharedBuffer, size_t>));
    EXPECT_FALSE((std::is_convertible_v<size_t, SharedBuffer>)) << "implicit constructor";
    EXPECT_TRUE((std::is_copy_assignable_v<SharedBuffer>));
}

TEST(basic, construct_fill_and_copy) {
    SharedBuffer buffer { 10 };
    EXPECT_EQ(1U, buffer.useCount());
    SharedBuffer const ref = buffer;
    EXPECT_EQ(2U, buffer.useCount());
    EXPECT_EQ(2U, ref.useCount());

    EXPECT_EQ(10U, buffer.getSize());
    EXPECT_EQ(10U, ref.getSize());

    for (size_t i = 0; i < buffer.getSize(); ++i) {
        EXPECT_EQ(0, buffer.getData()[i]);
        EXPECT_EQ(0, ref.getData()[i]);

        buffer.getData()[i] = char(i);
        EXPECT_EQ(char(i), ref.getData()[i]);
    }

    {
        SharedBuffer copy = buffer;
        EXPECT_EQ(3U, buffer.useCount());
        EXPECT_EQ(3U, copy.useCount());
        EXPECT_EQ((void*)copy.getData(), (void*)buffer.getData());
    }

    EXPECT_EQ(2U, buffer.useCount());
}

TEST(basic, share_data) {
    SharedBuffer buf1 {100};
    SharedBuffer buf2 {buf1};
    SharedBuffer buf3 { 10 };

    SharedBuffer const& ref = buf1;

    EXPECT_EQ(buf2.getSize(), buf1.getSize()) << "Buffers should have the same size";
    EXPECT_EQ((void*)buf2.getData(), (void*)buf1.getData()) << "Buffers should point to the same memory";

    buf3 = ref;
    EXPECT_EQ((void*)buf3.getData(), (void*)buf1.getData()) << "After assignment, buf1 and buf3 should point to the same memory too";
}

TEST(basic, self_assign) {
    for (size_t i = 0; i < 100; ++i) {
        SharedBuffer b { i };
        SharedBuffer const& ref = b;
        b = ref;
    }
}

TESTING_MAIN()
