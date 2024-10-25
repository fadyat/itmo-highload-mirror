#include "lazy_string.hpp"
#include "lazy_string.hpp"

#include <cassert>
#include <cstring>
#include <string>
#include <type_traits>

#include "../simple_test.h"


TEST(operator, read_access) {
    // реквизит: две строки с общим содержимым
    LazyString s1 = "Hello world";
    LazyString s2 = s1;
    ASSERT_EQ(2U, s1.useCount());
    ASSERT_EQ((const void*)s2.cStr(), (const void*)s1.cStr());

    // проверяем, что доступ к элементу не изменяет общность
    EXPECT_EQ('e', s1[1]);
    EXPECT_EQ(2U, s1.useCount());
    EXPECT_EQ((const void*)s2.cStr(), (const void*)s1.cStr());

    EXPECT_EQ(s1[1], 'e');
    EXPECT_EQ(2U, s1.useCount());
    EXPECT_EQ((const void*)s2.cStr(), (const void*)s1.cStr());

    char e = s1[1];
    EXPECT_EQ('e', e);
    EXPECT_EQ(2U, s1.useCount());
    EXPECT_EQ(s2.cStr(), s1.cStr());

    // итого
    EXPECT_STREQ("Hello world", s1.cStr());
    EXPECT_STREQ("Hello world", s2.cStr());
}

TEST(operator, write_access) {
    // реквизит: две строки с общим содержимым
    LazyString s1 = "Hello world";
    LazyString s2 = s1;
    ASSERT_EQ(2U, s1.useCount());
    ASSERT_EQ((const void*)s2.cStr(), (const void*)s1.cStr());
    const void* shared_data = s1.cStr();

    // доступ на запись должен клонировать строку
    s1[1] = 'o';
    EXPECT_EQ(1U, s1.useCount());
    EXPECT_EQ(shared_data, (const void*)s2.cStr());
    EXPECT_NE(shared_data, (const void*)s1.cStr());
    EXPECT_EQ('o', s1.cStr()[1]);

    // последующий доступ не приводит к повторному клонированию
    const char* s1_data = s1.cStr();
    s1[2] = 'w';
    EXPECT_EQ(1U, s1.useCount());
    EXPECT_EQ(s1_data, (const void*)s1.cStr());
    EXPECT_EQ('w', s1.cStr()[2]);
    // сделаем слово осмысленным: Howdy = Как-дела?
    s1[3] = 'd';
    s1[4] = 'y';

    // итого
    EXPECT_STREQ("Howdy world", s1.cStr());
    EXPECT_STREQ("Hello world", s2.cStr());
}

TEST(operator, autoincrement) {
    // реквизит: две строки с общим содержимым
    LazyString s1 = "Hello world";
    LazyString s2 = s1;
    ASSERT_EQ(2U, s1.useCount());
    ASSERT_EQ((const void*)s2.cStr(), (const void*)s1.cStr());
    const void* shared_data = s1.cStr();

    // доступ на запись должен клонировать строку
    char c1 = ++s1[1];
    EXPECT_EQ(1U, s1.useCount());
    EXPECT_EQ(shared_data, (const void*)s2.cStr());
    EXPECT_NE(shared_data, (const void*)s1.cStr());
    EXPECT_EQ('f', c1);
    EXPECT_EQ('f', s1.cStr()[1]);

    // последующий доступ не приводит к повторному клонированию
    const char* s1_data = s1.cStr();
    char c2 = s1[2]++;
    EXPECT_EQ(1U, s1.useCount());
    EXPECT_EQ(s1_data, (const void*)s1.cStr());
    EXPECT_EQ('l', c2);
    EXPECT_EQ('m', s1.cStr()[2]);

    // двойной преинкремент
    char c3 = ++ ++s1[3];
    EXPECT_EQ(1U, s1.useCount());
    EXPECT_EQ(s1_data, (const void*)s1.cStr());
    EXPECT_EQ('n', c3);
    EXPECT_EQ('n', s1.cStr()[3]);

    // итого
    EXPECT_STREQ("Hfmno world", s1.cStr());
    EXPECT_STREQ("Hello world", s2.cStr());
}

TESTING_MAIN()
