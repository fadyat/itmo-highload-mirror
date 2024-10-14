#include "compute_fib.hpp"

#include <cassert>
#include <fstream>
#include <exception>
#include <sstream>

void check_compute_fib(uint16_t n, int64_t expected) {
	{
		std::ofstream file("input.txt", std::ios::trunc); // truncates existing file

		file << n << std::endl;
	}

	int64_t actual = compute_fib();

	if (expected != actual) {
		std::stringstream msg;
		msg << "Expected " << expected << ", got " << actual << " instead!";
		throw std::runtime_error(msg.str());
	}
}

void test_compute_fib() {
	check_compute_fib(0, 0);
	check_compute_fib(1, 1);
	check_compute_fib(2, 1);
	check_compute_fib(3, 2);

	check_compute_fib(7, 13);

	check_compute_fib(10, 55);
}

int main() {
    test_compute_fib();

    return 0;
}
