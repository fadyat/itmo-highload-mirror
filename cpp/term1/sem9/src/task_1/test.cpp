#include "queue.hpp"
#include "queue.hpp"


#include "../simple_test.h"
#include <stdexcept>


TEST(queue, empty) {
    cls09::queue<int> q;
    EXPECT_EQ(0u, q.size());
}

TEST(queue, empty_is_lightweight) {
    struct Heavy {
        Heavy() = delete;
        char cargo[10000];
    };

    cls09::queue<Heavy> q;
    EXPECT_LT(sizeof(q), sizeof(Heavy));
}


struct Counters {
    int ctor = 0;
    int copy_ctor = 0;
    int move_ctor = 0;
    int copy_assign = 0;
    int move_assign = 0;
    int dtor = 0;

    int all_ctors() const { return ctor + copy_ctor + move_ctor; }
    int all_assigns() const { return copy_assign + move_assign; }

    int all_copy() const { return copy_ctor + copy_assign; }
    int all_move() const { return move_ctor + move_assign; }

    int alive() const { return all_ctors() - dtor; }

    void reset() {
        *this = Counters{};
    }
};

class Item {
public:
    static Counters counters;

private:
    static constexpr int GARBAGE = 12345678;

    static int copy_data(const Item& other) {
        ASSERT_TRUE(other.alive);
        ASSERT_NE(GARBAGE, other.data);  // копируемый объект - содержательный (не после move)
        return other.data;
    }
    static int move_data(Item& other) {
        ASSERT_TRUE(other.alive);
        ASSERT_NE(GARBAGE, other.data);
        return std::exchange(other.data, GARBAGE);
    }

    static unsigned check_fire(unsigned f) {
        if (f == 0) return f;  // no throw
        if (f == 1) throw std::runtime_error("fire some problems");
        return f - 1;
    }
    static int copy_fire(const Item& other) {
        return check_fire(other.fire);
    }
    static unsigned move_fire(Item& other) {
        return check_fire(std::exchange(other.fire, 0));
    }

    int data;
    unsigned fire = 0;
    bool alive = true;  // сбрасывается в деструкторе

public:
    int value() const {
        ASSERT_NE(GARBAGE, data);
        return data;
    }

    explicit Item(int v, int f = 0) : data(v), fire(f) {
        counters.ctor++;
    }
    Item(const Item& other)
    : data(copy_data(other))
    , fire(copy_fire(other)) {
        counters.copy_ctor++;
    }
    Item(Item&& other)
    : data(move_data(other))
    , fire(move_fire(other)) {
        counters.move_ctor++;
    }
    Item& operator = (const Item& other) {
        ASSERT_TRUE(alive);
        data = copy_data(other);
        fire = copy_fire(other);
        counters.copy_assign++;
        return *this;
    }
    Item& operator = (Item&& other) {
        ASSERT_NE(&other, this);  // сами себя не перемещаем
        ASSERT_TRUE(alive);
        data = move_data(other);
        fire = move_fire(other);
        counters.move_assign++;
        return *this;
    }
    ~Item() {
        EXPECT_TRUE(alive);
        data = GARBAGE;
        alive = false;
        counters.dtor++;
    }
};

Counters Item::counters = {};


TEST(queue, push_front_pop) {
    Item::counters.reset();

    cls09::queue<Item> q;
    EXPECT_EQ(0, Item::counters.all_ctors());

    const size_t n = 5;
    for (size_t i = 0; i != n; ++i) {
        EXPECT_EQ(i, q.size());

        q.push(Item(i));

        EXPECT_EQ(i+1, q.size());
        ASSERT_TRUE(q.size());
        EXPECT_EQ(0, q.front().value());
    }

    EXPECT_EQ(n, Item::counters.alive());  // ничего не утекло
    EXPECT_EQ(0, Item::counters.all_assigns());  // не использовали присваивание

    const int no_more_constructed = Item::counters.all_ctors();

    for (size_t i = 0; i != n; ++i) {
        EXPECT_EQ(n - i, q.size());
        ASSERT_TRUE(q.size());
        EXPECT_EQ(i, q.front().value());

        q.pop();

        EXPECT_EQ(n - i - 1, q.size());
    }

    EXPECT_EQ(0, Item::counters.alive());  // все объекты уничтожены
    EXPECT_EQ(no_more_constructed, Item::counters.all_ctors());  // ничего более не конструировали
}

TEST(queue, destroy_fulfilled_queue) {
    Item::counters.reset();

    {
        cls09::queue<Item> q;
        // наполняем данными
        const size_t n = 5;
        for (size_t i = 0; i != n; ++i) {
            q.push(Item(i));
        }
        ASSERT_EQ(n, q.size());
        ASSERT_EQ(n, Item::counters.alive());
    }

    EXPECT_EQ(0, Item::counters.alive());
}

