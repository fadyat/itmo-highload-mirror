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

   utils::whatever w1(a);
   assert(utils::whatever_cast<A>(&w1)->x == 1);

   utils::whatever w2;
   w2 = w1;
   assert(utils::whatever_cast<A>(&w1)->x == 1);
   assert(utils::whatever_cast<A>(&w2)->x == 2);
}

int main()
{
    check_copied_once();
}
