#include <cassert>
#include <string>
#include "bignum.hpp"

void check_constructor()
{
    std::string str{"1" + std::string(150, '0')};
    mp::bignum a(str);
    assert(a.to_string() == str);
}

void check_long_operators()
{
    mp::bignum a("1" + std::string(50, '0'));
    mp::bignum b("1" + std::string(77, '0'));

    std::string const aplusb("1" + std::string(26, '0') + "1" + std::string(50, '0'));
    std::string const amultb("1" + std::string(127, '0'));

    assert((a + b).to_string() == aplusb);
    assert((a * b).to_string() == amultb);
    assert((b + a).to_string() == aplusb);
    assert((b * a).to_string() == amultb);
}

int main()
{
    check_constructor();
    check_long_operators();
}
