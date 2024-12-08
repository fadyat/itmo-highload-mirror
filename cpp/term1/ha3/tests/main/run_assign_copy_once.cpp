#include <cassert>

#include "whatever.hpp"

struct A
{
    A() = default;
    A(A const & a) : x(a.x + 1) {}
    A(A && a) = delete;

    int x = 0;
};

void check_copied_once()
{
   A a;
   assert(a.x == 0);

   // Value should be passed by reference until final class that will store a copy of it.
   utils::whatever w;
   w = a;
   assert(utils::whatever_cast<A>(&w)->x == 1);
}

int main()
{
    check_copied_once();
}
