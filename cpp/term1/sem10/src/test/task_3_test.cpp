#include "../solution/shared_ptr.hpp"
#include "../solution/shared_ptr.hpp"

#include <type_traits>
#include <string>
#include <functional>

#include "simple_test.h"
#include "destroyable_object.h"

#include <cassert>

TEST(weak_ptr, type_check) {
  using int_weak_ptr = cls10::weak_ptr<int>;
  using int_shared_ptr = cls10::shared_ptr<int>;

  static_assert(
      std::is_copy_constructible_v<int_weak_ptr>,
      "weak_ptr should be copiable!");

  static_assert(
      std::is_copy_assignable_v<int_weak_ptr>,
      "weak_ptr should be assignable with '='!");

  static_assert(
      std::is_nothrow_move_assignable_v<int_weak_ptr>,
      "weak_ptr should be movable!");

  static_assert(
      std::is_nothrow_move_constructible_v<int_weak_ptr>,
      "weak_ptr should be move constructible!");

  static_assert(
      std::is_nothrow_default_constructible_v<int_weak_ptr>,
      "weak_ptr should be nothrow default constructible");

  static_assert(
      std::is_convertible_v<int_shared_ptr, int_weak_ptr>,
      "weak_ptr should be constructible from shared_ptr");
}

////////////////////////////////////////////////////////////////////////////////
// конструкторы

TEST(default_ctor, empty) {
  cls10::weak_ptr<int> w;

  EXPECT_TRUE(w.expired());
  auto s = w.lock();
  static_assert(std::is_same_v<decltype(s), cls10::shared_ptr<int>>);
  EXPECT_FALSE((bool)s);
}

// из shared_ptr

TEST(from_shared, empty) {
  cls10::shared_ptr<int> s0;

  cls10::weak_ptr<int> w = std::as_const(s0);

  EXPECT_TRUE(w.expired());
  EXPECT_FALSE((bool)w.lock());
}

TEST(from_shared, valuable) {
  cls10::shared_ptr<int> s0 {new int{123}};

  cls10::weak_ptr<int> w = std::as_const(s0);

  EXPECT_FALSE(w.expired());
  auto s = w.lock();
  EXPECT_TRUE((bool)s);
  EXPECT_EQ(s0.get(), s.get());
}

////////////////////////////////////////////////////////////////////////////////
// заготовки источников для конструкторов и операторов присваивания

template<class T> struct Empty {
  cls10::weak_ptr<T> weak;

  Empty() {
    // предусловие для дальнейшего использования: указатель пустой
    ASSERT_TRUE(weak.expired());
    ASSERT_TRUE(!weak.lock());
  }
};

template<class T> struct Valuable {
  const cls10::shared_ptr<T> shared;  // исходные данные сохраняются!
  cls10::weak_ptr<T> weak;

  explicit Valuable(auto&& ... args)
    : shared(cls10::make_shared<T>(std::forward<decltype(args)>(args)...))
    , weak(shared)
  {
    // предусловие для дальнейшего использования: указатель содержательный
    ASSERT_TRUE((bool)shared);
    ASSERT_FALSE(weak.expired());
    ASSERT_EQ(shared.get(), weak.lock().get());
  }
};

////////////////////////////////////////////////////////////////////////////////
// копирование

TEST(copy_ctor, empty) {
  const Empty<int> src;

  cls10::weak_ptr<int> w = src.weak;

  EXPECT_TRUE(w.expired());
  EXPECT_TRUE(!w.lock());
}

TEST(copy_ctor, valuable) {
  const Valuable<int> src{123};

  cls10::weak_ptr<int> w = src.weak;

  EXPECT_FALSE(w.expired());
  auto s = w.lock();
  EXPECT_EQ(src.shared.get(), s.get());
}

// перемещение

TEST(move_ctor, empty) {
  Empty<int> src;

  cls10::weak_ptr<int> w = std::move(src.weak);

  EXPECT_TRUE(src.weak.expired());
  EXPECT_TRUE(!src.weak.lock());

  EXPECT_TRUE(w.expired());
  EXPECT_TRUE(!w.lock());
}

TEST(move_ctor, valuable) {
  Valuable<int> src{123};

  cls10::weak_ptr<int> w = std::move(src.weak);

  EXPECT_TRUE(src.weak.expired());
  EXPECT_TRUE(!src.weak.lock());

  EXPECT_FALSE(w.expired());
  auto s = w.lock();
  EXPECT_TRUE((bool)s);
  EXPECT_EQ(src.shared.get(), s.get());
}

// копирующее присваивание

TEST(copy_assign, empty_from_empty) {
  const Empty<int> src;
  Empty<int> dst;

  dst.weak = src.weak;

  EXPECT_TRUE(dst.weak.expired());
  EXPECT_TRUE(!dst.weak.lock());
}

TEST(copy_assign, valuable_from_empty) {
  const Empty<int> src;
  Valuable<int> dst{456};

  dst.weak = src.weak;

  EXPECT_TRUE(dst.weak.expired());
  EXPECT_TRUE(!dst.weak.lock());
}

