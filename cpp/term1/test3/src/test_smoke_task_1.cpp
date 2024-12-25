#include "maybe.hpp"
#include "maybe.hpp"

#include <cassert>
#include <string>
#include <iostream>
#include <functional>

#include "test_utils.hpp"

#include "simple_test.h"

TEST(maybe, check_size) {
  struct Value { double data[1000]; };
  using MaybeValue = control_03::maybe<Value>;

  EXPECT_LT(sizeof(Value), sizeof(MaybeValue));
  EXPECT_LT(sizeof(MaybeValue), sizeof(Value) + sizeof(double)*2);
}

TEST(maybe, check_alignment) {
  struct Value { double data; };
  using MaybeValue = control_03::maybe<Value>;

  static_assert(alignof(Value) > 1);
  EXPECT_EQ(alignof(Value), alignof(MaybeValue));

  MaybeValue mb{ Value{} };
  ptrdiff_t data_offset = intptr_t(&mb.value()) - intptr_t(&mb);
  EXPECT_EQ(0, data_offset % alignof(Value));
}


TEST(maybe, construct_empty) {
  struct Value {
    Value() = delete;  // для maybe не требуется дефолтный конструктор
  };

  using MaybeValue = control_03::maybe<Value>;

  MaybeValue mb0;
  EXPECT_FALSE(mb0.has_value());

  MaybeValue mb1 = mb0;
  EXPECT_FALSE(mb1.has_value());

  MaybeValue mb2 = std::move(mb0);
  EXPECT_FALSE(mb0.has_value());
}

TEST(maybe, construct_from_copyable) {
  DEFINE_COUNTED_CLASS(ValueBase)

  struct Value : ValueBase {
    int x;

    explicit Value(int x) : x(x) {}
    Value(const Value&) = default;

    Value(Value&&) = delete;
    Value& operator=(Value&) = delete;  // присваивание в конструкторе не нужно
  };

  using MaybeValue = control_03::maybe<Value>;

  const Value v{123};
  Value::counters().reset();  // исключим внешнее значение из подсчёта

  {
    MaybeValue mb0{v};
    EXPECT_TRUE((std::is_same_v<Value&, decltype(mb0.value())>));
    EXPECT_TRUE((std::is_same_v<const Value&, decltype(std::as_const(mb0).value())>));

    EXPECT_EQ(1, Value::counters().alive());  // mb0
    EXPECT_EQ(1, Value::counters().copy_ctor);
    EXPECT_TRUE(mb0.has_value());
    EXPECT_EQ(v.x, mb0.value().x);
    EXPECT_NE(&v, &mb0.value());  // maybe не запоминает ссылку на первоисточник

    MaybeValue mb1 = std::as_const(mb0);
    EXPECT_EQ(2, Value::counters().alive());  // mb0, mb1
    EXPECT_EQ(2, Value::counters().copy_ctor);
    EXPECT_TRUE(mb1.has_value());
    EXPECT_EQ(v.x, mb1.value().x);
    EXPECT_NE(&v.x, &mb1.value().x);
    EXPECT_NE(&mb0.value(), &mb1.value());
    // copy-семантика
    EXPECT_TRUE(mb0.has_value());
    EXPECT_EQ(v.x, mb0.value().x);

    // переменные существуют независимо
    mb0.value().x += 10;
    mb1.value().x += 100;
    EXPECT_EQ(v.x + 10, mb0.value().x);
    EXPECT_EQ(v.x + 100, mb1.value().x);
  }
  EXPECT_EQ(0, Value::counters().alive());
}

