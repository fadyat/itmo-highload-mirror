// эти файлы включены дважды намеренно, для проверки на include guard / pragma once
#include "my_ostream_file.hpp"
#include "my_ostream_file.hpp"
#include "my_ostream.hpp"
#include "my_ostream.hpp"

#include "../simple_test.h"
#include "test_utils.h"

#include <type_traits>

TEST(file, type_check) {
  EXPECT_TRUE(std::is_abstract_v<cls_06::my_ostream>);

  EXPECT_TRUE((std::is_base_of_v<cls_06::my_ostream, cls_06::my_ostream_file>));

  EXPECT_FALSE(std::is_copy_constructible_v<cls_06::my_ostream_file>);
  EXPECT_FALSE(std::is_copy_assignable_v<cls_06::my_ostream_file>);

  EXPECT_FALSE((std::is_convertible_v<const char*, cls_06::my_ostream_file>))
      << "explicit constructor, please";
  EXPECT_FALSE((std::is_convertible_v<std::string, cls_06::my_ostream_file>))
      << "explicit constructor, please";

  EXPECT_TRUE((std::is_constructible_v<cls_06::my_ostream_file, const char*>));
}

const char *const file_name = "tmp.txt";

TEST(file, string) {
  FileSentry file(file_name);
  {
    cls_06::my_ostream_file file_stream{file_name};
    cls_06::my_ostream &ref = file_stream;
    ref << "Hello there!";
  }
  EXPECT_EQ("Hello there!", file.str());
}

TEST(file, int) {
  FileSentry file(file_name);
  {
    cls_06::my_ostream_file file_stream{file_name};
    cls_06::my_ostream &ref = file_stream;
    ref << -1234;
  }
  EXPECT_EQ("-1234", file.str());
}

TEST(file, double) {
  FileSentry file(file_name);
  {
    cls_06::my_ostream_file file_stream{file_name};
    cls_06::my_ostream &ref = file_stream;
    ref << 1234.5;
  }
  EXPECT_EQ("1234.5", file.str());
}

TESTING_MAIN()
