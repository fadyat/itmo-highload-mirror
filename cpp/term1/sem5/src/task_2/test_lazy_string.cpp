#include "lazy_string.hpp"
#include "lazy_string.hpp"

#include <cstring>
#include <stdexcept>
#include <string>

#include "../simple_test.h"

////////////////////////////////////////////////////////////////////////////////
// тесты основных функций

TEST(basic, empty_string) {
    LazyString s1;
    LazyString const &ref = s1;

    EXPECT_EQ(0U, s1.getSize());
    EXPECT_EQ(0U, ref.getSize());
    EXPECT_EQ('\0', s1.cStr()[0]);
    EXPECT_EQ('\0', ref.cStr()[0]);

    EXPECT_EQ(1U, s1.useCount());
}

TEST(basic, shared_copy) {
    LazyString s1 = "Hello world";
    EXPECT_EQ(1U, s1.useCount());
    LazyString const s2 = s1;
    EXPECT_EQ(2U, s1.useCount());

    EXPECT_EQ(s1.getSize(), s2.getSize());
    EXPECT_EQ(s2.cStr(), s1.cStr());

    EXPECT_EQ(sizeof("Hello world") - 1, s1.getSize());
    EXPECT_STREQ(s1.cStr(), "Hello world");
}

TEST(basic, autoincrement) {
    LazyString s1;
    for (size_t i = 0; i < 10; ++i) {
        s1 += "A";
    }

    EXPECT_EQ(10U, s1.getSize());
    EXPECT_STREQ(s1.cStr(), "AAAAAAAAAA");
}

TEST(basic, concat) {
    LazyString s = "Hello";
    LazyString s_concat = s + " with suffix";
    EXPECT_EQ(s_concat.cStr(), std::strstr(s_concat.cStr(), s.cStr()));

    EXPECT_EQ(strlen(s_concat.cStr()), strlen(s.cStr()) + 12);
}

TEST(basic, at) {
    std::string str{"Hello, world!"};
    LazyString lazy_str{str.c_str()};

    for (size_t i = 0; i < str.size(); ++i) {
        EXPECT_EQ(lazy_str.at(i), str[i]);
    }
}

TEST(basic, exception) {
    LazyString s = "Hello";
    EXPECT_EQ('e', s.at(1));

    try {
        (void) s.at(10);
        FAIL() << "no exception";
    } catch (const std::out_of_range &e) {
    } catch (...) {
        FAIL() << "expected std::out_of_range exception!";
    }
}

////////////////////////////////////////////////////////////////////////////////
// исчерпывающие тесты

TEST(constructor, type_check) {
    EXPECT_TRUE((std::is_default_constructible_v<LazyString>));
    EXPECT_TRUE((std::is_constructible_v<LazyString, const char*>));
}

TEST(construct_and_share, empty) {
    const char* const text = "";
    const size_t size = strlen(text);

    LazyString s;
    EXPECT_EQ(1U, s.useCount());
    EXPECT_EQ(size, s.getSize());
    EXPECT_STREQ(text, s.cStr());

    LazyString s2 = s;
    // счётчик общий (оптимально было бы вообще не изменять - для семейства пустых строк)
    EXPECT_GE(2U, s.useCount());
    EXPECT_EQ(s.useCount(), s2.useCount());

    // исходная строка не изменилась
    EXPECT_EQ(size, s.getSize());
    EXPECT_STREQ(text, s.cStr());

    // новая такая же
    EXPECT_EQ(size, s2.getSize());
    EXPECT_STREQ(text, s2.cStr());
    EXPECT_EQ((const void*)s.cStr(), (const void*)s2.cStr())
        << "надо оптимизировать - данные должны быть общими";

    LazyString s3 = s2;
    // счётчик общий на все три
    EXPECT_GE(3U, s.useCount());
    EXPECT_EQ(s.useCount(), s2.useCount());
    EXPECT_EQ(s.useCount(), s3.useCount());

    // исходные строки не изменились
    EXPECT_EQ(size, s.getSize());
    EXPECT_STREQ(text, s.cStr());
    EXPECT_EQ(size, s2.getSize());
    EXPECT_STREQ(text, s2.cStr());

    // новая такая же
    EXPECT_EQ(size, s3.getSize());
    EXPECT_STREQ(text, s3.cStr());
    EXPECT_EQ((const void*)s.cStr(), (const void*)s3.cStr())
        << "надо оптимизировать - данные должны быть общими";
}

