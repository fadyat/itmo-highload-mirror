#include "get_size.hpp"
#include "get_size.hpp"

#include "../simple_test.h"

#include <cassert>
#include <string>


#define CODE_AND_TEXT(name, code) \
  code ; \
  constexpr char name[] = #code ";" ;


TEST(get_size, single_field) {
  // маленький тип с разным выравниванием
  CODE_AND_TEXT(def_char_1, struct single_char_1 { alignas(1) char x; })
  CODE_AND_TEXT(def_char_2, struct single_char_2 { alignas(2) char x; })
  CODE_AND_TEXT(def_char_4, struct single_char_4 { alignas(4) char x; })
  CODE_AND_TEXT(def_char_8, struct single_char_8 { alignas(8) char x; })
  CODE_AND_TEXT(def_char_16, struct single_char_16 { alignas(16) char x; })

  EXPECT_EQ(1U, cls_06::GetSize(def_char_1));
  EXPECT_EQ(2U, cls_06::GetSize(def_char_2));
  EXPECT_EQ(4U, cls_06::GetSize(def_char_4));
  EXPECT_EQ(8U, cls_06::GetSize(def_char_8));
  EXPECT_EQ(16U, cls_06::GetSize(def_char_16));

  // средний тип с разным выравниванием
  CODE_AND_TEXT(def_int_4, struct single_int_4 { alignas(4) int x; })
  CODE_AND_TEXT(def_int_8, struct single_int_8 { alignas(8) int x; })
  CODE_AND_TEXT(def_int_16, struct single_int_16 { alignas(16) int x; })

  EXPECT_EQ(4U, cls_06::GetSize(def_int_4));
  EXPECT_EQ(8U, cls_06::GetSize(def_int_8));
  EXPECT_EQ(16U, cls_06::GetSize(def_int_16));

  // большой тип с разным выравниванием
  CODE_AND_TEXT(def_double_8, struct single_double_8 { alignas(8) double x; })
  CODE_AND_TEXT(def_double_16, struct single_double_16 { alignas(16) double x; })

  EXPECT_EQ(8U, cls_06::GetSize(def_double_8));
  EXPECT_EQ(16U, cls_06::GetSize(def_double_16));
}

TEST(get_size, padding_between_fields) {
  CODE_AND_TEXT(def_xy,
    struct xy {
      alignas(2) char x;
      // padding
      alignas(16) char y;
    }
  )
  EXPECT_EQ(sizeof(xy), cls_06::GetSize(def_xy));
}

TEST(get_size, padding_after_fields) {
  CODE_AND_TEXT(def_xy,
    struct xy {
      alignas(16) char x;
      alignas(2) char y;
      // padding
    }
  )
  EXPECT_EQ(sizeof(xy), cls_06::GetSize(def_xy));
}

TEST(get_size, heavy_parsing) {
  struct mixture {
      short x;
      alignas(16) char y;
      // padding
      int z;
      alignas(16) int t;
      long u;
      short v;
      // padding
  };

  constexpr char def_mixture[] = R"(
    struct mixture {
        short x;  alignas(16)char  y ;
        int   z;alignas(16)  int   t ;
        long  u;             float v ;
    };
  )";

  EXPECT_EQ(sizeof(mixture), cls_06::GetSize(def_mixture));
}


TESTING_MAIN()