TEST(maybe, construct_from_moveable) {
  DEFINE_COUNTED_CLASS(ValueBase)

  struct Value : ValueBase {
    std::string x;

    explicit Value(const std::string& x) : x(x) {}
    Value(Value&&) = default;

    Value(const Value&) = delete;
    Value& operator=(Value&) = delete;
  };

  using MaybeValue = control_03::maybe<Value>;

  const std::string vx = "123";
  Value v{vx};
  Value::counters().reset();

  {
    MaybeValue mb0{std::move(v)};  // используется move-конструктор
    EXPECT_EQ(1, Value::counters().alive());
    EXPECT_EQ(1, Value::counters().move_ctor);
    EXPECT_TRUE(mb0.has_value());
    EXPECT_EQ(vx, mb0.value().x);

    MaybeValue mb1 = std::move(mb0);  // используется move-конструктор
    EXPECT_EQ(mb0.has_value() + mb1.has_value(), Value::counters().alive());
    EXPECT_EQ(2, Value::counters().move_ctor);
    EXPECT_TRUE(mb1.has_value());
    EXPECT_EQ(vx, mb1.value().x);
    // move-семантика:
    // либо значение полностью перемещено и разрушено в источнике,
    // либо объект остался жить, но в состоянии "после перемещения"
    if (mb0.has_value()) {
      EXPECT_TRUE(mb0.value().x.empty());  // например, строка отдаёт своё содержание в приёмник
    }
  }
  EXPECT_EQ(0, Value::counters().alive());
}

TEST(maybe, assign_with_copyable) {
  DEFINE_COUNTED_CLASS(ValueBase)

  struct Value : ValueBase {
    int x;
    explicit Value(int x) : x(x) {}

    Value(const Value&) = default;
    Value(Value&&) = delete;

    Value& operator=(const Value&) = default;
    Value& operator=(Value&&) = delete;
  };
  using MaybeValue = control_03::maybe<Value>;

  const Value v{123};
  Value::counters().reset();  // исключим внешнее значение из подсчёта

  {
    MaybeValue mb0, mb1, mb2;
    EXPECT_EQ(0, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().all_ctors());

    ASSERT_FALSE(mb0.has_value());
    ASSERT_FALSE(mb1.has_value());
    ASSERT_FALSE(mb2.has_value());

    // присваивание пустому пустого
    mb0 = mb1;
    EXPECT_EQ(0, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().all_ctors());
    EXPECT_FALSE(mb0.has_value());

    // присваивание пустому значения
    mb0 = v;  // пустой maybe конструирует объект
    EXPECT_EQ(1, Value::counters().alive());  // mb0
    EXPECT_EQ(1, Value::counters().copy_ctor);  // +1 copy ctor
    EXPECT_EQ(0, Value::counters().all_assigns());
    ASSERT_TRUE(mb0.has_value());
    EXPECT_EQ(v.x, mb0.value().x);

    // присваивание пустому значащего
    mb1 = std::as_const(mb0);
    EXPECT_EQ(2, Value::counters().alive());  // mb0, mb1
    EXPECT_EQ(2, Value::counters().copy_ctor);  // +1 copy ctor
    EXPECT_EQ(0, Value::counters().all_assigns());
    ASSERT_TRUE(mb1.has_value());
    EXPECT_EQ(v.x, mb1.value().x);
    // семантика копирования
    ASSERT_TRUE(mb0.has_value());
    EXPECT_EQ(v.x, mb0.value().x);
    // это всё разные экземпляры
    EXPECT_NE(&v, &mb0.value());
    EXPECT_NE(&v, &mb1.value());
    EXPECT_NE(&mb0.value(), &mb1.value());

    // самоприсвоение не должно ничего делать
    mb0 = std::as_const(mb0);
    EXPECT_EQ(2, Value::counters().alive());  // mb0, mb1
    EXPECT_EQ(2, Value::counters().copy_ctor);  // не изменилось
    EXPECT_EQ(0, Value::counters().all_assigns());
    ASSERT_TRUE(mb0.has_value());
    EXPECT_EQ(v.x, mb0.value().x);

    // присвоение значащему значащего
    mb1.value().x += 1000;  // чтобы убедиться, что мы изменим значение ниже
    mb1 = mb0;
    EXPECT_EQ(2, Value::counters().alive());  // mb0, mb1
    EXPECT_EQ(2, Value::counters().copy_ctor);  // не изменилось
    EXPECT_EQ(1, Value::counters().copy_assign);  // +1
    EXPECT_EQ(0, Value::counters().move_assign);
    ASSERT_TRUE(mb1.has_value());
    EXPECT_EQ(v.x, mb1.value().x);

    // присвоение значащему пустого
    mb1 = mb2;
    EXPECT_EQ(1, Value::counters().alive());  // mb0
    EXPECT_EQ(2, Value::counters().copy_ctor);  // не изменилось
    EXPECT_EQ(1, Value::counters().copy_assign);  // не изменилось
    EXPECT_EQ(0, Value::counters().move_assign);
    EXPECT_FALSE(mb1.has_value());
  }
  EXPECT_EQ(0, Value::counters().alive());
}

