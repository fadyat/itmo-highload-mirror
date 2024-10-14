#include "fizzbuzz.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

void test_empty_fizzbuzz() {
	std::ostringstream buffer;
	std::cout.rdbuf(buffer.rdbuf());

	fizzbuzz(0);

	std::string expected = "";

	assert(buffer.str() == expected || buffer.str() == expected + "\n");
}

void test_short_fizzbuzz() {
	std::ostringstream buffer;
	std::cout.rdbuf(buffer.rdbuf());

	fizzbuzz(10);

	std::string expected = ""
		"1\n"
		"2\n"
		"Fizz\n"
		"4\n"
		"Buzz\n"
		"Fizz\n"
		"7\n"
		"8\n"
		"Fizz\n"
		"Buzz\n";

	assert(buffer.str() == expected || buffer.str() == expected + "\n");
}

void test_long_fizzbuzz() {
	std::ostringstream buffer;
	std::cout.rdbuf(buffer.rdbuf());

	fizzbuzz(16);

	std::string expected = ""
		"1\n"
		"2\n"
		"Fizz\n"
		"4\n"
		"Buzz\n"
		"Fizz\n"
		"7\n"
		"8\n"
		"Fizz\n"
		"Buzz\n"
		"11\n"
		"Fizz\n"
		"13\n"
		"14\n"
		"FizzBuzz\n"
		"16\n";

	assert(buffer.str() == expected || buffer.str() == expected + "\n");
}

void test_max_fizzbuzz() {
	std::ostringstream buffer;
	std::cout.rdbuf(buffer.rdbuf());

	fizzbuzz(std::numeric_limits<uint8_t>::max());
	assert(buffer.str().size() > 0);
}


void test_fizzbuzz() {
	std::streambuf* coutbuf = std::cout.rdbuf();

	test_empty_fizzbuzz();
	test_short_fizzbuzz();
	test_long_fizzbuzz();
	test_max_fizzbuzz();

	std::cout.rdbuf(coutbuf);
}

int main() {
	test_fizzbuzz();

    return 0;
}
