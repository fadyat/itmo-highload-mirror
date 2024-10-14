#include "strcmp.hpp"
#include "strcmp.hpp"

#include "../simple_test.h"

TEST(myStrCmp, ascii) {
    EXPECT_LT(myStrCmp("a", "b"), 0);
    EXPECT_LT(myStrCmp("", "a"), 0);
    EXPECT_GT(myStrCmp("a", ""), 0);
    EXPECT_LT(myStrCmp("aaa", "aac"), 0);
    EXPECT_GT(myStrCmp("aaac", "aaabb"), 0);
    EXPECT_GT(myStrCmp("aaaca", "aaabb"), 0);
    EXPECT_EQ(myStrCmp("aaa", "aaa"), 0);
    EXPECT_EQ(myStrCmp("bb", "bb"), 0);
    EXPECT_EQ(myStrCmp("", ""), 0);
}

TEST(myStrCmp, extended_charset) {
    EXPECT_EQ(myStrCmp("abc\xAA\xBB\xCC", "abc\xAA\xBB\xCC"), 0);

    // разные символы просто "не равны" (больше/меньше - за пределами ASCII не играет роли)
    EXPECT_NE(myStrCmp("abc\xAA\xBB\xCC\x11", "abc\xAA\xBB\xCC\xDD"), 0);
    EXPECT_NE(myStrCmp("abc\xAA\xBB\xCC\xDD", "abc\xAA\xBB\xCC\x11"), 0);

    // более длинная строка должна быть "больше" более короткой, неважно, что там за символ
    EXPECT_GT(myStrCmp("abc\xAA\xBB\xCC\x11", "abc\xAA\xBB\xCC"), 0);
    EXPECT_GT(myStrCmp("abc\xAA\xBB\xCC\xDD", "abc\xAA\xBB\xCC"), 0);

    EXPECT_LT(myStrCmp("abc\xAA\xBB\xCC", "abc\xAA\xBB\xCC\xDD"), 0);
    EXPECT_LT(myStrCmp("abc\xAA\xBB\xCC", "abc\xAA\xBB\xCC\x11"), 0);
}

TEST(myStrCmp, very_long_strings) {
    size_t N = 10'000'000;
    char* a = new char[N];
    char* b = new char[N];

    for (size_t i = 0; i != N-1; ++i) {
        a[i] = b[i] = (char)('a' + i%26);
    }
    a[N-1] = b[N-1] = 0;

    size_t X = 1000;
    a[X] = '0';
    b[X] = '1';

    size_t Y = N - 1000;
    a[Y] = '3';
    b[Y] = '2';

    // "abc...xyz0abc...3..." < "abc...xyz1abc...2..."
    EXPECT_LT(myStrCmp(a, b), 0);
    EXPECT_GT(myStrCmp(b, a), 0);

    // "abc...xyz3abc..." > "abc...xyz2abc..."
    EXPECT_GT(myStrCmp(a + X + 1, b + X + 1), 0);
    EXPECT_LT(myStrCmp(b + X + 1, a + X + 1), 0);

    delete[] a;
    delete[] b;
}

TESTING_MAIN()