TEST(maybe, assign_with_moveable) {
  DEFINE_COUNTED_CLASS(ValueBase)

  struct Value : ValueBase {
    std::string x;
    explicit Value(const std::string& x) : x(x) {}

    Value(const Value&) = delete;
    Value(Value&&) = default;

    Value& operator=(const Value&) = delete;
    Value& operator=(Value&&) = default;
  };
  using MaybeValue = control_03::maybe<Value>;

  const std::string vx = "123";
  Value v{vx}, v3{vx};
  Value::counters().reset();  // исключим внешнее значение из подсчёта

  {
    MaybeValue mb0, mb1, mb2, mb3;
    auto actual_alive = [&]() {
      return mb0.has_value() + mb1.has_value() + mb2.has_value() + mb3.has_value();
    };

    EXPECT_EQ(0, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().all_ctors());

    ASSERT_FALSE(mb0.has_value());
    ASSERT_FALSE(mb1.has_value());
    ASSERT_FALSE(mb2.has_value());
    ASSERT_FALSE(mb3.has_value());

    // присваивание пустому пустого
    mb0 = std::move(mb1);
    EXPECT_EQ(0, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().all_ctors());
    EXPECT_FALSE(mb0.has_value());

    // присваивание пустому значения
    mb0 = std::move(v);  // пустой maybe конструирует объект
    EXPECT_EQ(actual_alive(), Value::counters().alive());  // mb0
    EXPECT_EQ(1, Value::counters().move_ctor);  // +1 move ctor
    EXPECT_EQ(0, Value::counters().all_assigns());
    ASSERT_TRUE(mb0.has_value());
    EXPECT_EQ(vx, mb0.value().x);

    // нам понадобится для опытов ещё один экземпляр
    mb3 = std::move(v3);  // пустой maybe конструирует объект
    EXPECT_EQ(actual_alive(), Value::counters().alive());  // mb0, mb3
    EXPECT_EQ(2, Value::counters().move_ctor);  // +1 move ctor
    EXPECT_EQ(0, Value::counters().all_assigns());
    ASSERT_TRUE(mb3.has_value());
    EXPECT_EQ(vx, mb3.value().x);

    // присваивание пустому значащего
    mb1 = std::move(mb0);
    EXPECT_EQ(actual_alive(), Value::counters().alive());  // mb1, mb3, возможно mb0
    EXPECT_EQ(3, Value::counters().move_ctor);  // +1 move ctor
    EXPECT_EQ(0, Value::counters().all_assigns());
    ASSERT_TRUE(mb1.has_value());
    EXPECT_EQ(vx, mb1.value().x);
    // семантика перемещения
    if (mb0.has_value()) {
      EXPECT_TRUE(mb0.value().x.empty());
    }

    // присвоение значащему значащего
    mb1.value().x += "456";  // чтобы убедиться, что мы изменим значение ниже
    mb1 = std::move(mb3);
    EXPECT_EQ(actual_alive(), Value::counters().alive());  // mb1, возможно mb0 и mb3
    EXPECT_EQ(3, Value::counters().move_ctor);  // не изменилось
    EXPECT_EQ(1, Value::counters().move_assign);  // +1
    ASSERT_TRUE(mb1.has_value());
    EXPECT_EQ(vx, mb1.value().x);
    // семантика перемещения
    if (mb3.has_value()) {
      EXPECT_TRUE(mb3.value().x.empty());
    }

    // присвоение значащему пустого
    mb1 = std::move(mb2);
    EXPECT_EQ(actual_alive(), Value::counters().alive());  // возможно mb0 и mb3
    EXPECT_EQ(3, Value::counters().move_ctor);  // не изменилось
    EXPECT_EQ(1, Value::counters().move_assign);  // не изменилось
    EXPECT_FALSE(mb1.has_value());
  }
  EXPECT_EQ(0, Value::counters().alive());
}