TEST(copy_assign, empty_from_valuable) {
  const Valuable<int> src{123};
  Empty<int> dst;

  dst.weak = src.weak;

  EXPECT_FALSE(dst.weak.expired());
  EXPECT_EQ(src.shared.get(), dst.weak.lock().get());
}

TEST(copy_assign, valuable_from_valuable) {
  const Valuable<int> src{123};
  Valuable<int> dst{456};

  dst.weak = src.weak;

  EXPECT_FALSE(dst.weak.expired());
  EXPECT_EQ(src.shared.get(), dst.weak.lock().get());
}

TEST(copy_assign, empty_self_assign) {
  Empty<int> src;

  src.weak = std::as_const(src.weak);

  EXPECT_TRUE(src.weak.expired());
  EXPECT_TRUE(!src.weak.lock());
}

TEST(copy_assign, valuable_self_assign) {
  Valuable<int> src{123};

  src.weak = std::as_const(src.weak);

  EXPECT_FALSE(src.weak.expired());
  EXPECT_EQ(src.shared.get(), src.weak.lock().get());
}

TEST(copy_assign, valuable_indirect_self_assign) {
  Valuable<int> src{123};

  src.weak = cls10::weak_ptr<int>{std::as_const(src.weak)};

  EXPECT_FALSE(src.weak.expired());
  EXPECT_EQ(src.shared.get(), src.weak.lock().get());
}

// перемещающее присваивание

TEST(move_assign, empty_from_empty) {
  Empty<int> src;
  Empty<int> dst;

  dst.weak = std::move(src.weak);

  EXPECT_TRUE(src.weak.expired());
  EXPECT_TRUE(!src.weak.lock());

  EXPECT_TRUE(dst.weak.expired());
  EXPECT_TRUE(!dst.weak.lock());
}

TEST(move_assign, valuable_from_empty) {
  Empty<int> src;
  Valuable<int> dst{456};
  // обратите внимание, что shared мы не трогаем, это референсные значения

  dst.weak = std::move(src.weak);

  EXPECT_TRUE(src.weak.expired());
  EXPECT_TRUE(!src.weak.lock());

  EXPECT_TRUE(dst.weak.expired());
  EXPECT_TRUE(!dst.weak.lock());
}

TEST(move_assign, empty_from_valuable) {
  Valuable<int> src{123};
  Empty<int> dst;

  dst.weak = std::move(src.weak);

  EXPECT_TRUE(src.weak.expired());
  EXPECT_TRUE(!src.weak.lock());

  EXPECT_FALSE(dst.weak.expired());
  EXPECT_EQ(src.shared.get(), dst.weak.lock().get());
}

TEST(move_assign, valuable_from_valuable) {
  Valuable<int> src{123};
  Valuable<int> dst{456};

  dst.weak = std::move(src.weak);

  EXPECT_TRUE(src.weak.expired());
  EXPECT_TRUE(!src.weak.lock());

  EXPECT_FALSE(dst.weak.expired());
  EXPECT_EQ(src.shared.get(), dst.weak.lock().get());
}

////////////////////////////////////////////////////////////////////////////////

TEST(reset, empty) {
  Empty<int> src;

  src.weak.reset();
  EXPECT_TRUE(src.weak.expired());
  EXPECT_TRUE(!src.weak.lock());
}

TEST(reset, valuable) {
  Valuable<int> src{123};

  src.weak.reset();
  EXPECT_TRUE(src.weak.expired());
  EXPECT_TRUE(!src.weak.lock());
}

////////////////////////////////////////////////////////////////////////////////

TEST(lifetime, lock_cares_about_lifetime) {
  int dtor_count = 0;
  {
    auto s = cls10::make_shared<Object>(dtor_count);
    cls10::weak_ptr<Object> w {s};
    ASSERT_EQ(0, dtor_count);
    ASSERT_FALSE(w.expired());

    // временные указатели lock)() - владение добавляется и убавляется
    // но пока жив постоянный указатель s, жив и указуемый объект
    EXPECT_EQ(s.get(), w.lock().get());
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(s.get(), w.lock().get());
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(s.get(), w.lock().get());
    EXPECT_EQ(0, dtor_count);

    // продлеваем в другом указателе
    auto s1 = w.lock();
    s.reset();
    EXPECT_EQ(0, dtor_count);
  }
  EXPECT_EQ(1, dtor_count);  // ничего не утекло
}

TEST(lifetime, reset_makes_it_expired) {
  int dtor_count = 0;
  auto s = cls10::make_shared<Object>(dtor_count);
  cls10::weak_ptr<Object> w {s};
  ASSERT_EQ(0, dtor_count);
  ASSERT_FALSE(w.expired());

  s.reset();

  EXPECT_EQ(1, dtor_count);
  EXPECT_TRUE(w.expired());
  EXPECT_TRUE(!w.lock());
}

