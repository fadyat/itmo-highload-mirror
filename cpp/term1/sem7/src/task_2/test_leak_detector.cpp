#include "leak_detector.hpp"
#include <iostream>
#include <cassert>


static void test_alloc() {
    char* a = new char{'a'};
    char* b = new char{'b'};

    {
        cls07::LeakDetector ld;
        assert(ld.GetNextLeakedAddress() == a);
        assert(ld.GetNextLeakedAddress() == b);
        assert(!ld.GetNextLeakedAddress());
    }

    char* c = new char{'c'};

    assert(c - b == b - a);
    // implementation defined
    // assert(c - b == 0x20);

    delete b;
    {
        cls07::LeakDetector ld;
        assert(ld.GetNextLeakedAddress() == a);
        assert(ld.GetNextLeakedAddress() == c);
        assert(!ld.GetNextLeakedAddress());
    }

    char *newB = new char{'b'};
    assert(b == newB);
    {
        cls07::LeakDetector ld;
        assert(ld.GetNextLeakedAddress() == a);
        assert(ld.GetNextLeakedAddress() == b);
        assert(ld.GetNextLeakedAddress() == c);
        assert(!ld.GetNextLeakedAddress());
    }
}

int main() {
    test_alloc();

    return 0;
}
