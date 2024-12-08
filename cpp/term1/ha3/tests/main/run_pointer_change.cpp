#include <cassert>

#include "whatever.hpp"

void check_pointer_change()
{
    utils::whatever w3(42);
    int* b = utils::whatever_cast<int>(&w3);
    const utils::whatever& w4 = w3;

    const int* bref = utils::whatever_cast<int>(&w4);
    *b = 43;
    assert(*bref == *b);
}

int main()
{
    check_pointer_change();
}
