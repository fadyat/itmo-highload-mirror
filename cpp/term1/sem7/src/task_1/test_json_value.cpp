#include "json_value.hpp"
#include "json_value.hpp"

#include "../simple_test.h"
#include <type_traits>


using cls07::JsonValue;

TEST(check_types, various) {
    JsonValue j;
    const JsonValue& cj = j;
    // типы StringType и ListType должны быть определены
    EXPECT_FALSE(std::is_reference_v<JsonValue::StringType>);
    EXPECT_EQ(typeid(JsonValue::StringType&), typeid(j.GetString()));
    EXPECT_EQ(typeid(const JsonValue::StringType&), typeid(cj.GetString()));

    EXPECT_FALSE(std::is_reference_v<JsonValue::ListType>);
    EXPECT_EQ(typeid(JsonValue::ListType&), typeid(j.GetArray()));
    EXPECT_EQ(typeid(const JsonValue::ListType&), typeid(cj.GetArray()));
}


TEST(construct, empty) {
    const JsonValue j;
    EXPECT_TRUE(j.IsNull());
    EXPECT_FALSE(j.IsBool());
    EXPECT_FALSE(j.IsInt());
    EXPECT_FALSE(j.IsString());
    EXPECT_FALSE(j.IsArray());
}

TEST(construct, bool) {
    const JsonValue j = true;
    EXPECT_FALSE(j.IsNull());
    EXPECT_TRUE(j.IsBool());
    EXPECT_FALSE(j.IsInt());
    EXPECT_FALSE(j.IsString());
    EXPECT_FALSE(j.IsArray());

    EXPECT_EQ(true, j.GetBool());
}

TEST(construct, int) {
    const JsonValue j = 0;  // заодно проверим всякие неоднозначности
    EXPECT_FALSE(j.IsNull());
    EXPECT_FALSE(j.IsBool());
    EXPECT_TRUE(j.IsInt());
    EXPECT_FALSE(j.IsString());
    EXPECT_FALSE(j.IsArray());

    EXPECT_EQ(0, j.GetInt());
}

TEST(construct, char) {
    const JsonValue j = 'a';
    EXPECT_FALSE(j.IsNull());
    EXPECT_FALSE(j.IsBool());
    EXPECT_FALSE(j.IsInt());
    EXPECT_TRUE(j.IsString());
    EXPECT_FALSE(j.IsArray());

    EXPECT_EQ("a", j.GetString());
}

TEST(construct, c_string) {
    const JsonValue j = "abc";
    EXPECT_FALSE(j.IsNull());
    EXPECT_FALSE(j.IsBool());
    EXPECT_FALSE(j.IsInt());
    EXPECT_TRUE(j.IsString());
    EXPECT_FALSE(j.IsArray());

    EXPECT_EQ("abc", j.GetString());
}

TEST(construct, std_string) {
    const JsonValue j = std::string("abc");
    EXPECT_FALSE(j.IsNull());
    EXPECT_FALSE(j.IsBool());
    EXPECT_FALSE(j.IsInt());
    EXPECT_TRUE(j.IsString());
    EXPECT_FALSE(j.IsArray());

    EXPECT_EQ("abc", j.GetString());
}

TEST(assign, various_types) {
    JsonValue j;
    const JsonValue& cj = j;
    ASSERT_TRUE(j.IsNull());

    j = false;
    EXPECT_FALSE(cj.IsNull());
    EXPECT_TRUE(cj.IsBool());
    EXPECT_EQ(false, cj.GetBool());
    j = true;
    EXPECT_TRUE(cj.IsBool());
    EXPECT_EQ(true, cj.GetBool());

    j = 0;
    EXPECT_FALSE(cj.IsBool());
    EXPECT_TRUE(cj.IsInt());
    EXPECT_EQ(0, cj.GetInt());
    j = 1;
    EXPECT_TRUE(cj.IsInt());
    EXPECT_EQ(1, cj.GetInt());

    j = 'a';
    EXPECT_FALSE(cj.IsInt());
    EXPECT_TRUE(cj.IsString());
    EXPECT_EQ("a", cj.GetString());
    j = 'b';
    EXPECT_TRUE(cj.IsString());
    EXPECT_EQ("b", cj.GetString());

    j = 123;
    EXPECT_FALSE(cj.IsString());
    EXPECT_TRUE(cj.IsInt());

    j = "abc";
    EXPECT_TRUE(cj.IsString());
    EXPECT_EQ("abc", cj.GetString());

    j = true;
    EXPECT_FALSE(cj.IsString());
    EXPECT_TRUE(cj.IsBool());

    j = std::string{"def"};
    EXPECT_TRUE(cj.IsString());
    EXPECT_EQ("def", cj.GetString());

    j.SetNull();
    EXPECT_FALSE(cj.IsString());
    EXPECT_TRUE(cj.IsNull());
}