TEST(maybe, boolean) {
  struct Value {};
  using MaybeValue = control_03::maybe<Value>;

  EXPECT_TRUE((std::is_constructible_v<bool, MaybeValue>));
  EXPECT_FALSE((std::is_convertible_v<MaybeValue, bool>));

  MaybeValue empty;
  MaybeValue full{Value{}};

  EXPECT_FALSE(static_cast<bool>(empty));
  EXPECT_TRUE(static_cast<bool>(full));

  if (empty) {
    ADD_FAILURE() << "expected empty as false";
  }
  if (full) ; else {
    ADD_FAILURE() << "expected full as true";
  }
}

TEST(maybe, why_implicit_bool_is_bad_idea) {
  [](auto){
    // неявное преобразование может создавать путаницу для программиста
    // (не для компилятора - он-то, возможно, и скомпилирует)
    // между значением и признаком
    // например, разнообразные опечатки при рефакторинге,
    // замене T на maybe<T>, окажутся незамеченными и не исправленными
    //
    // все EXPECT здесь - ловушки! как если бы мы ошиблись при рефакторинге.

    bool old_b = false;
    control_03::maybe<bool> new_b = false;

    int old_i = 123;
    control_03::maybe<int> new_i = 123;

    struct Funs {
      static void f(bool v) { EXPECT_EQ(false, v) << "это ловушка"; }
      static void f(int v) { EXPECT_EQ(123, v) << "это ловушка"; }
    };

    if constexpr (requires{ Funs::f(new_b); }) {
      Funs::f(old_b);
      Funs::f(new_b);
    }
    if constexpr (requires{ Funs::f(new_i); }) {
      Funs::f(old_i);
      Funs::f(new_i);
    }

    // хак! компилятор слишком рано проверяет, можно ли выполнить присваивание,
    // поэтому нужно прикрыть ему глазки дополнительным шаблоном - тогда он честно
    // сделает проверку requires и ветвление по её результатам
    auto try_assign = [](auto& dst, const auto& src, const auto& expected) {
      if constexpr(requires { dst = src; }) {
        dst = src;
        EXPECT_EQ(expected, dst) << "это ловушка!";
      }
    };
    bool b;
    try_assign(b, old_b, false);
    try_assign(b, new_b, false);

    int i;
    try_assign(i, old_i, 123);
    try_assign(i, new_i, 123);

  }(0);
}

TEST(maybe, why_implicit_constructor_is_not_so_bad) {
  [](auto){
    // рассмотрим обратную ситуацию при рефакторинге
    struct Funs {
      static bool fun_v0(bool b) {
        EXPECT_EQ(false, b);
        return true;
      }
      static bool fun_v1(control_03::maybe<bool> mb) {
        EXPECT_TRUE(mb.has_value());
        EXPECT_EQ(false, mb.value());
        return true;
      }
      static bool fun_v2(control_03::maybe<bool> mb) {
        if (mb) {
          EXPECT_EQ(false, mb.value());
          return true;
        } else {
          return false;
        }
      }
    };

    EXPECT_TRUE(Funs::fun_v0(false));
    EXPECT_TRUE(Funs::fun_v1(false));
    EXPECT_TRUE(Funs::fun_v2(false));
    EXPECT_FALSE(Funs::fun_v2({}));
  }(0);
}

