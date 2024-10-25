#include <cassert>
#include "bignum.hpp"

int main()
{
    mp::bignum a(4294967295);
    assert(a);
    a += 1;
    assert(a);
}
