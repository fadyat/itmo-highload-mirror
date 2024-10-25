#include <string>
#include "bignum.hpp"

int main()
{
    mp::polynomial p("1*x^0");
    // There must be no implicit constructor from string to polynomial.
    p = std::string("10*x^0");

    (void)(p(0));
}
