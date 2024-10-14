#include "network.hpp"
#include <cassert>
#include <cstring>
#include <iostream>

#include "simple_test.h"


TEST(post, part_5) {
    {
      Post* post = create_post("u1", "This is so interesting post!");
      replace_with_stars(post, "interesting");
      EXPECT_FALSE(std::strcmp(get_text(post), "This is so *********** post!"));
      destroy_posts(post);
    }

    {
      Post* post = create_post("u1", "aaaaaaaaa");
      replace_with_stars(post, "aa");
      EXPECT_FALSE(std::strcmp(get_text(post), "********a"));
      destroy_posts(post);
    }
}

TEST(post, part_5_simpliest) {
  replace_with_stars(nullptr, "interesting");
}

TEST(post, part_5_pattern_with_star) {
    Post* post = create_post("u1", "abc*defdefdef");
    replace_with_stars(post, "*def");
    EXPECT_FALSE(std::strcmp(get_text(post), "abc****defdef"));
    destroy_posts(post);
}

TEST(post, part_5_check_tail) {
  Post* post = create_post("u1", "This is so interesting post!");
  Post* post2 = create_post("u1", "Skip this first post", post);
  replace_with_stars(post2, "interesting");

  EXPECT_EQ(previous(post2), post);
  EXPECT_FALSE(previous(post));

  EXPECT_FALSE(std::strcmp(get_text(post2), "Skip this first post"));
  EXPECT_FALSE(std::strcmp(get_text(post), "This is so *********** post!"));
  destroy_posts(post2);
}


TESTING_MAIN()