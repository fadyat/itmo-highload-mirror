#include "../solution/unique_ptr.hpp"
#include "../solution/unique_ptr.hpp"

#include <type_traits>
#include <string>

#include "simple_test.h"
#include "destroyable_object.h"

TEST(unique_ptr, type_check) {
  using int_unique_ptr = cls10::unique_ptr<int>;

  static_assert(
      !std::is_copy_constructible_v<int_unique_ptr>,
      "unique_ptr should not be copiable!");

  static_assert(
      !std::is_copy_assignable_v<int_unique_ptr>,
      "unique_ptr should not be assignable with '='!");

  static_assert(
      std::is_nothrow_move_assignable_v<int_unique_ptr>,
      "unique_ptr should be movable!");

  static_assert(
      std::is_nothrow_move_constructible_v<int_unique_ptr>,
      "unique_ptr should be move constructible!");

  static_assert(
      std::is_nothrow_default_constructible_v<int_unique_ptr>,
      "unique_ptr should be nothrow default constructible");

  static_assert(
      std::is_constructible_v<bool, const int_unique_ptr>,
      "unique_ptr should be explicitly convertible to bool");
  static_assert(
      !std::is_convertible_v<int_unique_ptr, bool>,
      "unique_ptr should NOT be implicitly convertible to bool");
}

////////////////////////////////////////////////////////////////////////////////
// конструкторы

TEST(default_ctor, form_1) {
  cls10::unique_ptr<int> u;
  EXPECT_FALSE((bool)u);
  EXPECT_TRUE(!u);
  EXPECT_EQ(nullptr, u.get());
}

TEST(default_ctor, form_2) {
  cls10::unique_ptr<int> u{};
  EXPECT_FALSE((bool)u);
  EXPECT_TRUE(!u);
  EXPECT_EQ(nullptr, u.get());
}

TEST(unary_ctor, nullptr) {
  // неявное приведение нульпоинтера к умному указателю - безопасно
  static_assert(
      std::is_convertible_v<std::nullptr_t, cls10::unique_ptr<int>>,
      "unique_ptr can be implicitly constructed from nullptr");
  cls10::unique_ptr<int> u = nullptr;
  EXPECT_FALSE((bool)u);
  EXPECT_TRUE(!u);
  EXPECT_EQ(nullptr, u.get());
}

TEST(unary_ctor, null_value) {
  // неявное приведение голого указателя к умному указателю - не безопасно
  // (этим указателем может владеть кто-то другой)
  // поэтому конструировать умный указатель из голого нужно явно
  static_assert(
      !std::is_convertible_v<int*, cls10::unique_ptr<int>>,
      "unique_ptr should not be implicitly constructed from a pointer");
  int* n = nullptr;
  cls10::unique_ptr<int> u{n};
  EXPECT_FALSE((bool)u);
  EXPECT_TRUE(!u);
  EXPECT_EQ(nullptr, u.get());
}

TEST(unary_ctor, valuable) {
  using Data = std::pair<int, int>;

  Data* p = new Data{100, 200};

  cls10::unique_ptr<Data> u{p};
  EXPECT_TRUE((bool)u);
  EXPECT_FALSE(!u);

  const auto& c = std::as_const(u);

  static_assert(std::is_same_v<Data*, decltype(c.get())>);
  static_assert(std::is_same_v<Data&, decltype(*c)>);

  // проверяем функции адреса
  EXPECT_EQ(p, c.get());
  EXPECT_EQ(p, std::to_address(c));
  // и разыменование
  EXPECT_EQ(p, &*c);
  EXPECT_EQ(100, c->first);
  EXPECT_EQ(200, c->second);
  // и неконстантный доступ
  c->first++;
  c->second++;
  EXPECT_EQ(101, c->first);
  EXPECT_EQ(201, c->second);
}

////////////////////////////////////////////////////////////////////////////////

TEST(destructor, calls_only_once) {
  int dtor_count = 0;
  {
    cls10::unique_ptr<Object> u {new Object(dtor_count)};
    ASSERT_TRUE((bool)u);
  }
  EXPECT_EQ(1, dtor_count);
}

