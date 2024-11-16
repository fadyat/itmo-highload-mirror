// эти файлы включены дважды намеренно, для проверки на include guard / pragma once
#include "my_ostream_combo.hpp"
#include "my_ostream_combo.hpp"
#include "my_ostream_file.hpp"
#include "my_ostream_file.hpp"
#include "my_ostream_con.hpp"
#include "my_ostream_con.hpp"
#include "my_ostream.hpp"
#include "my_ostream.hpp"

#include "../simple_test.h"
#include "test_utils.h"

#include <type_traits>
#include <unordered_set>

TEST(combo, type_check) {
  EXPECT_TRUE(std::is_abstract_v<cls_06::my_ostream>);

  EXPECT_TRUE((std::is_base_of_v<cls_06::my_ostream, cls_06::my_ostream_combo>));

  EXPECT_TRUE(std::has_virtual_destructor_v<cls_06::my_ostream>);

  EXPECT_TRUE((std::is_constructible_v<
      cls_06::my_ostream_combo,
      cls_06::my_ostream_combo*, cls_06::my_ostream_combo*>));
}

const char *const file_name = "tmp.txt";
const char *const file_name_2 = "tmp2.txt";

TEST(combo, string) {
  CoutSentry sout;
  FileSentry fout(file_name);
  {
    cls_06::my_ostream_combo stream{
        new cls_06::my_ostream_con{},
        new cls_06::my_ostream_file{file_name}
    };
    cls_06::my_ostream& ref = stream;

    ref << "Hello";
  }
  EXPECT_EQ("Hello", sout.str());
  EXPECT_EQ("Hello", fout.str());
}

TEST(combo, int) {
  CoutSentry sout;
  FileSentry fout(file_name);
  {
    cls_06::my_ostream_combo stream{
        new cls_06::my_ostream_con{},
        new cls_06::my_ostream_file{file_name}
    };
    cls_06::my_ostream& ref = stream;

    ref << -1234;
  }
  EXPECT_EQ("-1234", sout.str());
  EXPECT_EQ("-1234", fout.str());
}

TEST(combo, double) {
  CoutSentry sout;
  FileSentry fout(file_name);
  {
    cls_06::my_ostream_combo stream{
        new cls_06::my_ostream_con{},
        new cls_06::my_ostream_file{file_name}
    };
    cls_06::my_ostream& ref = stream;

    ref << -1234.5;
  }
  EXPECT_EQ("-1234.5", sout.str());
  EXPECT_EQ("-1234.5", fout.str());
}

TEST(combo, complex_test) {
  CoutSentry sout;
  FileSentry fout1(file_name);
  FileSentry fout2(file_name_2);
  {
    cls_06::my_ostream_combo stream{
        new cls_06::my_ostream_combo{
          new cls_06::my_ostream_con{},
          new cls_06::my_ostream_file{file_name}
        },
        new cls_06::my_ostream_combo{
          new cls_06::my_ostream_file{file_name_2},
          new cls_06::my_ostream_con{}
        },
    };
    cls_06::my_ostream& ref = stream;

    ref << "Hello" << -123 << -4.5;
  }
  EXPECT_EQ("HelloHello-123-123-4.5-4.5", sout.str());
  EXPECT_EQ("Hello-123-4.5", fout1.str());
  EXPECT_EQ("Hello-123-4.5", fout2.str());
}

template<class ComboStream> void test_copy_constructible() {
  // если класс некопируемый, то и не надо,
  // но если он копируемый, то должен работать корректно!
  if constexpr (std::is_copy_constructible_v<ComboStream>) {
    CoutSentry sout;
    FileSentry fout(file_name);
    {
      ComboStream stream{
          new cls_06::my_ostream_con{},
          new cls_06::my_ostream_file{file_name}
      };
      ComboStream copy = stream;

      stream << "Hello";
      copy << "World";
    }
    EXPECT_EQ("HelloWorld", sout.str());
    EXPECT_EQ("HelloWorld", fout.str());
  }
}

TEST(combo, copy_constructible) {
  test_copy_constructible<cls_06::my_ostream_combo>();
}

template<class ComboStream> void test_copy_assignable() {
  // если класс некопируемый, то и не надо,
  // но если он копируемый, то должен работать корректно!
  if constexpr (std::is_copy_assignable_v<ComboStream>) {
    CoutSentry sout;
    FileSentry fout1(file_name);
    FileSentry fout2(file_name_2);
    {
      ComboStream stream{
          new cls_06::my_ostream_con{},
          new cls_06::my_ostream_file{file_name}
      };
      ComboStream stream2{
          new cls_06::my_ostream_con{},
          new cls_06::my_ostream_file{file_name_2}
      };
      stream2 = stream;

      stream << "Hello";
      stream2 << "World";
    }
    EXPECT_EQ("HelloWorld", sout.str());
    EXPECT_EQ("HelloWorld", fout1.str());
    EXPECT_EQ("", fout2.str());
  }
}

TEST(combo, copy_assignable) {
  test_copy_assignable<cls_06::my_ostream_combo>();
}

TESTING_MAIN()
