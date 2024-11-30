#include "leak_detector.hpp"
#include <iostream>
#include <new>

namespace cls07 {

constexpr static size_t POOL_SIZE = 65536;

// pool is a static array, which emulates the memory pool.
// here we allocating requested memory blocks and storing meta structures.
char pool[POOL_SIZE];

// allocations is a linked list of Meta structures, which are stored in the pool
// and contain information about the allocated memory blocks.
Meta *allocations = nullptr;

LeakDetector::LeakDetector() : head(allocations), current(head) {}

LeakDetector::~LeakDetector() {
    while (head != nullptr) {
        Meta *next = head->next;
        if (!head->isFree) {
            std::cerr << "Memory leak at " << head << std::endl;
        }

        head = next;
    }
}

void *LeakDetector::GetNextLeakedAddress() {
    while (current != nullptr) {
        if (!current->isFree) {
            void *addr = reinterpret_cast<void *>(current + 1);
            current = current->next;
            return addr;
        }

        current = current->next;
    }

    return nullptr;
}

}

void *operator new(size_t s) {
    using namespace cls07;

    size_t pool_offset = 0, total_size = s + sizeof(Meta);
    Meta *previous = nullptr, *current = allocations;

    while (current != nullptr) {
        // trying to take already allocated block, which is free
        // and has enough size to allocate a new block
        if (current->isFree && current->size >= s) {
            current->isFree = false;
            return reinterpret_cast<void *>(current + 1);
        }

        // if we can't find a suitable block, we should move to the next one
        // and remember the offset for the future allocations
        pool_offset += current->size + sizeof(Meta);
        previous = current;
        current = current->next;
    }

    // there is no suitable block found no enought space left
    // to allocate a new block with requested size
    if (pool_offset + total_size > POOL_SIZE) {
        throw std::bad_alloc();
    }

    Meta *new_block = reinterpret_cast<Meta *>(pool + pool_offset);
    new_block->next = nullptr;
    new_block->isFree = false;
    new_block->size = s;

    if (previous != nullptr) {
        previous->next = new_block;
    } else {
        allocations = new_block;
    }

    return reinterpret_cast<void *>(new_block + 1);
}

void operator delete(void *addr) noexcept {
    using namespace cls07;

    if (addr == nullptr) {
        return;
    }

    Meta *block = reinterpret_cast<Meta *>(addr) - 1;
    block->isFree = true;
}
