#include "network.hpp"
#include <cassert>
#include <cstring>
#include <iostream>

#include "simple_test.h"


TEST(post, part_1) {
    Post* post1 = create_post("u1", "m1");
    Post* post2 = create_post("u2", "m2", post1);

    EXPECT_EQ(previous(post2), post1);
    EXPECT_EQ(previous(post1), nullptr);

    EXPECT_EQ(std::strcmp(get_user(post1), "u1"), 0);
    EXPECT_EQ(std::strcmp(get_text(post2), "m2"), 0);

    destroy_posts(post2);
}

TEST(post, part_1_copy_check) {
    char buff[] = "Hello msg";
    Post* post = create_post("u1", buff);

    buff[0] = 'A';
    EXPECT_NE(std::strcmp(get_text(post), buff), 0); // Hello != Aello

    destroy_posts(post);
}


TESTING_MAIN()
