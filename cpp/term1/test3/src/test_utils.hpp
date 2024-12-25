#pragma once

struct MethodCounters {
  int default_ctor = 0;
  int copy_ctor = 0;
  int move_ctor = 0;
  int copy_assign = 0;
  int move_assign = 0;
  int dtor = 0;

  int all_ctors() const { return default_ctor + copy_ctor + move_ctor; }
  int all_assigns() const { return copy_assign + move_assign; }
  int alive() const { return all_ctors() - dtor; }

  void reset() {
    *this = MethodCounters{};
  }
};


#define DEFINE_COUNTED_CLASS(ClassName) \
    struct ClassName {                                                                       \
        static auto& counters() { static MethodCounters c; return c; }                       \
        ClassName() { ++counters().default_ctor; }                                           \
        ClassName(const ClassName&) { ++counters().copy_ctor; }                              \
        ClassName& operator=(const ClassName &) { ++counters().copy_assign; return *this; }  \
        ClassName(ClassName&&) { ++counters().move_ctor; }                                   \
        ClassName& operator=(ClassName &&) { ++counters().move_assign; return *this; }       \
        ~ClassName() { ++counters().dtor; }                                                  \
    };
