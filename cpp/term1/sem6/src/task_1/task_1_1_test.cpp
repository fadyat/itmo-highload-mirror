// эти файлы включены дважды намеренно, для проверки на include guard / pragma once
#include "my_ostream_con.hpp"
#include "my_ostream_con.hpp"
#include "my_ostream.hpp"
#include "my_ostream.hpp"

#include "../simple_test.h"
#include "test_utils.h"

#include <type_traits>


TEST(console, type_check) {
  EXPECT_TRUE(std::is_abstract_v<cls_06::my_ostream>);

  EXPECT_TRUE((std::is_base_of_v<cls_06::my_ostream, cls_06::my_ostream_con>));
  EXPECT_TRUE(std::is_default_constructible_v<cls_06::my_ostream_con>);
}

TEST(console, string) {
  CoutSentry sout;

  cls_06::my_ostream_con stream;
  cls_06::my_ostream& ref = stream;
  ref << "Hello" << std::string(", ") << "world";

  EXPECT_EQ("Hello, world", sout.str());
}

TEST(console, int) {
  CoutSentry sout;

  cls_06::my_ostream_con stream;
  cls_06::my_ostream& ref = stream;
  ref << 1234 << -5678;

  EXPECT_EQ("1234-5678", sout.str());
}

TEST(console, double) {
  CoutSentry sout;

  cls_06::my_ostream_con stream;
  cls_06::my_ostream& ref = stream;
  ref << 10. << -20.5;

  EXPECT_EQ("10-20.5", sout.str());
}

TESTING_MAIN()