TEST(queue, copy_construct) {
    Item::counters.reset();

    cls09::queue<Item> q;
    // наполняем данными
    const size_t n = 5;
    for (size_t i = 0; i != n; ++i) {
        q.push(Item(i));
    }
    ASSERT_EQ(n, q.size());
    ASSERT_EQ(n, Item::counters.alive());

    const int copy_ctors_before_action = Item::counters.copy_ctor;

    // копируем
    cls09::queue<Item> q2 = std::as_const(q);
    EXPECT_EQ(n, q.size());
    EXPECT_EQ(n, q2.size());
    EXPECT_EQ(n * 2, Item::counters.alive());

    EXPECT_EQ(copy_ctors_before_action + n, Item::counters.copy_ctor);  // не копируем сверх нужды

    // вычерпываем данные
    for (size_t i = 0; i != n; ++i) {
        ASSERT_EQ(n - i, q.size());
        EXPECT_EQ(i, q.front().value());
        q.pop();
    }

    for (size_t i = 0; i != n; ++i) {
        ASSERT_EQ(n - i, q2.size());
        EXPECT_EQ(i, q2.front().value());
        q2.pop();
    }
}

TEST(queue, copy_assign) {
    Item::counters.reset();

    cls09::queue<Item> q, q2;
    // наполняем данными
    const size_t n = 5, n2 = 7;
    for (size_t i = 0; i != n; ++i) {
        q.push(Item(i));
    }
    for (size_t i = 0; i != n2; ++i) {
        q2.push(Item(i + 100));
    }
    ASSERT_EQ(n, q.size());
    ASSERT_EQ(n2, q2.size());
    ASSERT_EQ(n + n2, Item::counters.alive());

    const int copy_before_action = Item::counters.all_copy();

    // копируем
    q2 = std::as_const(q);
    EXPECT_EQ(n, q.size());
    EXPECT_EQ(n, q2.size());
    EXPECT_EQ(n * 2, Item::counters.alive());

    EXPECT_EQ(copy_before_action + n, Item::counters.all_copy()); // не копируем сверх нужды

    // вычерпываем данные
    for (size_t i = 0; i != n; ++i) {
        ASSERT_EQ(n - i, q.size());
        EXPECT_EQ(i, q.front().value());
        q.pop();
    }

    for (size_t i = 0; i != n; ++i) {
        ASSERT_EQ(n - i, q2.size());
        EXPECT_EQ(i, q2.front().value());
        q2.pop();
    }

    EXPECT_EQ(0, Item::counters.alive());
}

TEST(queue, move_construct) {
    Item::counters.reset();

    cls09::queue<Item> q;
    // наполняем данными
    const size_t n = 5;
    for (size_t i = 0; i != n; ++i) {
        q.push(Item(i));
    }
    ASSERT_EQ(n, q.size());
    ASSERT_EQ(n, Item::counters.alive());

    const int copy_before_action = Item::counters.all_copy();
    const int move_before_action = Item::counters.all_move();

    // перемещаем
    cls09::queue<Item> q2 = std::move(q);
    EXPECT_EQ(0, q.size());
    EXPECT_EQ(n, q2.size());
    EXPECT_EQ(n, Item::counters.alive());

    // конструктор перемещения не нуждается в создании новых элементов
    EXPECT_EQ(copy_before_action, Item::counters.all_copy());
    EXPECT_EQ(move_before_action, Item::counters.all_move());

    // вычерпываем данные
    for (size_t i = 0; i != n; ++i) {
        ASSERT_EQ(n - i, q2.size());
        EXPECT_EQ(i, q2.front().value());
        q2.pop();
    }
}

TEST(queue, move_assign) {
    Item::counters.reset();

    cls09::queue<Item> q, q2;
    // наполняем данными
    const size_t n = 5, n2 = 7;
    for (size_t i = 0; i != n; ++i) {
        q.push(Item(i));
    }
    for (size_t i = 0; i != n2; ++i) {
        q2.push(Item(i + 100));
    }
    ASSERT_EQ(n, q.size());
    ASSERT_EQ(n2, q2.size());
    ASSERT_EQ(n + n2, Item::counters.alive());

    const int copy_before_action = Item::counters.all_copy();
    const int move_before_action = Item::counters.all_move();

    // перемещаем
    q2 = std::move(q);
    EXPECT_EQ(0, q.size());
    EXPECT_EQ(n, q2.size());
    EXPECT_EQ(n, Item::counters.alive());

    // оператор перемещения не нуждается в создании новых элементов
    EXPECT_EQ(copy_before_action, Item::counters.all_copy());
    EXPECT_EQ(move_before_action, Item::counters.all_move());

    // вычерпываем данные
    for (size_t i = 0; i != n; ++i) {
        ASSERT_EQ(n - i, q2.size());
        EXPECT_EQ(i, q2.front().value());
        q2.pop();
    }

    EXPECT_EQ(0, Item::counters.alive());
}

