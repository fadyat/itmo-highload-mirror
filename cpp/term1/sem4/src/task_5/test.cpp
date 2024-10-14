#include <cassert>

extern "C" {
#include "sum.h"
}

int main() {
    assert(sum(10, 20) == 30);
    return 0;
}
