#include <cassert>

#include "whatever.hpp"

int main()
{
    utils::whatever w1(42);
    utils::whatever w2(1.);
    w2 = w1;
    assert(*utils::whatever_cast<int>(&w2) == 42);
    assert(*utils::whatever_cast<int>(&w1) == 42);
}
