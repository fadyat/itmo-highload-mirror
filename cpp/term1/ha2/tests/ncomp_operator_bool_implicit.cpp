#include <cassert>

#include "bignum.hpp"

int main()
{
    mp::bignum n;
    // Implicit operator bool() allows dangerous convertions.
    // This must not compile - use explicit conversion operator.
    int x = n + 1;
    (void)x;
}
