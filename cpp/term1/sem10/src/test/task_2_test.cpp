#include "../solution/shared_ptr.hpp"
#include "../solution/shared_ptr.hpp"

#include <type_traits>
#include <string>
#include <functional>

#include "simple_test.h"
#include "destroyable_object.h"



TEST(shared_ptr, type_check) {
  using int_shared_ptr = cls10::shared_ptr<int>;

  static_assert(
      std::is_copy_constructible_v<int_shared_ptr>,
      "shared_ptr should be copiable!");

  static_assert(
      std::is_copy_assignable_v<int_shared_ptr>,
      "shared_ptr should be assignable with '='!");

  static_assert(
      std::is_nothrow_move_assignable_v<int_shared_ptr>,
      "shared_ptr should be movable!");

  static_assert(
      std::is_nothrow_move_constructible_v<int_shared_ptr>,
      "shared_ptr should be move constructible!");

  static_assert(
      std::is_nothrow_default_constructible_v<int_shared_ptr>,
      "shared_ptr should be nothrow default constructible");

  static_assert(
      std::is_constructible_v<bool, const int_shared_ptr>,
      "shared_ptr should be explicitly convertible to bool");
  static_assert(
      !std::is_convertible_v<int_shared_ptr, bool>,
      "shared_ptr should NOT be implicitly convertible to bool");
}

////////////////////////////////////////////////////////////////////////////////
// конструкторы

TEST(default_ctor, form_1) {
  cls10::shared_ptr<int> u;
  EXPECT_FALSE((bool)u);
  EXPECT_TRUE(!u);
  EXPECT_EQ(nullptr, u.get());
}

TEST(default_ctor, form_2) {
  cls10::shared_ptr<int> u{};
  EXPECT_FALSE((bool)u);
  EXPECT_TRUE(!u);
  EXPECT_EQ(nullptr, u.get());
}

TEST(unary_ctor, nullptr) {
  // неявное приведение нульпоинтера к умному указателю - безопасно
  static_assert(
      std::is_convertible_v<std::nullptr_t, cls10::shared_ptr<int>>,
      "shared_ptr can be implicitly constructed from nullptr");

  cls10::shared_ptr<int> u = nullptr;
  EXPECT_FALSE((bool)u);
  EXPECT_TRUE(!u);
  EXPECT_EQ(nullptr, u.get());
}

TEST(unary_ctor, null_value) {
  // неявное приведение голого указателя к умному указателю - не безопасно
  // (этим указателем может владеть кто-то другой)
  // поэтому конструировать умный указатель из голого нужно явно
  static_assert(
      !std::is_convertible_v<int*, cls10::shared_ptr<int>>,
      "shared_ptr should not be implicitly constructed from a pointer");

  int* n = nullptr;
  cls10::shared_ptr<int> u{n};
  EXPECT_FALSE((bool)u);
  EXPECT_TRUE(!u);
  EXPECT_EQ(nullptr, u.get());
}

TEST(unary_ctor, valuable) {
  using Data = std::pair<int, int>;

  Data* p = new Data{100, 200};

  cls10::shared_ptr<Data> u{p};
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
    cls10::shared_ptr<Object> u {new Object(dtor_count)};
    ASSERT_TRUE((bool)u);
  }
  EXPECT_EQ(1, dtor_count);
}

////////////////////////////////////////////////////////////////////////////////
// all about move

TEST(move_ctor, from_empty) {
  cls10::shared_ptr<int> u;
  ASSERT_EQ(nullptr, u.get());
  cls10::shared_ptr<int> v{std::move(u)};
  EXPECT_EQ(nullptr, u.get());
  EXPECT_EQ(nullptr, v.get());
}

