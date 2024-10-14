#include "network.hpp"
#include <cassert>
#include <cstring>
#include <iostream>

#include "simple_test.h"


TEST(post, part_2_simple) {
    Post* post = create_post("u1", "m1");
    Post* result = remove_by_content(post, "1");

    EXPECT_FALSE(result);

    destroy_posts(result);
}


TEST(post, part_2_harder) {
    Post* post1 = create_post("u1", "my cow");
    Post* post2 = create_post("u2", "cow is so", post1);
    Post* post3 = create_post("u2", "is so my", post2);
    Post* result = remove_by_content(post3, "my");

    EXPECT_EQ(result, post2);
    EXPECT_FALSE(previous(result));

    destroy_posts(result);
}

TEST(post, part_2_regression) {
    Post* post1 = create_post("u1", "my cow");
    Post* post2 = create_post("u2", "cow is so", post1);
    Post* post3 = create_post("u2", "is so my", post2);
    Post* post4 = create_post("u2", "is so so", post3);
    Post* result = remove_by_content(post4, "my");

    EXPECT_EQ(result, post4);
    EXPECT_EQ(previous(result), post2);
    EXPECT_FALSE(previous(post2));

    destroy_posts(result);
}

// Requirement: Если в ленте не осталось постов, то возвращается nullptr
TEST(post, part_2_remove_all_feed) {
    Post* post1 = create_post("u1", "my so cow");
    Post* post2 = create_post("u2", "cow is so", post1);
    Post* post3 = create_post("u2", "is so my", post2);
    Post* post4 = create_post("u2", "is so so", post3);
    Post* result = remove_by_content(post4, "so");
    EXPECT_FALSE(result);
}


TESTING_MAIN()
