#include "get_size.hpp"
#include "get_size.hpp"

#include "../simple_test.h"

#include <cassert>
#include <string>


#define CODE_AND_TEXT(name, code) \
  code ; \
  constexpr char name[] = #code ";" ;

TEST(get_size, empty) {
  CODE_AND_TEXT(def_empty, struct Empty {})
  static_assert(sizeof(Empty) == 1);

  EXPECT_EQ(1U, cls_06::GetSize(def_empty));
}

TEST(get_size, single_field) {
  CODE_AND_TEXT(def_char, struct single_char { char x; })
  CODE_AND_TEXT(def_short, struct single_short { short x; })
  CODE_AND_TEXT(def_int, struct single_int { int x; })
  CODE_AND_TEXT(def_long, struct single_long { long x; })
  CODE_AND_TEXT(def_float, struct single_float { float x; })
  CODE_AND_TEXT(def_double, struct single_double { double x; })

  static_assert(sizeof(single_char) == 1);
  EXPECT_EQ(1U, cls_06::GetSize(def_char));

  static_assert(sizeof(single_short) == 2);
  EXPECT_EQ(2U, cls_06::GetSize(def_short));

  static_assert(sizeof(single_int) == 4);
  EXPECT_EQ(4U, cls_06::GetSize(def_int));

  static_assert(sizeof(single_long) == 8);
  EXPECT_EQ(8U, cls_06::GetSize(def_long));

  static_assert(sizeof(single_float) == 4);
  EXPECT_EQ(4U, cls_06::GetSize(def_float));

  static_assert(sizeof(single_double) == 8);
  EXPECT_EQ(8U, cls_06::GetSize(def_double));
}

TEST(get_size, packed_structures) {
  CODE_AND_TEXT(def_chars,
    struct five_chars {
      char a;
      char b;
      char c;
      char d;
      char e;
    }
  )
  EXPECT_EQ(sizeof(five_chars), cls_06::GetSize(def_chars));

  CODE_AND_TEXT(def_shorts,
    struct five_shorts {
      short a;
      short b;
      short c;
      short d;
      short e;
    }
  )
  EXPECT_EQ(sizeof(five_shorts), cls_06::GetSize(def_shorts));
}

TEST(get_size, padding_between_fields) {
  CODE_AND_TEXT(def_char_int_double,
    struct char_int_double {
      char x;
      // padding
      int y;
      // padding
      double z;
    }
  )
  EXPECT_EQ(sizeof(char_int_double), cls_06::GetSize(def_char_int_double));

  CODE_AND_TEXT(def_short_char_double,
    struct short_char_double {
      short x;
      char y;
      // padding
      double z;
    }
  )
  EXPECT_EQ(sizeof(short_char_double), cls_06::GetSize(def_short_char_double));
}

TEST(get_size, padding_after_fields) {
  CODE_AND_TEXT(def_double_int_char,
    struct double_int_char {
      double x;
      int y;
      char z;
      // padding
    }
  )
  EXPECT_EQ(sizeof(double_int_char), cls_06::GetSize(def_double_int_char));

  CODE_AND_TEXT(def_int_int_double_short,
    struct int_int_double_short {
      int x;
      int y;
      double z;
      short t;
      // padding
    }
  )
  EXPECT_EQ(sizeof(int_int_double_short), cls_06::GetSize(def_int_int_double_short));
}


TEST(get_size, mixed_case) {
  CODE_AND_TEXT(def_mixture,
    struct mixture {
      short x;
      char y;
      // padding
      int z;
      int t;
      // padding
      long u;
      float v;
      // padding
    }
  )
  EXPECT_EQ(sizeof(mixture), cls_06::GetSize(def_mixture));
}

TEST(get_size, heavy_parsing) {
  struct mixture {
      short x;
      char y;
      // padding
      int z;
      int t; /* padding */
      long u;
      float v;
      // padding
  };

  constexpr char def_mixture[] = R"(
    struct mixture {
        short x;char  y ;
        int   z;int   t ;
        long  u;float v ;
    };
  )";

  EXPECT_EQ(sizeof(mixture), cls_06::GetSize(def_mixture));
}


TESTING_MAIN()
