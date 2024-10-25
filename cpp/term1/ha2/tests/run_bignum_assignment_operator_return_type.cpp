#include <cassert>
#include <string>
#include "bignum.hpp"

void add_one(mp::bignum& n)
{
    n += 1;
}

int main()
{
    mp::bignum a(0);
    mp::bignum b(3);

    add_one(a=b);
    assert(std::uint32_t(a) == 4);

    add_one(a+=b);
    assert(std::uint32_t(a) == 8);

    add_one(a*=2);
    assert(std::uint32_t(a) == 17);

    return 0;
}
