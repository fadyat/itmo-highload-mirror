#include <cassert>

#include "whatever.hpp"

void check_construct_from_empty()
{
    utils::whatever w1;
    assert(w1.empty());
    utils::whatever w2(w1);
    assert(w1.empty());
    assert(w2.empty());
}

int main()
{
    check_construct_from_empty();
}
