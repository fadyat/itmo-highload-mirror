#include "maybe.hpp"
#include "maybe.hpp"

#include <cassert>
#include <string>
#include <iostream>
#include <functional>

#include "simple_test.h"

namespace
{
  std::size_t glb_alloc{ 0 };
  std::size_t glb_dealloc{ 0 };
}

void* operator new(std::size_t sz)
{
  glb_alloc += 1;
  return std::malloc(sz);
}

void operator delete(void* ptr) noexcept
{
  glb_dealloc += 1;
  std::free(ptr);
}

TEST(memory, no_heap_allocations) {
  size_t was_alloc = glb_alloc;
  size_t was_dealloc = glb_dealloc;
  {
    control_03::maybe<int> mb;
    EXPECT_EQ(glb_alloc, was_alloc);

    mb.reset(123);
    EXPECT_EQ(glb_alloc, was_alloc);

    mb = 456;
    EXPECT_EQ(glb_alloc, was_alloc);

    control_03::maybe<int> mb2(mb);
    EXPECT_EQ(glb_alloc, was_alloc);

    control_03::maybe<int> mb3(789);
    EXPECT_EQ(glb_alloc, was_alloc);
  }
  EXPECT_EQ(glb_dealloc, was_dealloc);
}

TESTING_MAIN()
