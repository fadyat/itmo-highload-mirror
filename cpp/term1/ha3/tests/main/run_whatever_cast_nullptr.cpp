#include <cassert>

#include "whatever.hpp"

int main()
{
    utils::whatever* w{ nullptr };
    assert(nullptr == utils::whatever_cast<int>(w));

    utils::whatever const* cw{ nullptr };
    assert(nullptr == utils::whatever_cast<const int>(cw));
    return 0;
}