TEST(assign, json) {
    JsonValue j1{123};
    const JsonValue& cj1 = j1;
    JsonValue rj1 = j1;
    ASSERT_TRUE(cj1.IsInt());
    ASSERT_EQ(123, cj1.GetInt());

    JsonValue j2{"abc"};
    const JsonValue& cj2 = j2;
    ASSERT_TRUE(cj2.IsString());
    ASSERT_EQ("abc", cj2.GetString());

    // самоприсваивание не изменяет объект
    j1 = rj1;
    EXPECT_TRUE(cj1.IsInt());
    EXPECT_EQ(123, cj1.GetInt());

    // копирующее присваивание не изменяет источник
    j1 = j2;
    EXPECT_TRUE(cj1.IsString());
    EXPECT_EQ("abc", cj1.GetString());
    EXPECT_TRUE(cj2.IsString());
    EXPECT_EQ("abc", cj2.GetString());

    // одинаковые объекты не разделяют владение содержимым
    j2 = JsonValue{true};
    EXPECT_TRUE(cj1.IsString());
    EXPECT_EQ("abc", cj1.GetString());
    EXPECT_TRUE(cj2.IsBool());
    EXPECT_EQ(true, cj2.GetBool());
}

TEST(nonconst_getters, various_types) {
    JsonValue j1;
    const JsonValue& cj1 = j1;
    ASSERT_TRUE(cj1.IsNull());

    // неконстантный доступ изменяет тип!
    {
        auto& v = j1.GetBool();
        EXPECT_TRUE(cj1.IsBool());
        EXPECT_EQ(false, v);
        v = true;
        EXPECT_EQ(true, cj1.GetBool());
    }
    {
        auto& v = j1.GetInt();
        EXPECT_TRUE(cj1.IsInt());
        EXPECT_EQ(0, v);
        v = 123;
        EXPECT_EQ(123, cj1.GetInt());
    }
    {
        auto& v = j1.GetString();
        EXPECT_TRUE(cj1.IsString());
        EXPECT_EQ("", v);
        v = "abc";
        EXPECT_EQ("abc", cj1.GetString());
    }
}

TEST(array, simple_contents) {
    JsonValue j1 = "abc";
    const JsonValue& cj1 = j1;
    ASSERT_TRUE(cj1.IsString());

    // добавление элемента изменяет тип!
    j1.Append(true);
    EXPECT_TRUE(cj1.IsArray());
    j1.Append(123);
    j1.Append('a');
    j1.Append("xyz");
    j1.Append(JsonValue{});

    EXPECT_TRUE(cj1[0].IsBool());
    EXPECT_TRUE(cj1[1].IsInt());
    EXPECT_TRUE(cj1[2].IsString());
    EXPECT_TRUE(cj1[3].IsString());
    EXPECT_TRUE(cj1[4].IsNull());
}

