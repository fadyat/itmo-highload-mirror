#include "composition.hpp"
#include "composition.hpp"

#include <cmath>
#include "../simple_test.h"

namespace {

double sqr(double x) {
  return x * x;
}

}  // anonymous namespace

TEST(compose, main) {
  FuncT cos_func = static_cast<FuncT>(cos);

  EXPECT_EQ(2.0, Compose(0, 2.0));
  EXPECT_EQ(4.0, Compose(1, sqr, 2.0));
  EXPECT_EQ(1.0, Compose(2, cos_func, sqr, 0.0));
  EXPECT_EQ(1.0, Compose(2, sqr, cos_func, 0.0));
}

TESTING_MAIN()