////////////////////////////////////////////////////////////////////////////////
// move

TEST(move_ctor, from_empty) {
  cls10::unique_ptr<int> u;
  ASSERT_EQ(nullptr, u.get());
  cls10::unique_ptr<int> v{std::move(u)};
  EXPECT_EQ(nullptr, u.get());
  EXPECT_EQ(nullptr, v.get());
}

TEST(move_ctor, from_valuable) {
  int dtor_count = 0;
  {
    Object* raw = new Object(dtor_count);
    cls10::unique_ptr<Object> u {raw};
    ASSERT_EQ(0, dtor_count);
    ASSERT_EQ(raw, u.get());

    auto v = std::move(u);
    ASSERT_EQ(0, dtor_count);
    EXPECT_EQ(nullptr, u.get());
    EXPECT_EQ(raw, v.get());
  }
  EXPECT_EQ(1, dtor_count);
}

TEST(move_assign, empty_from_empty) {
  cls10::unique_ptr<int> u;
  cls10::unique_ptr<int> v;
  ASSERT_FALSE((bool)u);
  ASSERT_FALSE((bool)v);

  u = std::move(v);
  EXPECT_FALSE((bool)u);
  EXPECT_FALSE((bool)v);
}

TEST(move_assign, empty_from_valuable) {
  int dtor_count = 0;
  {
    Object* raw = new Object(dtor_count);
    cls10::unique_ptr<Object> u;
    cls10::unique_ptr<Object> v {raw};
    ASSERT_EQ(nullptr, u.get());
    ASSERT_EQ(raw, v.get());
    ASSERT_EQ(0, dtor_count);

    u = std::move(v);
    EXPECT_EQ(raw, u.get());
    EXPECT_EQ(nullptr, v.get());
  }  // удаляет объект
  EXPECT_EQ(1, dtor_count);
}

TEST(move_assign, valuable_from_empty) {
  int dtor_count = 0;
  {
    Object* raw = new Object(dtor_count);
    cls10::unique_ptr<Object> u {raw};
    cls10::unique_ptr<Object> v;
    ASSERT_EQ(raw, u.get());
    ASSERT_EQ(nullptr, v.get());
    ASSERT_EQ(0, dtor_count);

    u = std::move(v);  // удаляет объект
    EXPECT_EQ(nullptr, u.get());
    EXPECT_EQ(nullptr, v.get());
    EXPECT_EQ(1, dtor_count);
  }
  EXPECT_EQ(1, dtor_count);
}

TEST(move_assign, valuable_from_valuable) {
  int dtor_count_u = 0;
  int dtor_count_v = 0;
  {
    Object* raw_u = new Object(dtor_count_u);
    Object* raw_v = new Object(dtor_count_v);
    cls10::unique_ptr<Object> u {raw_u};
    cls10::unique_ptr<Object> v {raw_v};
    ASSERT_EQ(raw_u, u.get());
    ASSERT_EQ(raw_v, v.get());
    ASSERT_EQ(0, dtor_count_u);
    ASSERT_EQ(0, dtor_count_v);

    u = std::move(v);  // удаляет первый объект
    EXPECT_EQ(raw_v, u.get());
    EXPECT_EQ(nullptr, v.get());

    EXPECT_EQ(1, dtor_count_u);
    EXPECT_EQ(0, dtor_count_v);
  }  // удаляет второй объект
  EXPECT_EQ(1, dtor_count_v);
}

////////////////////////////////////////////////////////////////////////////////
// reset

TEST(reset, empty) {
  cls10::unique_ptr<Object> u;
  u.reset();
  EXPECT_FALSE((bool)u);
  u.reset(nullptr);
  EXPECT_FALSE((bool)u);
}

TEST(reset, valuable_to_empty) {
  int dtor_count = 0;
  cls10::unique_ptr<Object> u {new Object{dtor_count}};
  u.reset();  // удаляет старый объект
  EXPECT_EQ(nullptr, u.get());
  EXPECT_EQ(1, dtor_count);
}