TEST(array, self_append) {
    JsonValue j1;
    const JsonValue& cj1 = j1;

    j1.Append(0);
    j1.Append(10);
    j1.Append(20);
    j1.Append(30);
    ASSERT_TRUE(cj1.IsArray());
    {
        const auto& v = cj1.GetArray();
        ASSERT_EQ(4U, std::size(v));
        ASSERT_TRUE(v[0].IsInt()); ASSERT_EQ(0, v[0].GetInt());
        ASSERT_TRUE(v[1].IsInt()); ASSERT_EQ(10, v[1].GetInt());
        ASSERT_TRUE(v[2].IsInt()); ASSERT_EQ(20, v[2].GetInt());
        ASSERT_TRUE(v[3].IsInt()); ASSERT_EQ(30, v[3].GetInt());
    }

    // тестируем!
    j1.Append(j1);
    ASSERT_TRUE(cj1.IsArray());
    {
        const auto& v = cj1.GetArray();
        ASSERT_EQ(5U, std::size(v));
        EXPECT_TRUE(v[0].IsInt()); EXPECT_EQ(0, v[0].GetInt());
        EXPECT_TRUE(v[1].IsInt()); EXPECT_EQ(10, v[1].GetInt());
        EXPECT_TRUE(v[2].IsInt()); EXPECT_EQ(20, v[2].GetInt());
        EXPECT_TRUE(v[3].IsInt()); EXPECT_EQ(30, v[3].GetInt());

        ASSERT_TRUE(v[4].IsArray());
        const auto& w = v[4].GetArray();
        EXPECT_TRUE(w[0].IsInt()); EXPECT_EQ(0, w[0].GetInt());
        EXPECT_TRUE(w[1].IsInt()); EXPECT_EQ(10, w[1].GetInt());
        EXPECT_TRUE(w[2].IsInt()); EXPECT_EQ(20, w[2].GetInt());
        EXPECT_TRUE(w[3].IsInt()); EXPECT_EQ(30, w[3].GetInt());
    }
}

TEST(array, assign_scalar) {
    JsonValue j1;
    const JsonValue& cj1 = j1;

    j1.Append(0);
    j1.Append(10);
    j1.Append(20);
    j1.Append(30);
    ASSERT_TRUE(cj1.IsArray());

    // проверяем!
    j1 = 123;
    EXPECT_TRUE(cj1.IsInt());
    EXPECT_EQ(123, cj1.GetInt());
}

TEST(array, nonconst_access_as_scalar) {
    JsonValue j1;
    const JsonValue& cj1 = j1;

    j1.Append(0);
    j1.Append(10);
    j1.Append(20);
    j1.Append(30);
    ASSERT_TRUE(cj1.IsArray());

    // проверяем!
    int& v = j1.GetInt();
    EXPECT_TRUE(cj1.IsInt());
    EXPECT_EQ(0, v);
    EXPECT_EQ(0, cj1.GetInt());
}

TEST(array, nonconst_access_to_items) {
    JsonValue j1;
    const JsonValue& cj1 = j1;

    j1.Append(0);
    j1.Append(10);
    j1.Append(20);
    j1.Append(30);
    ASSERT_TRUE(cj1.IsArray());
    ASSERT_TRUE(cj1[1].IsInt());

    j1[1] = "abc";
    EXPECT_TRUE(cj1[1].IsString());

    auto& v = j1[2].GetBool();
    EXPECT_TRUE(cj1[2].IsBool());
    EXPECT_EQ(false, v);
}

TEST(array, change_type) {
    JsonValue j1;

    j1.Append(42);
    j1.Append("foo");
    j1.Append("bar");
    
    // изменяет тип
    j1[1].Append(24);
    j1[1].Append(66);

    EXPECT_TRUE(j1[0].IsInt());
    EXPECT_TRUE(j1[1].IsArray());
    EXPECT_TRUE(j1[2].IsString());

    j1[1] = "a";
    EXPECT_TRUE(j1[0].IsInt());
    EXPECT_TRUE(j1[1].IsString());
    EXPECT_EQ(1UL, j1[1].GetString().size());
    EXPECT_TRUE(j1[2].IsString());
    EXPECT_EQ(3UL, j1[2].GetString().size());

    j1 = "bb";
    EXPECT_TRUE(j1.IsString());
    EXPECT_EQ(2UL, j1.GetString().size());
}

TESTING_MAIN()

