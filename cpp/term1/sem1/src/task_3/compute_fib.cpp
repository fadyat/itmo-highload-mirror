#include <fstream>


int64_t compute_fib() {
	std::ifstream in("input.txt");

	int64_t fnumber;
	in >> fnumber;
	
	if (fnumber < 2) {
	    return fnumber;
	}
	
	int64_t a, b, c;
	a = 0;
	c = b = 1;
	
	for (auto i = 1; i < fnumber; i++) {
		c = a + b;
		a = b;
		b = c;
	}
	
	return c;
}

