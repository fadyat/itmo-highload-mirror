#include <cassert>

#include "whatever.hpp"

int main()
{
    utils::whatever w1(42);
    utils::whatever w2;
    assert(*utils::whatever_cast<int>(&w1) == 42);
    assert(utils::whatever_cast<int>(&w2) == nullptr);
    w1 = w2;
    assert(utils::whatever_cast<int>(&w1) == nullptr);
    assert(utils::whatever_cast<int>(&w2) == nullptr);
}
