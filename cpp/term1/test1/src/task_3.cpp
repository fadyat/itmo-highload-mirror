#include "network.hpp"
#include <cassert>
#include <cstring>
#include <iostream>

#include "simple_test.h"


TEST(post, part_3) {
    Post* post1 = create_post("u1", "m1");
    Post* post2 = create_post("u1", "m2");
    Post* post3 = create_post("u1", "m2", post1);
    Post* post4 = create_post("u1", "m2", post2);

    EXPECT_EQ(timestamp(post1), 0ul);
    EXPECT_EQ(timestamp(post2), 1ul);
    EXPECT_EQ(timestamp(post3), 2ul);
    EXPECT_EQ(timestamp(post4), 3ul);

    Post* result = merge(post3, post4);

    EXPECT_EQ(result, post4);
    EXPECT_EQ(previous(result), post3);
    EXPECT_EQ(previous(previous(result)), post2);
    EXPECT_EQ(previous(previous(previous(result))), post1);
    EXPECT_FALSE(previous(previous(previous(previous(result)))));

    destroy_posts(result);
}


TEST(post, part_3_merge_and_concat) {
    Post* post1 = create_post("u1", "m1");
    Post* post2 = create_post("u1", "m2");
    Post* post3 = create_post("u1", "m3", post2);

    EXPECT_EQ(timestamp(post1), 4ul);
    EXPECT_EQ(timestamp(post2), 5ul);
    EXPECT_EQ(timestamp(post3), 6ul);

    Post* result = merge(post1, post3);

    EXPECT_EQ(result, post3);
    EXPECT_EQ(previous(result), post2);
    EXPECT_EQ(previous(previous(result)), post1);
    EXPECT_FALSE(previous(previous(previous(result))));

    destroy_posts(result);
}


TESTING_MAIN()
