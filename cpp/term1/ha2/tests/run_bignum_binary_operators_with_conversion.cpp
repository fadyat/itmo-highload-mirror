#include <cassert>
#include <string>
#include "bignum.hpp"

int main()
{
    mp::bignum const a(10);

    assert((a + 10).to_string() == "20");
    assert((10 + a).to_string() == "20");
    assert((a * 10).to_string() == "100");
    assert((10 * a).to_string() == "100");
}