TEST(queue, copy_self_assign) {
    Item::counters.reset();

    cls09::queue<Item> q;
    // наполняем данными
    const size_t n = 5;
    for (size_t i = 0; i != n; ++i) {
        q.push(Item(i));
    }
    ASSERT_EQ(n, q.size());
    ASSERT_EQ(n, Item::counters.alive());

    const int copy_before_action = Item::counters.all_copy();
    const int move_before_action = Item::counters.all_move();

    // копируем
    q = std::as_const(q);
    EXPECT_EQ(n, q.size());
    EXPECT_EQ(n, Item::counters.alive());

    // никаких лишних действий не сделали!
    EXPECT_EQ(copy_before_action, Item::counters.all_copy());
    EXPECT_EQ(move_before_action, Item::counters.all_move());

    // вычерпываем данные
    for (size_t i = 0; i != n; ++i) {
        ASSERT_EQ(n - i, q.size());
        EXPECT_EQ(i, q.front().value());
        q.pop();
    }

    EXPECT_EQ(0, Item::counters.alive());
}

TEST(queue, exception_in_push) {
    Item::counters.reset();

    cls09::queue<Item> q;
    // наполняем данными
    const size_t n = 5;
    for (size_t i = 0; i != n; ++i) {
        q.push(Item(i));
    }
    ASSERT_EQ(n, q.size());
    ASSERT_EQ(n, Item::counters.alive());

    EXPECT_THROW(q.push(Item(n, 1)), std::runtime_error&);

    EXPECT_EQ(n, q.size());
    EXPECT_EQ(n, Item::counters.alive());
}

TEST(queue, exception_in_copy_ctor) {
    Item::counters.reset();

    cls09::queue<Item> q;
    // наполняем данными
    const size_t n = 5;

    const unsigned NO_FIRE = 0, FIRE_ON_NEXT_COPY = 2;
    for (size_t i = 0; i != n; ++i) {
        q.push(Item(i, i <= (n / 2) ? NO_FIRE : FIRE_ON_NEXT_COPY));
    }
    ASSERT_EQ(n, q.size());
    ASSERT_EQ(n, Item::counters.alive());

    EXPECT_THROW(cls09::queue<Item> q2 = q, std::runtime_error&);

    EXPECT_EQ(n, Item::counters.alive());  // ничего не утекло
}


TEST(queue, exception_in_copy_assign) {
    Item::counters.reset();

    cls09::queue<Item> q, q2;
    // наполняем данными
    const size_t n = 5, n2 = 7;
    const size_t m = 3;  // досчитаем до m и будем кидать исключение
    const int FOO = 100, BAR = 200;  // произвольное число, чтоб отличать элементы во второй очереди
    const unsigned NO_FIRE = 0, FIRE_ON_NEXT_COPY = 2;
    for (size_t i = 0; i != n; ++i) {
        q.push(Item(i + FOO, i < m ? NO_FIRE : FIRE_ON_NEXT_COPY));
    }
    for (size_t i = 0; i != n2; ++i) {
        q2.push(Item(i + BAR));
    }
    ASSERT_EQ(n, q.size());
    ASSERT_EQ(n2, q2.size());
    ASSERT_EQ(n + n2, Item::counters.alive());

    // копируем (безуспешно)
    EXPECT_THROW(q2 = std::as_const(q), std::runtime_error&);

    // источник не должен был измениться
    EXPECT_EQ(n, q.size());
    for (size_t i = 0; i != n; ++i) {
        ASSERT_EQ(n - i, q.size());
        EXPECT_EQ(i + FOO, q.front().value());
        q.pop();
    }

    // ЕСЛИ ВЫ СУМЕЛИ РЕАЛИЗОВАТЬ СТРОГУЮ БЕЗОПАСНОСТЬ ИСКЛЮЧЕНИЙ
    if (true) {
        // приёмник не должен был измениться
        EXPECT_EQ(n2, q2.size());
        for (size_t i = 0; i != n2; ++i) {
            ASSERT_EQ(n2 - i, q2.size());
            EXPECT_EQ(i + BAR, q2.front().value());
            q2.pop();
        }
    } else {
        // приёмник остался в каком-то целостном состоянии
        // и не содержит проблемных
        while (q2.size()) {
            int v = q2.front().value();
            EXPECT_TRUE(
                (FOO <= v && v <= FOO + (int)m)  // не все n, а только первые m
                ||
                (BAR <= v && v < BAR + (int)n2)
            ) << v;
            q2.pop();
        }
    }

    EXPECT_EQ(0, Item::counters.alive());  // ничего не утекло
}

TESTING_MAIN()