TEST(construct_and_share, valuable) {
    const char* const text = "hello";
    const size_t size = strlen(text);

    LazyString s = text;
    EXPECT_EQ(1U, s.useCount());
    EXPECT_EQ(size, s.getSize());
    EXPECT_STREQ(text, s.cStr());
    EXPECT_NE((const void*)text, (const void*)s.cStr());

    LazyString s2 = s;
    EXPECT_EQ(2U, s.useCount());
    EXPECT_EQ(2U, s2.useCount());

    // исходная строка не изменилась
    EXPECT_EQ(size, s.getSize());
    EXPECT_STREQ(text, s.cStr());

    // новая такая же
    EXPECT_EQ(size, s2.getSize());
    EXPECT_STREQ(text, s2.cStr());
    EXPECT_EQ((const void*)s.cStr(), (const void*)s2.cStr())
        << "надо оптимизировать - данные должны быть общими";

    LazyString s3 = s2;
    // счётчик общий на все три
    EXPECT_EQ(3U, s.useCount());
    EXPECT_EQ(3U, s2.useCount());
    EXPECT_EQ(3U, s3.useCount());

    // исходные строки не изменились
    EXPECT_EQ(size, s.getSize());
    EXPECT_STREQ(text, s.cStr());
    EXPECT_EQ(size, s2.getSize());
    EXPECT_STREQ(text, s2.cStr());

    // новая такая же
    EXPECT_EQ(size, s3.getSize());
    EXPECT_STREQ(text, s3.cStr());
    EXPECT_EQ((const void*)s.cStr(), (const void*)s3.cStr())
        << "надо оптимизировать - данные должны быть общими";
}

TEST(construct_share_and_destroy, empty) {
    LazyString* s1 = new LazyString();
    EXPECT_EQ(1U, s1->useCount());

    LazyString* s2 = new LazyString(*s1);
    EXPECT_GE(2U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s2->useCount());

    LazyString* s3 = new LazyString(*s2);
    EXPECT_GE(3U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s2->useCount());
    EXPECT_EQ(s1->useCount(), s3->useCount());

    LazyString* s4 = new LazyString(*s3);
    EXPECT_GE(4U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s2->useCount());
    EXPECT_EQ(s1->useCount(), s3->useCount());
    EXPECT_EQ(s1->useCount(), s4->useCount());

    delete s2;
    EXPECT_GE(3U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s3->useCount());
    EXPECT_EQ(s1->useCount(), s4->useCount());

    delete s4;
    EXPECT_GE(2U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s3->useCount());

    delete s3;
    EXPECT_GE(1U, s1->useCount());

    delete s1;
}

TEST(construct_share_and_destroy, valuable) {
    const char* text = "hello";
    LazyString* s1 = new LazyString(text);
    EXPECT_EQ(1U, s1->useCount());
    EXPECT_STREQ(text, s1->cStr());

    LazyString* s2 = new LazyString(*s1);
    EXPECT_EQ(2U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s2->useCount());
    EXPECT_STREQ(text, s1->cStr());
    EXPECT_STREQ(text, s2->cStr());

    LazyString* s3 = new LazyString(*s2);
    EXPECT_EQ(3U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s2->useCount());
    EXPECT_EQ(s1->useCount(), s3->useCount());
    EXPECT_STREQ(text, s1->cStr());
    EXPECT_STREQ(text, s2->cStr());
    EXPECT_STREQ(text, s3->cStr());

    LazyString* s4 = new LazyString(*s3);
    EXPECT_EQ(4U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s2->useCount());
    EXPECT_EQ(s1->useCount(), s3->useCount());
    EXPECT_EQ(s1->useCount(), s4->useCount());
    EXPECT_STREQ(text, s1->cStr());
    EXPECT_STREQ(text, s2->cStr());
    EXPECT_STREQ(text, s3->cStr());
    EXPECT_STREQ(text, s4->cStr());

    delete s2;
    EXPECT_EQ(3U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s3->useCount());
    EXPECT_EQ(s1->useCount(), s4->useCount());
    EXPECT_STREQ(text, s1->cStr());
    EXPECT_STREQ(text, s3->cStr());
    EXPECT_STREQ(text, s4->cStr());

    delete s4;
    EXPECT_EQ(2U, s1->useCount());
    EXPECT_EQ(s1->useCount(), s3->useCount());
    EXPECT_STREQ(text, s1->cStr());
    EXPECT_STREQ(text, s3->cStr());

    delete s3;
    EXPECT_EQ(1U, s1->useCount());
    EXPECT_STREQ(text, s1->cStr());

    delete s1;
}

