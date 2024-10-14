#include "network.hpp"
#include <cassert>
#include <cstring>
#include <iostream>

#include "simple_test.h"


TEST(post, part_4) {
    Post* post = create_post("u1", "m1");

    char new_text[] = "My awesome new text!";
    set_text(post, new_text);
    EXPECT_FALSE(std::strcmp(get_text(post), new_text));
    new_text[std::strlen(new_text) - 1] = '?';
    EXPECT_TRUE(std::strcmp(get_text(post), new_text));

    destroy_posts(post);
}


TESTING_MAIN()
