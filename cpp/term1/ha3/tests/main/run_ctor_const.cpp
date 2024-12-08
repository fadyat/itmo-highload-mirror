#include <cassert>

#include "whatever.hpp"

int main()
{
    utils::whatever const w1(42);
    utils::whatever w2(w1);
    assert(*utils::whatever_cast<int>(&w2) == 42);
    assert(*utils::whatever_cast<int>(&w1) == 42);
}
