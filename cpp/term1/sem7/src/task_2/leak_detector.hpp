#pragma once
#include <cstddef>

void *operator new(size_t s);
void operator delete(void *addr) noexcept;

namespace cls07 {

struct Meta {
    Meta *next;
    size_t size;
    bool isFree;
};

struct LeakDetector {
  private:
    // Head is a pointer to the first element of the list with allocated memory.
    Meta *head;

    // Current is a pointer to the last processed element of the list with allocated memory
    // used to iterate through the list and define leaks.
    Meta *current;

  public:
    LeakDetector();
    ~LeakDetector();

    void *GetNextLeakedAddress();
};

}