TEST(reset, empty_to_valuable) {
  int dtor_count = 0;
  {
    cls10::unique_ptr<Object> u;
    ASSERT_FALSE((bool)u);

    Object* raw = new Object{dtor_count};
    u.reset(raw);
    EXPECT_EQ(raw, u.get());
    EXPECT_EQ(0, dtor_count);
  }  // удаляет объект
  EXPECT_EQ(1, dtor_count);
}

TEST(reset, valuable_to_valuable) {
  int dtor_count_u = 0;
  int dtor_count_v = 0;
  {
    Object* raw_u = new Object{dtor_count_u};
    cls10::unique_ptr<Object> u {raw_u};
    ASSERT_EQ(raw_u, u.get());
    ASSERT_EQ(0, dtor_count_u);

    Object* raw_v = new Object{dtor_count_v};
    u.reset(raw_v);  // удаляет старый объект
    EXPECT_EQ(raw_v, u.get());
    EXPECT_EQ(1, dtor_count_u);
    EXPECT_EQ(0, dtor_count_v);
  }  // удаляет новый объект
  EXPECT_EQ(1, dtor_count_v);
}

////////////////////////////////////////////////////////////////////////////////
// release

TEST(release, empty) {
  cls10::unique_ptr<Object> u;
  EXPECT_EQ(nullptr, u.release());
  EXPECT_EQ(nullptr, u.get());
}

TEST(release, valuable) {
  int dtor_count = 0;
  Object* raw = new Object{dtor_count};
  {
    cls10::unique_ptr<Object> u {raw};
    ASSERT_EQ(raw, u.get());
    ASSERT_EQ(0, dtor_count);

    EXPECT_EQ(raw, u.release());  // не удаляет объект
    EXPECT_EQ(nullptr, u.get());
    EXPECT_EQ(0, dtor_count);
  }
  ASSERT_EQ(0, dtor_count);
  delete raw;
  ASSERT_EQ(1, dtor_count);
}

////////////////////////////////////////////////////////////////////////////////
// make_unique

TEST(make_unique, ctor_with_no_args) {
  auto u = cls10::make_unique<std::string>();
  static_assert(std::is_same_v<decltype(u), cls10::unique_ptr<std::string>>);
  ASSERT_TRUE((bool)u);
  EXPECT_TRUE(u->empty());
}

TEST(make_unique, ctor_with_1_arg) {
  auto u = cls10::make_unique<std::string>("hello");
  static_assert(std::is_same_v<decltype(u), cls10::unique_ptr<std::string>>);
  ASSERT_TRUE((bool)u);
  EXPECT_EQ("hello", *u);
}

TEST(make_unique, ctor_with_2_args) {
  auto u = cls10::make_unique<std::string>(5, 'h');
  static_assert(std::is_same_v<decltype(u), cls10::unique_ptr<std::string>>);
  ASSERT_TRUE((bool)u);
  EXPECT_EQ("hhhhh", *u);
}

TEST(make_unique, perfect_forwarding) {
  struct Moveable {
    Moveable() = default;
    Moveable(Moveable&&) = default;
    Moveable(const Moveable&) = delete;
  };

  struct Copyable {
    Copyable() = default;
    Copyable(Copyable&&) = default;
    Copyable(const Copyable&) = delete;
  };

  struct Noncopyable {
    Noncopyable() = default;
    Noncopyable(Noncopyable&&) = delete;
    Noncopyable(const Noncopyable&) = delete;
  };

  struct Foo {
    Foo(Moveable&&, const Copyable&, Noncopyable&) {}
  };
  struct Bar {
    Bar(Moveable&&, const Copyable&, const Noncopyable&) {}
  };

  Noncopyable noncopyable;

  std::make_unique<Foo>(Moveable{}, Copyable{}, noncopyable);
  std::make_unique<Bar>(Moveable{}, Copyable{}, noncopyable);
  std::make_unique<Bar>(Moveable{}, Copyable{}, std::as_const(noncopyable));
}

////////////////////////////////////////////////////////////////////////////////

TESTING_MAIN()
