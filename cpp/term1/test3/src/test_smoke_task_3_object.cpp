#include "maybe.hpp"
#include "maybe.hpp"

#include "simple_test.h"

void use_in_place();

TEST(maybe_inplace_construct, inplace_obj) {
    (void)&control_03::in_place_t;
    use_in_place();
}


TESTING_MAIN()
