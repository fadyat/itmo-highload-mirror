#include "leak_detector.hpp"
#include <cassert>
#include <new>

static void TestExtensiveAlloc() {
    int* addresses[200] = {0};

    size_t allocated = 0;
    for (; allocated < 200; allocated++) {
        try {
            int* new_address = new int[allocated + 1];
            addresses[allocated] = new_address;
        } catch (std::bad_alloc& e) {
            break;
        }
    }
    cls07::LeakDetector detector;
    for (int* address : addresses) {
        assert(detector.GetNextLeakedAddress() == address);
    }
    assert(detector.GetNextLeakedAddress() == nullptr);
    for (int* address : addresses) {
        delete[] address;
    }

    cls07::LeakDetector detector2;
    assert(detector2.GetNextLeakedAddress() == nullptr);
}

int main() {
    TestExtensiveAlloc();
    return 0;
}