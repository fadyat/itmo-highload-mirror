#pragma once

////////////////////////////////////////////////////////////////////////////////
// объект, который следит за своим деструктором

struct ObjectBase {
    virtual ~ObjectBase() = default;
};

struct Object : ObjectBase {
  int& dtor_count;

  explicit Object(int& c) : dtor_count(c) {}
  Object(Object&) = delete;
  Object& operator=(Object&) = delete;
  ~Object() override { ++dtor_count; }
};