TEST(maybe, reset) {
  DEFINE_COUNTED_CLASS(ValueBase)

  struct Value : ValueBase {
    int x;
    explicit Value(int x) : x(x) {}
  };
  using MaybeValue = control_03::maybe<Value>;

  Value v1{123}, v2{456};
  Value::counters().reset();  // исключим внешнее значение из подсчёта

  {
    MaybeValue mb;
    EXPECT_EQ(0, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().dtor);

    // сброс пустого - ничего не делает
    mb.reset();
    EXPECT_EQ(0, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().dtor);
    EXPECT_FALSE(mb.has_value());
  }

  Value::counters().reset();

  {
    MaybeValue mb{std::as_const(v1)};
    EXPECT_EQ(1, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().dtor);

    // сброс значащего
    mb.reset();
    EXPECT_EQ(0, Value::counters().alive());
    EXPECT_EQ(1, Value::counters().dtor);
    EXPECT_FALSE(mb.has_value());
  }

  Value::counters().reset();

  {
    MaybeValue mb;
    EXPECT_EQ(0, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().dtor);
    EXPECT_EQ(0, Value::counters().copy_ctor);

    // сброс пустого - конструирует
    mb.reset(std::as_const(v2));
    EXPECT_EQ(1, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().dtor);
    EXPECT_EQ(1, Value::counters().copy_ctor);
    EXPECT_TRUE(mb.has_value());
    EXPECT_EQ(v2.x, mb.value().x);
  }

  Value::counters().reset();

  {
    MaybeValue mb{std::as_const(v1)};
    EXPECT_EQ(1, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().dtor);
    EXPECT_EQ(1, Value::counters().copy_ctor);

    // сброс значащего - пересоздаёт
    mb.reset(std::as_const(v2));
    EXPECT_EQ(1, Value::counters().alive());
    EXPECT_EQ(1, Value::counters().dtor);
    EXPECT_EQ(2, Value::counters().copy_ctor);
    EXPECT_TRUE(mb.has_value());
    EXPECT_EQ(v2.x, mb.value().x);
  }

  Value::counters().reset();

  {
    MaybeValue mb;
    EXPECT_EQ(0, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().dtor);
    EXPECT_EQ(0, Value::counters().move_ctor);

    // сброс пустого - конструирует
    mb.reset(std::move(v2));
    EXPECT_EQ(1, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().dtor);
    EXPECT_EQ(1, Value::counters().move_ctor);
    EXPECT_TRUE(mb.has_value());
    EXPECT_EQ(v2.x, mb.value().x);
  }

  Value::counters().reset();

  {
    MaybeValue mb{std::as_const(v1)};
    EXPECT_EQ(1, Value::counters().alive());
    EXPECT_EQ(0, Value::counters().dtor);
    EXPECT_EQ(1, Value::counters().copy_ctor);
    EXPECT_EQ(0, Value::counters().move_ctor);

    // сброс значащего - пересоздаёт
    mb.reset(std::move(v2));
    EXPECT_EQ(1, Value::counters().alive());
    EXPECT_EQ(1, Value::counters().dtor);
    EXPECT_EQ(1, Value::counters().copy_ctor);
    EXPECT_EQ(1, Value::counters().move_ctor);
    EXPECT_TRUE(mb.has_value());
    EXPECT_EQ(v2.x, mb.value().x);
  }
}

////////////////////////////////////////////////////////////////////////////////
// практический пример: арифметика над maybe-числами
// эксплуатирует неявное приведение типа

using MaybeInt = control_03::maybe<int>;

MaybeInt operator + (MaybeInt a, MaybeInt b) {
  if (a && b) {
    return a.value() + b.value();
  } else {
    return {};
  }
}

MaybeInt operator - (MaybeInt a, MaybeInt b) {
  if (a && b) {
    return a.value() - b.value();
  } else {
    return {};
  }
}

MaybeInt operator * (MaybeInt a, MaybeInt b) {
  if (a && b) {
    return a.value() * b.value();
  } else {
    return {};
  }
}

MaybeInt operator / (MaybeInt a, MaybeInt b) {
  if (a && b && b.value() != 0) {  // ради чего это всё затевалось
    return a.value() / b.value();
  } else {
    return {};  // при делении на 0 вернём "не шмогла!"
  }
}

TEST(maybe, some_practical_example) {
  auto expected = 1 * 2 * 3 * 4 / 5 + 6 - 7 + 0;
  auto ok = MaybeInt(1) * 2 * 3 * 4 / 5 + 6 - 7 + 0;
  EXPECT_TRUE(ok.has_value());
  EXPECT_EQ(expected, ok.value());

  auto bad = MaybeInt(1) * 2 * 3 * 4 / 0 + 6 - 7 + 0;
  EXPECT_FALSE(bad.has_value());

  auto some_ratio = [](int a, int b) {
    return (a + b) / MaybeInt(a - b);
  };
  EXPECT_EQ(10, some_ratio(11, 9).value());
  EXPECT_FALSE(some_ratio(10, 10).has_value());
}

TESTING_MAIN()
