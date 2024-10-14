#include "find_nearest_same.hpp"
#include "find_nearest_same.hpp"

#include "../simple_test.h"

TEST(findNearestSame, basic_tests) {
    const char s[] = "abdaaacb";

    EXPECT_EQ(3, findNearestSame(s, s));
    EXPECT_EQ(6, findNearestSame(s, s + 1));
    EXPECT_EQ(0, findNearestSame(s, s + 2));
    EXPECT_EQ(1, findNearestSame(s, s + 3));
    EXPECT_EQ(1, findNearestSame(s, s + 4));
    EXPECT_EQ(-1, findNearestSame(s, s + 5));
    EXPECT_EQ(0, findNearestSame(s, s + 6));
    EXPECT_EQ(-6, findNearestSame(s, s + 7));
}

TEST(findNearestSame, very_long_string) {
    const ptrdiff_t N = 100'000'000;
    char* megabuffer = new char[N + 1];
    for (ptrdiff_t i = 0; i < N; ++i) megabuffer[i] = 'a';

    const ptrdiff_t M = N / 2;
    megabuffer[M] = 'b';

    // stress test: call findNearestSame multiple times.
    for (const ptrdiff_t L : {5, 10, 20, 30, 40, 50}) {
        megabuffer[M - L] = 'b';  // ...b...b...
        EXPECT_EQ(-L, findNearestSame(megabuffer, megabuffer + M));
        EXPECT_EQ(L, findNearestSame(megabuffer, megabuffer + M - L));
        megabuffer[M + L] = 'b';  // ...b...b...b...
        EXPECT_EQ(L, findNearestSame(megabuffer, megabuffer + M));
        megabuffer[M - L + 1] = 'b';
        EXPECT_EQ(-L+1, findNearestSame(megabuffer, megabuffer + M));

        megabuffer[M - L] = megabuffer[M + L] = megabuffer[M - L + 1] = 'a';
    }

    delete[] megabuffer;
}

TESTING_MAIN()
