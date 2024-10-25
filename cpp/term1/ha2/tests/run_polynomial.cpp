#include <cassert>
#include <string>
#include <cmath>
#include <limits>

// Include guard should prevent double inclusion.
#include "bignum.hpp"

void check_polynomial()
{
    mp::polynomial const p("2*x^2+10*x^1+1*x^0");

    assert(p(0) == 1);
    assert(p(-1) == -7);
    assert(std::abs(p(1.0) - 13.) < std::numeric_limits<double>::epsilon());
    assert(std::abs(p(0.1) - 2.02) < std::numeric_limits<double>::epsilon());
    assert(p(mp::bignum(2)).to_string() == "29");
}

void check_long_coefficient()
{
    mp::polynomial p("100*x^0");
    assert(p.at(0) == 100);
}

void check_long_double()
{
    mp::polynomial p("1*x^1");
    assert(std::abs(p(1e100)-1e100) < 1e90);
}

void check_at()
{
    mp::polynomial p("1*x^0");

    assert(p.at(0) == 1);
    assert(p.at(1) == 0);
    assert(p(1) == 1);

    p.at(1) = 2;

    assert(p.at(0) == 1);
    assert(p.at(1) == 2);
    assert(p(1) == 3);
}

void check_const_at()
{
    mp::polynomial const p("2*x^2+10*x^1+1*x^0");

    assert(p.at(0) == 1);
    assert(p.at(1) == 10);
    assert(p.at(2) == 2);
}

int main()
{
    check_polynomial();
    check_long_coefficient();
    check_long_double();
    check_at();
    check_const_at();
}