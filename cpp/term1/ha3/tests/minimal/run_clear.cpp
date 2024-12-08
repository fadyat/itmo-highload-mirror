#include <cassert>

#include "whatever.hpp"

void check_clear()
{
    utils::whatever w1;
    assert(w1.empty());
    w1.clear();
    assert(w1.empty());
    w1 = 42;
    assert(!w1.empty());
    w1.clear();
    assert(w1.empty());
}

int main()
{
    check_clear();
}
