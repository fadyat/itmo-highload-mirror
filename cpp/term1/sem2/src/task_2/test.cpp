#include "array_utils.hpp"
#include "array_utils.hpp"

#include "../simple_test.h"

TEST(sum, empty) {
    EXPECT_EQ(0, sum({}, 0)) << "Sum of empty array should be equal zero!";
}

TEST(sum, single_item) {
    int array[]{1, 2, 3, 4, 5};

    EXPECT_EQ(0, sum(array, 0)) << "Sum of empty array should be equal zero!";

    for (int i = 0; i != 5; ++i) {
        EXPECT_EQ(array[i], sum(array + i, 1));
    }
}

TEST(sum, many_items) {
    int const array[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    EXPECT_EQ(15, sum(array, 5));
    EXPECT_EQ(55, sum(array, 10));
}

///

TEST(countNumbers, empty) {
    EXPECT_EQ(0U, countNumbers({}, 0));
}

TEST(countNumbers, repeated_digits) {
    int const arr[] {1, 2, 3, 5, 9, 5, 3, 2, 1};
    size_t const N = sizeof(arr) / sizeof(int);

    EXPECT_EQ(0U, countNumbers(arr, 0));
    EXPECT_EQ(1U, countNumbers(arr, 1));  // 1
    EXPECT_EQ(3U, countNumbers(arr, 3));  // 1,2,3
    EXPECT_EQ(4U, countNumbers(arr, 4));  // 1,2,3,5
    EXPECT_EQ(5U, countNumbers(arr, 5));  // 1,2,3,5,9
    EXPECT_EQ(5U, countNumbers(arr, N));  // 1,2,3,5,9
}

TEST(countNumbers, very_long_array) {
    size_t const N = 100'000'000;
    int* arr = new int[N];
    for (size_t i = 0; i != N; ++i) arr[i] = (i % 10);
    EXPECT_EQ(10U, countNumbers(arr, N));
    delete[] arr;
}

static void runPopZerosTest(const_int_pointer_t *in, size_t s, const_int_pointer_t* expectedArray) {
    popZeros(in, s);

    size_t found_errors = 0;
    const size_t stop_after_errors = 20;  // чтобы не засорять выхлоп на сверхдлинных массивах

    for (size_t i = 0; i < s; ++i) {
        if (expectedArray[i] == nullptr) {
            found_errors += (in[i] != nullptr);
            EXPECT_EQ(nullptr, in[i])
                << "in[" << i << "] -> " << *in[i];
        } else {
            found_errors += (in[i] == nullptr);
            EXPECT_NE(nullptr, in[i])
                << "in[" << i << "]";
            if (!in[i]) continue;  // при нарушении условия - нижележащие проверки невозможны

            if (*(expectedArray[i]) == 0) {
                // равенство нулей, мб другой порядок
                found_errors += (*in[i] != 0);
                EXPECT_EQ(0, *(in[i]))
                    << "in[" << i << "]";
            } else {
                // равенство адресов
                found_errors += (in[i] != expectedArray[i]);
                EXPECT_EQ(expectedArray[i], in[i])
                    << "in[" << i << "] -> " << *in[i] << " != " << *expectedArray[i];
            }
        }

        ASSERT_LT(found_errors, stop_after_errors);
    }
}

TEST(popZeros, test1) {
    int i = 2, z = 0, k = -2;

    const size_t N = 4;
    const_int_pointer_t a[N] = {&i, nullptr, &z, &k};
    const_int_pointer_t expected[N] = {&i, nullptr, &k, &z};
    runPopZerosTest(a, N, expected);
}

TEST(popZeros, test2) {
    int z = 0;

    const_int_pointer_t sameZerosPair[] = {&z, &z};
    const_int_pointer_t expected2[] = {&z, &z};
    runPopZerosTest(sameZerosPair, 2, expected2);
}

TEST(popZeros, test3) {
    int z = 0, zz = 0;

    const_int_pointer_t differentZerosPair[] = {&z, &zz};
    const_int_pointer_t expected3[] = {&z, &zz};
    runPopZerosTest(differentZerosPair, 2, expected3);
}

TEST(popZeros, test4) {
    int z = 0, k = -2, zz = 0;

    const_int_pointer_t differentZerosAndNum[] = {&z, &zz, &k};
    const_int_pointer_t expected4[] = {&k, &z, &zz};
    runPopZerosTest(differentZerosAndNum, 3, expected4);
}

TEST(popZeros, test5) {
    int i = 2, z = 0, k = -2, zz = 0;

    const size_t N = 6;
    const_int_pointer_t anotherLongArray[N] = {&zz, &i, nullptr, &k, &z, nullptr};
    const_int_pointer_t expected5[N] = {&i, nullptr, &k, nullptr, &zz, &z};
    runPopZerosTest(anotherLongArray, N, expected5);
}

TEST(popZeros, very_long_array_nonzero) {
    const size_t N = 10'000'000;
    const_int_pointer_t* arr = new const_int_pointer_t[N];
    const_int_pointer_t* expected = new const_int_pointer_t[N];

    int a = 1, b = 2;
    for (size_t i = 0; i != N; ++i) { arr[i] = expected[i] = (i % 2) ? &a : &b; }
    runPopZerosTest(arr, N, expected);

    delete[] arr;
    delete[] expected;
}

TEST(popZeros, very_long_array_zero) {
    const size_t N = 10'000'000;
    const_int_pointer_t* arr = new const_int_pointer_t[N];
    const_int_pointer_t* expected = new const_int_pointer_t[N];

    int a = 0, b = 0;
    for (size_t i = 0; i != N; ++i) { arr[i] = expected[i] = (i % 2) ? &a : &b; }
    runPopZerosTest(arr, N, expected);

    delete[] arr;
    delete[] expected;
}

TEST(popZeros, very_long_array_interleaving) {
    const size_t S = 5;
    const size_t R = 1'000'000;
    const size_t N = R * S * 2;
    const_int_pointer_t* arr = new const_int_pointer_t[N];
    const_int_pointer_t* expected = new const_int_pointer_t[N];

    int a = 1, b = 0;
    for (size_t i = 0; i != R; ++i) {
        for (size_t j = 0; j != S; ++j) {
            arr[i*S*2 + j*2 + 0] = &a;
            arr[i*S*2 + j*2 + 1] = &b;

            expected[i*S + j + 0] = &a;
            expected[i*S + j + R*S] = &b;
        }
    }
    runPopZerosTest(arr, N, expected);

    delete[] arr;
    delete[] expected;
}

TESTING_MAIN()
