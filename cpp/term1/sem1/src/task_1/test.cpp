#include "hello.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

void test_hello_world() {
	std::ostringstream buffer;
	std::streambuf* coutbuf = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

	say_hello();

	assert(buffer.str() == "Hello, world!\n" || buffer.str() == "Hello, world!");

	std::cout.rdbuf(coutbuf);
}

int main() {
    test_hello_world();

    return 0;
}