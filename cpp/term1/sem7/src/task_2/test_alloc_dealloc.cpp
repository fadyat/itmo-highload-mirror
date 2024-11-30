#include "leak_detector.hpp"
#include <cassert>
#include <new>

static void TestAllocDealloc() {
    int* some_addr = new int;

    for (int i = 0; i < 100000; i++) {
        cls07::LeakDetector detector;
        assert(detector.GetNextLeakedAddress() == some_addr);
        assert(detector.GetNextLeakedAddress() == nullptr);

        int* addr1 = new int;

        *addr1 = i * i;
        *some_addr = i + *addr1;
        delete addr1;
    }
    delete some_addr;
    cls07::LeakDetector detector;
    assert(detector.GetNextLeakedAddress() == nullptr);
}

int main() {
    TestAllocDealloc();
    return 0;
}