#include <cassert>
#include <stdexcept>
#include "whatever.hpp"

struct Foo {
    Foo(bool copyAllowed)
        :  copyAllowed_{copyAllowed}
    {}

    Foo(const Foo& other)
        : copyAllowed_{other.copyAllowed_}
    {
        if (!copyAllowed_)
        {
            throw std::runtime_error("copy fail");
        }
    }

    bool copyAllowed_{ false };
};

void no_noexcept_test()
{
    Foo foo(false);
    try {
        utils::whatever a(foo);
    } catch (...) {
    }

    try {
        utils::whatever a;
        a = foo;
    } catch (...) {
    }

    Foo bar(true);
    utils::whatever a(bar);
    utils::whatever_cast<Foo>(&a)->copyAllowed_ = false;

    try {
        utils::whatever b(a);
    } catch (...) {
    }

    try {
        utils::whatever c;
        c = a;
    } catch (...) {
    }
}

int main()
{
    no_noexcept_test();
    return 0;
}
