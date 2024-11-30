#include "fibonacci.hpp"
#include "fibonacci.hpp"

#include "../simple_test.h"

// убеждаемся, что аргумент - это константа времени компиляции
template<auto N> constexpr auto compile_time_constant = N;

// короткая запись для извлечения результата
template<auto N> constexpr uint64_t fib = compile_time_constant<cls08::FibonacciNumber<N>::value>;

TEST(fibonacci, values) {
    EXPECT_EQ(0U, fib<0>);
    EXPECT_EQ(1U, fib<1>);
    EXPECT_EQ(1U, fib<2>);
    EXPECT_EQ(2U, fib<3>);
    EXPECT_EQ(3U, fib<4>);
    EXPECT_EQ(5U, fib<5>);
    EXPECT_EQ(8U, fib<6>);

    EXPECT_EQ(6'765U, fib<20>);
    EXPECT_EQ(832'040U, fib<30>);
    EXPECT_EQ(102'334'155U, fib<40>);
    EXPECT_EQ(12'586'269'025U, fib<50>);  // 64-битный
}

TESTING_MAIN()