TEST(move_ctor, from_valuable) {
  int dtor_count = 0;
  {
    Object* raw = new Object(dtor_count);
    cls10::shared_ptr<Object> u {raw};
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
  cls10::shared_ptr<int> u;
  cls10::shared_ptr<int> v;
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
    cls10::shared_ptr<Object> u;
    cls10::shared_ptr<Object> v {raw};
    ASSERT_EQ(nullptr, u.get());
    ASSERT_EQ(raw, v.get());
    ASSERT_EQ(0, dtor_count);

    u = std::move(v);
    EXPECT_EQ(raw, u.get());
    EXPECT_EQ(nullptr, v.get());
  }  // удаляет объект
  EXPECT_EQ(1, dtor_count);
}

TEST(move_assign, unique_valuable_from_empty) {
  int dtor_count = 0;
  {
    Object* raw = new Object(dtor_count);
    cls10::shared_ptr<Object> u {raw};
    cls10::shared_ptr<Object> v;
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

TEST(move_assign, unique_valuable_from_valuable) {
  int dtor_count_u = 0;
  int dtor_count_v = 0;
  {
    Object* raw_u = new Object(dtor_count_u);
    Object* raw_v = new Object(dtor_count_v);
    cls10::shared_ptr<Object> u {raw_u};
    cls10::shared_ptr<Object> v {raw_v};
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

TEST(move_assign, shared_valuable_from_empty) {
    int dtor_count_u = 0;
    {
        auto* raw_u = new Object{dtor_count_u};

        cls10::shared_ptr<Object> u{raw_u};
        cls10::shared_ptr<Object> v;
        cls10::shared_ptr<Object> w = u;  // shared
        ASSERT_EQ(0, dtor_count_u);
        ASSERT_EQ(raw_u, u.get());
        ASSERT_EQ(nullptr, v.get());
        ASSERT_EQ(raw_u, w.get());

        u = std::move(v);  // raw_u остался жить в w
        EXPECT_EQ(0, dtor_count_u);
        EXPECT_EQ(nullptr, u.get());
        EXPECT_EQ(nullptr, v.get());
    }  // удаляем
    EXPECT_EQ(1, dtor_count_u);
}

TEST(move_assign, shared_valuable_from_valuable) {
    int dtor_count_u = 0;
    int dtor_count_v = 0;
    {
        auto* raw_u = new Object{dtor_count_u};
        auto* raw_v = new Object{dtor_count_v};

        cls10::shared_ptr<Object> u{raw_u};
        cls10::shared_ptr<Object> v{raw_v};
        cls10::shared_ptr<Object> w = u;  // shared
        ASSERT_EQ(0, dtor_count_u);
        ASSERT_EQ(0, dtor_count_v);
        ASSERT_EQ(raw_u, u.get());
        ASSERT_EQ(raw_v, v.get());
        ASSERT_EQ(raw_u, w.get());

        u = std::move(v);  // raw_u остался жить в w
        EXPECT_EQ(0, dtor_count_u);
        EXPECT_EQ(0, dtor_count_v);
        EXPECT_EQ(raw_v, u.get());
        EXPECT_EQ(nullptr, v.get());
    }  // удаляем
    EXPECT_EQ(1, dtor_count_u);
    EXPECT_EQ(1, dtor_count_v);
}

////////////////////////////////////////////////////////////////////////////////
// all about copy

TEST(copy_ctor, from_empty) {
  cls10::shared_ptr<int> u;
  ASSERT_EQ(nullptr, u.get());
  cls10::shared_ptr<int> v{std::as_const(u)};
  EXPECT_EQ(nullptr, u.get());
  EXPECT_EQ(nullptr, v.get());
}

TEST(copy_ctor, from_valuable) {
  int dtor_count = 0;
  {
    Object* raw = new Object(dtor_count);
    cls10::shared_ptr<Object> u {raw};
    ASSERT_EQ(0, dtor_count);
    ASSERT_EQ(raw, u.get());

    auto v = std::as_const(u);
    ASSERT_EQ(0, dtor_count);
    EXPECT_EQ(raw, u.get());
    EXPECT_EQ(raw, v.get());

    auto w = std::as_const(u);
    ASSERT_EQ(0, dtor_count);
    EXPECT_EQ(raw, u.get());
    EXPECT_EQ(raw, v.get());
    EXPECT_EQ(raw, w.get());
  }
  EXPECT_EQ(1, dtor_count);
}

TEST(copy_assign, empty_from_empty) {
  cls10::shared_ptr<int> u;
  cls10::shared_ptr<int> v;
  ASSERT_FALSE((bool)u);
  ASSERT_FALSE((bool)v);

  u = std::as_const(v);
  EXPECT_FALSE((bool)u);
  EXPECT_FALSE((bool)v);
}

TEST(copy_assign, empty_from_valuable) {
  int dtor_count = 0;
  {
    Object* raw = new Object(dtor_count);
    cls10::shared_ptr<Object> u;
    cls10::shared_ptr<Object> v {raw};
    ASSERT_EQ(nullptr, u.get());
    ASSERT_EQ(raw, v.get());
    ASSERT_EQ(0, dtor_count);

    u = std::as_const(v);
    EXPECT_EQ(raw, u.get());
    EXPECT_EQ(raw, v.get());
  }  // удаляет объект
  EXPECT_EQ(1, dtor_count);
}

TEST(copy_assign, unique_valuable_from_empty) {
  int dtor_count = 0;
  {
    Object* raw = new Object(dtor_count);
    cls10::shared_ptr<Object> u {raw};
    cls10::shared_ptr<Object> v;
    ASSERT_EQ(raw, u.get());
    ASSERT_EQ(nullptr, v.get());
    ASSERT_EQ(0, dtor_count);

    u = std::as_const(v);  // удаляет объект
    EXPECT_EQ(nullptr, u.get());
    EXPECT_EQ(nullptr, v.get());
    EXPECT_EQ(1, dtor_count);
  }
  EXPECT_EQ(1, dtor_count);
}

TEST(copy_assign, unique_valuable_from_valuable) {
  int dtor_count_u = 0;
  int dtor_count_v = 0;
  {
    Object* raw_u = new Object(dtor_count_u);
    Object* raw_v = new Object(dtor_count_v);
    cls10::shared_ptr<Object> u {raw_u};
    cls10::shared_ptr<Object> v {raw_v};
    ASSERT_EQ(raw_u, u.get());
    ASSERT_EQ(raw_v, v.get());
    ASSERT_EQ(0, dtor_count_u);
    ASSERT_EQ(0, dtor_count_v);

    u = std::as_const(v);  // удаляет первый объект
    EXPECT_EQ(raw_v, u.get());
    EXPECT_EQ(raw_v, v.get());

    EXPECT_EQ(1, dtor_count_u);
    EXPECT_EQ(0, dtor_count_v);
  }  // удаляет второй объект
  EXPECT_EQ(1, dtor_count_v);
}

TEST(copy_assign, shared_valuable_from_empty) {
    int dtor_count_u = 0;
    {
        auto* raw_u = new Object{dtor_count_u};

        cls10::shared_ptr<Object> u{raw_u};
        cls10::shared_ptr<Object> v;
        cls10::shared_ptr<Object> w = u;  // shared
        ASSERT_EQ(0, dtor_count_u);
        ASSERT_EQ(raw_u, u.get());
        ASSERT_EQ(nullptr, v.get());
        ASSERT_EQ(raw_u, w.get());

        u = std::as_const(v);  // raw_u остался жить в w
        EXPECT_EQ(0, dtor_count_u);
        EXPECT_EQ(nullptr, u.get());
    }  // удаляем
    EXPECT_EQ(1, dtor_count_u);
}

TEST(copy_assign, shared_valuable_from_valuable) {
    int dtor_count_u = 0;
    int dtor_count_v = 0;
    {
        auto* raw_u = new Object{dtor_count_u};
        auto* raw_v = new Object{dtor_count_v};

        cls10::shared_ptr<Object> u{raw_u};
        cls10::shared_ptr<Object> v{raw_v};
        cls10::shared_ptr<Object> w = u;  // shared
        ASSERT_EQ(0, dtor_count_u);
        ASSERT_EQ(0, dtor_count_v);
        ASSERT_EQ(raw_u, u.get());
        ASSERT_EQ(raw_v, v.get());
        ASSERT_EQ(raw_u, w.get());

        u = std::as_const(v);  // raw_u остался жить в w
        EXPECT_EQ(0, dtor_count_u);
        EXPECT_EQ(0, dtor_count_v);
        EXPECT_EQ(raw_v, u.get());
    }  // удаляем
    EXPECT_EQ(1, dtor_count_u);
    EXPECT_EQ(1, dtor_count_v);
}

TEST(copy_assign, empty_self_assign) {
    cls10::shared_ptr<Object> u;
    ASSERT_EQ(nullptr, u.get());

    u = std::as_const(u);
    EXPECT_EQ(nullptr, u.get());
}

TEST(copy_assign, valuable_self_assign) {
    int dtor_count = 0;
    {
        auto* raw = new Object{dtor_count};

        cls10::shared_ptr<Object> u {raw};
        ASSERT_EQ(raw, u.get());
        ASSERT_EQ(0, dtor_count);

        u = std::as_const(u);
        EXPECT_EQ(raw, u.get());
        EXPECT_EQ(0, dtor_count);
    }
    EXPECT_EQ(1, dtor_count);
}

TEST(copy_assign, valuable_indirect_self_assign) {
    int dtor_count = 0;
    {
        auto* raw = new Object{dtor_count};

        cls10::shared_ptr<Object> u {raw};
        ASSERT_EQ(raw, u.get());
        ASSERT_EQ(0, dtor_count);

        u = cls10::shared_ptr<Object>{std::as_const(u)};
        EXPECT_EQ(raw, u.get());
        EXPECT_EQ(0, dtor_count);
    }
    EXPECT_EQ(1, dtor_count);
}

////////////////////////////////////////////////////////////////////////////////
// reset

TEST(reset, empty) {
  cls10::shared_ptr<Object> u;
  u.reset();
  EXPECT_FALSE((bool)u);
  u.reset(nullptr);
  EXPECT_FALSE((bool)u);
}

TEST(reset, valuable_to_empty) {
  int dtor_count = 0;
  cls10::shared_ptr<Object> u {new Object{dtor_count}};
  cls10::shared_ptr<Object> v {u};  // shared

  u.reset();  // объект ещё жив
  EXPECT_EQ(nullptr, u.get());
  EXPECT_EQ(0, dtor_count);

  v.reset();  // удаляет старый объект
  EXPECT_EQ(1, dtor_count);
}

TEST(reset, empty_to_valuable) {
  int dtor_count = 0;
  {
    cls10::shared_ptr<Object> u;
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
  int dtor_count_w = 0;
  {
    Object* raw_u = new Object{dtor_count_u};
    cls10::shared_ptr<Object> u {raw_u};
    cls10::shared_ptr<Object> w {u};  // shared
    ASSERT_EQ(raw_u, u.get());
    ASSERT_EQ(0, dtor_count_u);

    Object* raw_v = new Object{dtor_count_v};
    u.reset(raw_v);  // старый объект ещё жив
    EXPECT_EQ(raw_v, u.get());
    EXPECT_EQ(0, dtor_count_u);
    EXPECT_EQ(0, dtor_count_v);

    Object* raw_w = new Object{dtor_count_w};
    w.reset(raw_w);  // а вот теперь удаляет
    EXPECT_EQ(1, dtor_count_u);
    EXPECT_EQ(0, dtor_count_v);
    EXPECT_EQ(0, dtor_count_w);
  }  // удаляет новые объекты
  EXPECT_EQ(1, dtor_count_v);
  EXPECT_EQ(1, dtor_count_w);
}

////////////////////////////////////////////////////////////////////////////////
// make_shared

TEST(make_shared, ctor_with_no_args) {
  auto u = cls10::make_shared<std::string>();
  static_assert(std::is_same_v<decltype(u), cls10::shared_ptr<std::string>>);
  ASSERT_TRUE((bool)u);
  EXPECT_TRUE(u->empty());
}

TEST(make_shared, ctor_with_1_arg) {
  auto u = cls10::make_shared<std::string>("hello");
  static_assert(std::is_same_v<decltype(u), cls10::shared_ptr<std::string>>);
  ASSERT_TRUE((bool)u);
  EXPECT_EQ("hello", *u);
}

TEST(make_shared, ctor_with_2_args) {
  auto u = cls10::make_shared<std::string>(5, 'h');
  static_assert(std::is_same_v<decltype(u), cls10::shared_ptr<std::string>>);
  ASSERT_TRUE((bool)u);
  EXPECT_EQ("hhhhh", *u);
}

TEST(make_shared, perfect_forwarding) {
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

  std::make_shared<Foo>(Moveable{}, Copyable{}, noncopyable);
  std::make_shared<Bar>(Moveable{}, Copyable{}, noncopyable);
  std::make_shared<Bar>(Moveable{}, Copyable{}, std::as_const(noncopyable));
}

////////////////////////////////////////////////////////////////////////////////

TEST(practical_example, with_lambda_binding) {
  std::function<void(int)> fun;

  {
    auto shared = cls10::make_shared<int>(123);

    fun = [shared](int expected_value) {
      EXPECT_TRUE((bool)shared);
      EXPECT_EQ(expected_value, *shared);
      ++*shared;
    };

    fun(123);
    *shared = 456;
    fun(456);
    fun(457);
  }

  // лямбда продолжает владеть объектом
  fun(458);
  fun(459);
}

TEST(practical_example, single_linked_list) {
  // узел владеет хвостом - и рекурсивно удаляет его
  // (если на другие узлы нет сторонних указателей)
  struct Node : Object {
    int value;
    cls10::shared_ptr<Node> forward;

    using Object::Object;
  };

  int ctor_count = 0;
  int dtor_count = 0;

  auto make_list = [&ctor_count, &dtor_count](int v, cls10::shared_ptr<Node> next) {
    auto node = cls10::make_shared<Node>(dtor_count);
    ++ctor_count;

    node->value = v;
    node->forward = std::move(next);
    return node;
  };

  dtor_count = ctor_count = 0;
  {
    auto list = make_list(123, nullptr);
    EXPECT_EQ(1, ctor_count);
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(123, list->value);
  }
  EXPECT_EQ(ctor_count, dtor_count);

  dtor_count = ctor_count = 0;
  {
    auto list = make_list(123, make_list(456, nullptr));
    EXPECT_EQ(2, ctor_count);
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(123, list->value);
    EXPECT_EQ(456, list->forward->value);
  }
  EXPECT_EQ(ctor_count, dtor_count);

  dtor_count = ctor_count = 0;
  {
    auto list = make_list(123, make_list(456, make_list(789, nullptr)));
    EXPECT_EQ(3, ctor_count);
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(123, list->value);
    EXPECT_EQ(456, list->forward->value);
    EXPECT_EQ(789, list->forward->forward->value);
  }

  dtor_count = ctor_count = 0;
  {
    auto list = make_list(123, make_list(456, make_list(789, nullptr)));
    EXPECT_EQ(3, ctor_count);
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(123, list->value);
    EXPECT_EQ(456, list->forward->value);
    EXPECT_EQ(789, list->forward->forward->value);

    auto tail = list->forward->forward;
    list = nullptr;
    EXPECT_EQ(2, dtor_count);
    EXPECT_EQ(789, tail->value);
  }
  EXPECT_EQ(ctor_count, dtor_count);
}


TESTING_MAIN()

