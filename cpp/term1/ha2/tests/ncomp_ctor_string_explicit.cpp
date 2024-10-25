#include <string>
#include "bignum.hpp"

int main()
{
    mp::bignum n;
    std::string str("100");
    // There must be no implicit constructor from string to bignum.
    (void)(n + str);
    return 0;
}
