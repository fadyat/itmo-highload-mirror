#include "whatever.hpp"

int main()
{
    utils::whatever const w(1);
    (void)utils::whatever_cast<int&>(w);

    return 0;
}