TEST(autoincrement, empty) {
    LazyString s;
    EXPECT_STREQ("", s.cStr());
    EXPECT_EQ(1U, s.useCount());

    s += "";
    EXPECT_STREQ("", s.cStr());
    EXPECT_EQ(1U, s.useCount());

    s += "hello";
    EXPECT_STREQ("hello", s.cStr());
    EXPECT_EQ(1U, s.useCount());
}

TEST(autoincrement, valuable) {
    LazyString s("hello");
    EXPECT_STREQ("hello", s.cStr());
    EXPECT_EQ(1U, s.useCount());

    s += "";
    EXPECT_STREQ("hello", s.cStr());
    EXPECT_EQ(1U, s.useCount());

    s += "world";
    EXPECT_STREQ("helloworld", s.cStr());
    EXPECT_EQ(1U, s.useCount());
}

TEST(autoincrement, copy_on_write_empty) {
    LazyString e;
    LazyString e2 = e;
    LazyString s = e;
    ASSERT_GE(3U, e.useCount());

    s += "";
    ASSERT_GE(3U, e.useCount());  // возможна оптимизация! мы ничего не прибавили

    s += "A";
    ASSERT_STREQ("", e.cStr());
    ASSERT_STREQ("A", s.cStr());

    ASSERT_GE(2U, e.useCount());
    ASSERT_EQ(1U, s.useCount());  // отсоединились

    s += "B";
    ASSERT_STREQ("", e.cStr());
    ASSERT_STREQ("AB", s.cStr());

    ASSERT_GE(2U, e.useCount());
    ASSERT_EQ(1U, s.useCount());
}

TEST(autoincrement, copy_on_write_valuable) {
    LazyString e = "essential";
    LazyString e2 = e;
    LazyString s = e;
    ASSERT_EQ(3U, e.useCount());

    s += "";
    ASSERT_GE(3U, e.useCount());  // возможна оптимизация! мы ничего не прибавили

    s += "A";
    ASSERT_STREQ("essential", e.cStr());
    ASSERT_STREQ("essentialA", s.cStr());

    ASSERT_EQ(2U, e.useCount());
    ASSERT_EQ(1U, s.useCount());  // отсоединились

    s += "B";
    ASSERT_STREQ("essential", e.cStr());
    ASSERT_STREQ("essentialAB", s.cStr());

    ASSERT_EQ(2U, e.useCount());
    ASSERT_EQ(1U, s.useCount());
}

TEST(autoincrement, itself) {
    LazyString s = "Ab";

    s += s.cStr();
    EXPECT_EQ(1U, s.useCount());
    EXPECT_STREQ("AbAb", s.cStr());

    s += s.cStr();
    EXPECT_EQ(1U, s.useCount());
    EXPECT_STREQ("AbAbAbAb", s.cStr());

    s += s.cStr();
    EXPECT_EQ(1U, s.useCount());
    EXPECT_STREQ("AbAbAbAbAbAbAbAb", s.cStr());
}

////////////////////////////////////////////////////////////////////////////////

TESTING_MAIN()
