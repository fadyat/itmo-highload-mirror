#include <cassert>

#include "whatever.hpp"

int main()
{
    utils::whatever const w;
    assert(w.empty());
}
