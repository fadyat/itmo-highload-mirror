#include <cassert>
#include <string>
#include "bignum.hpp"

int main()
{
    mp::bignum a(0);
    mp::bignum b(0);

    assert(!(a * b));
    assert((a * b).to_string() == "0");
}
