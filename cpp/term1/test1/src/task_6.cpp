#include "network.hpp"
#include <cassert>
#include <cstring>
#include <sstream>
#include <vector>
#include <iostream>

#include "simple_test.h"


TEST(post, part_6) {
    Post* post1 = create_post("u1", "m1");
    Post* post2 = create_post("u2", "m2", post1);

    EXPECT_EQ(previous(post2), post1);
    EXPECT_FALSE(previous(post1));

    like_post(post1);
    Post* head = sort_by_likes(post2);
    EXPECT_EQ(head, post1);
    EXPECT_EQ(previous(post1), post2);
    EXPECT_FALSE(previous(post2));

    destroy_posts(head);
}

TEST(post, part_6_copy_check) {
    // don't forget to copy
    for (size_t posts_num = 3; posts_num < 50; posts_num++) {
        std::vector<Post*> posts(posts_num);
        posts[posts_num - 1] = create_post("u1", "m1");
        for (size_t i = 1; i < posts_num; i++) {
            std::ostringstream ss;
            ss << "u" << i;
            std::string user = ss.str();
            ss.str("");
            ss << "m" << i;
            std::string message = ss.str();
            posts[posts_num - i - 1] = create_post(
                user.c_str(), message.c_str(), posts[posts_num - i]);
        }

        for (size_t i = 0; i < posts_num; i++) {
            if (i < posts_num - 1) {
                EXPECT_EQ(previous(posts[i]), posts[i + 1]);
            } else {
                EXPECT_FALSE(previous(posts[i]));
            }
            for (size_t j = 0; j < i; j++) {
                like_post(posts[i]);
            }
        }

        Post* head = posts[0];
        head = sort_by_likes(head);

        {
            Post* cur = head;
            for (size_t i = 0; i < posts_num; i++) {
                EXPECT_EQ(cur, posts[posts_num - i - 1]);
                cur = previous(cur);
            }
        }

        head = sort_by_timestamp(head);
        {
            Post* cur = head;
            for (size_t i = 0; i < posts_num; i++) {
                EXPECT_EQ(cur, posts[i]);
                cur = previous(cur);
            }
        }

        destroy_posts(head);
    }
}


TEST(post, part_6_same_likes_order) {
    Post* post1 = create_post("u1", "m1");
    Post* post2 = create_post("u2", "m2", post1);
    like_post(post1);
    like_post(post2);
    Post* head = sort_by_likes(post2);

    EXPECT_EQ(head, post2);
    EXPECT_EQ(previous(post2), post1);
    EXPECT_FALSE(previous(post1));

    destroy_posts(head);
}


TEST(post, part_6_same_likes_order_2) {
    Post* post1 = create_post("u1", "m1");
    Post* post2 = create_post("u2", "m2", post1);
    Post* post3 = create_post("u2", "m2", post2);
    like_post(post1);
    like_post(post2);
    Post* head = sort_by_likes(post3);

    EXPECT_EQ(head, post2);
    EXPECT_EQ(previous(post2), post1);
    EXPECT_EQ(previous(post1), post3);
    EXPECT_FALSE(previous(post3));
    destroy_posts(head);
}

TESTING_MAIN()