TEST(lifetime, reassign_makes_it_expired) {
  int dtor_count = 0;
  int dtor_count_2 = 0;
  {
    auto s = cls10::make_shared<Object>(dtor_count);
    cls10::weak_ptr<Object> w {s};
    ASSERT_EQ(0, dtor_count);
    ASSERT_FALSE(w.expired());

    s.reset(new Object(dtor_count_2));

    EXPECT_EQ(0, dtor_count_2);
    EXPECT_EQ(1, dtor_count);

    EXPECT_TRUE(w.expired());
    EXPECT_TRUE(!w.lock());
  }
  EXPECT_EQ(1, dtor_count_2);
}

TEST(lifetime, countdown_makes_it_expired) {
  int dtor_count = 0;
  auto s = cls10::make_shared<Object>(dtor_count);
  auto s1 = s;
  auto s2 = s;

  // количество слабых указателей ни на что не влияет
  cls10::weak_ptr<Object> w1 {s};
  cls10::weak_ptr<Object> w2 {s};

  ASSERT_EQ(0, dtor_count);
  ASSERT_FALSE(w1.expired());
  ASSERT_FALSE(w2.expired());

  s.reset();
  EXPECT_EQ(0, dtor_count);
  EXPECT_FALSE(w1.expired());
  EXPECT_FALSE(w2.expired());

  s1.reset();
  EXPECT_EQ(0, dtor_count);
  EXPECT_FALSE(w1.expired());
  EXPECT_FALSE(w2.expired());

  s2.reset();  // потеряно последнее владение
  EXPECT_EQ(1, dtor_count);
  EXPECT_TRUE(w1.expired());
  EXPECT_TRUE(w2.expired());

  EXPECT_TRUE(!w1.lock());
  EXPECT_TRUE(!w2.lock());
}

////////////////////////////////////////////////////////////////////////////////

TEST(practical_example, with_lambda_binding) {
  std::function<void(bool, int)> fun;

  {
    auto shared = cls10::make_shared<int>(123);

    fun = [weak = cls10::weak_ptr<int>(shared)](bool expected_alive, int expected_value) {
      if (auto locked = weak.lock()) {
        EXPECT_TRUE(expected_alive);
        EXPECT_EQ(expected_value, *locked);
        ++*locked;
      } else {
        EXPECT_FALSE(expected_alive);
      }
    };

    fun(true, 123);
    *shared = 456;
    fun(true, 456);
    fun(true, 457);
  }

  // при этом лямбда не владеет объектом и не продлевает его жизнь (не создаёт мягких утечек)
  fun(false, 0);
  fun(false, 0);
}

TEST(practical_example, circular_references) {
  // узел списка владеет своим хвостом и знает про своего владельца
  struct Node : Object {
    int value;
    cls10::shared_ptr<Node> forward;
    cls10::weak_ptr<Node> backward;  // если тут будет shared_ptr, возникнет утечка
    using Object::Object;
  };

  int ctor_count = 0;
  int dtor_count = 0;

  auto make_list = [&ctor_count, &dtor_count](int v, cls10::shared_ptr<Node> next) {
    auto node = cls10::make_shared<Node>(dtor_count);
    ++ctor_count;

    node->value = v;
    node->forward = next;
    if (next) next->backward = node;

    return node;
  };

  ctor_count = dtor_count = 0;
  {
    auto list = make_list(123, nullptr);

    EXPECT_EQ(1, ctor_count);
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(123, list->value);
  }
  EXPECT_EQ(ctor_count, dtor_count);

  ctor_count = dtor_count = 0;
  {
    auto list = make_list(123, make_list(456, nullptr));

    EXPECT_EQ(2, ctor_count);
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(123, list->value);
    EXPECT_EQ(456, list->forward->value);
  }
  EXPECT_EQ(ctor_count, dtor_count);

  ctor_count = dtor_count = 0;
  {
    auto list = make_list(123, make_list(456, make_list(789, nullptr)));

    EXPECT_EQ(3, ctor_count);
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(123, list->value);
    EXPECT_EQ(456, list->forward->value);
    EXPECT_EQ(789, list->forward->forward->value);
    EXPECT_EQ(456, list->forward->forward->backward.lock()->value);
    EXPECT_EQ(123, list->forward->forward->backward.lock()->backward.lock()->value);

    EXPECT_EQ(0, dtor_count);
  }
  EXPECT_EQ(ctor_count, dtor_count);

  ctor_count = dtor_count = 0;
  {
    auto list = make_list(123, make_list(456, make_list(789, nullptr)));

    EXPECT_EQ(3, ctor_count);
    EXPECT_EQ(0, dtor_count);

    EXPECT_EQ(123, list->value);
    EXPECT_EQ(456, list->forward->value);
    EXPECT_EQ(789, list->forward->forward->value);
    EXPECT_EQ(456, list->forward->forward->backward.lock()->value);
    EXPECT_EQ(123, list->forward->forward->backward.lock()->backward.lock()->value);

    EXPECT_EQ(0, dtor_count);

    auto tail = list->forward->forward;
    list = nullptr;
    EXPECT_EQ(2, dtor_count);

    EXPECT_EQ(789, tail->value);
    EXPECT_TRUE(tail->backward.expired());
  }
  EXPECT_EQ(ctor_count, dtor_count);
}

////////////////////////////////////////////////////////////////////////////////

TESTING_MAIN()
