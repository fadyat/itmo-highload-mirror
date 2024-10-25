#include <typeinfo>

#include "string_view.hpp"
#include "../simple_test.h"

#include <cassert>
#include <cstring>
#include <string>
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////
// россыпь тестов, проверяющих работоспособность

TEST(basic_test, VariousFunctions) {
    const char* const origData = "foobar";
    StringView sv = origData;

    EXPECT_EQ(origData, sv.data());
    EXPECT_EQ(6U, sv.size());
    EXPECT_EQ('b', sv[3]);

    StringView subSv = sv.substr(2);
    EXPECT_EQ(origData + 2, subSv.data());
    EXPECT_EQ(4U, subSv.size());
    EXPECT_EQ('o', subSv[0]);

    StringView subShortSv = sv.substr(3, 4);
    EXPECT_EQ('b', subShortSv[0]);
    EXPECT_EQ(1U, subShortSv.size());

    EXPECT_EQ(0U, sv.find('f'));
    EXPECT_EQ(1U, sv.find('o'));
    EXPECT_EQ(0U, sv.find("foo"));
    EXPECT_EQ(0U, sv.find("foo"));
    EXPECT_EQ(3U, sv.find("bar"));
    EXPECT_EQ(StringView::npos, sv.find('m'));
    EXPECT_EQ(StringView::npos, sv.find("moo"));

    EXPECT_TRUE(sv.startsWith('f'));
    EXPECT_TRUE(sv.startsWith("foo"));
    EXPECT_FALSE(sv.startsWith("Foo"));
    EXPECT_FALSE(sv.startsWith("ofo"));

    EXPECT_TRUE(sv.endsWith('r'));
    EXPECT_TRUE(sv.endsWith("bar"));
    EXPECT_FALSE(sv.endsWith("abr"));

    EXPECT_EQ(1U, sv.findFirstOf('o'));
    EXPECT_EQ(sv.findFirstOf('o', 1), sv.findFirstOf('o'));
    EXPECT_EQ(2U, sv.findFirstOf('o', 2));
    EXPECT_EQ(StringView::npos, sv.findFirstOf('m'));
    EXPECT_EQ(StringView::npos, sv.findFirstOf('m', 4));
    EXPECT_EQ(0U, sv.findFirstOf("oof"));
    EXPECT_EQ(0U, sv.findFirstOf("fm"));

    EXPECT_EQ(3U, sv.findFirstNotOf("oof"));
    EXPECT_EQ(3U, sv.findFirstNotOf("fo"));
    EXPECT_EQ(1U, sv.findFirstNotOf('f'));
    EXPECT_EQ(0U, sv.findFirstNotOf('m'));
}

TEST(basic_test, CopiesAndTrims) {
    const char* const OrigData = "foobar";
    StringView sv = OrigData;
    StringView svCopy = sv;

    EXPECT_EQ(OrigData, sv.data());
    EXPECT_EQ(6U, sv.size());

    sv.removePrefix(0);
    EXPECT_EQ(OrigData, sv.data());
    EXPECT_EQ(6U, sv.size());

    sv.removePrefix(2);
    EXPECT_EQ(OrigData + 2, sv.data());
    EXPECT_EQ(4U, sv.size());
    EXPECT_EQ('o', sv[0]);
    EXPECT_EQ('b', sv[1]);

    EXPECT_EQ(OrigData, svCopy.data());
    EXPECT_EQ(6U, svCopy.size());

    {
        StringView svCopyCopy = svCopy;
        EXPECT_EQ(OrigData, svCopyCopy.data());
        EXPECT_EQ(6U, svCopyCopy.size());

        svCopyCopy = sv;
        EXPECT_EQ(OrigData + 2, svCopyCopy.data());
        EXPECT_EQ(4U, svCopyCopy.size());
    }

    EXPECT_EQ(OrigData, svCopy.data());
    EXPECT_EQ(6U, svCopy.size());
}

TEST(basic_test, EndsWith) {
    StringView sv = "foobar";
    EXPECT_FALSE(sv.endsWith("foobarbaz"));
}

TEST(basic_test, SubString) {
    StringView sv = "foobar";
    StringView empty = sv.substr(0, 0);
    EXPECT_EQ(0U, empty.size());

    StringView whole = sv.substr(0, 100500);
    EXPECT_EQ(sv.size(), whole.size());
    EXPECT_STREQ(sv.data(), whole.data());
}

TEST(basic_test, NonCStringView) {
    std::string s("foobar\0baz", 10);
    StringView sv = s;

    EXPECT_EQ(10U, s.size());
    EXPECT_EQ(s.c_str(), sv.data());
    EXPECT_EQ(10U, sv.size());

    EXPECT_EQ(9U, sv.findFirstOf('z'));
    EXPECT_EQ(9U, sv.findFirstOf('z', 6));
    EXPECT_EQ(9U, sv.findFirstOf("mz"));
}

TEST(basic_test, FindFirstOf) {
    std::string s("foobar\0baz", 10);
    StringView sv = s;
    EXPECT_TRUE(sv.startsWith(sv));
    EXPECT_EQ(0U, sv.find(sv));
    EXPECT_EQ(7U, sv.find("baz"));

    char symbols[] = {'Z', '\0', 'z'};
    StringView searched_symbols(symbols, 3);
    EXPECT_EQ(6U, sv.findFirstOf(searched_symbols));

    s = "foobaz";
    sv = s;
    EXPECT_EQ(5U, sv.findFirstOf(searched_symbols));

    s = std::string("foobaz\0baz", 10);
    sv = s;
    EXPECT_EQ(5U, sv.findFirstOf(searched_symbols));
}

////////////////////////////////////////////////////////////////////////////////
// исчерпывающие тесты (пригодятся для отладки)

////////////////////////////////////////////////////////////////////////////////

TEST(access_functions, check_types) {
    const StringView sv = "foobar";

    EXPECT_EQ(typeid(const char*), typeid(sv.data()));
    EXPECT_EQ(typeid(size_t), typeid(sv.size()));
    EXPECT_EQ(typeid(const char&), typeid(sv[0]));  // не значение, а ссылка!
}

////////////////////////////////////////////////////////////////////////////////

TEST(construct, c_string) {
    const char* orig_data = "foobar";
    const size_t orig_size = strlen(orig_data);

    const StringView sv = orig_data;

    EXPECT_EQ(orig_data, sv.data());
    EXPECT_EQ(orig_size, sv.size());
    for (size_t i = 0; i != orig_size; ++i) {
        EXPECT_EQ(&orig_data[i], &sv[i]) << i;  // совпадают адреса!
    }
}

TEST(construct, std_string) {
    const std::string orig_data = "foobar";
    const size_t orig_size = std::size(orig_data);

    const StringView sv = orig_data;
    EXPECT_EQ(orig_data.c_str(), sv.data());
    EXPECT_EQ(orig_size, sv.size());
    for (size_t i = 0; i != orig_size; ++i) {
        EXPECT_EQ(&orig_data[i], &sv[i]) << i;  // совпадают адреса!
    }
}

TEST(construct, char_array) {
    const char* orig_data = "foobarbuz";
    const size_t orig_size = 6;

    const StringView sv{orig_data, orig_size};
    EXPECT_EQ(orig_data, sv.data());
    EXPECT_EQ(orig_size, sv.size());
    for (size_t i = 0; i != orig_size; ++i) {
        EXPECT_EQ(&orig_data[i], &sv[i]) << i;  // совпадают адреса!
    }
}

TEST(construct, char_array_with_zero_inside) {
    const char* orig_data = "foo\0barbuz";
    const size_t orig_size = 6;

    const StringView sv{orig_data, orig_size};
    EXPECT_EQ(orig_data, sv.data());
    EXPECT_EQ(orig_size, sv.size());
    for (size_t i = 0; i != orig_size; ++i) {
        EXPECT_EQ(&orig_data[i], &sv[i]) << i;  // совпадают адреса!
    }
}

TEST(construct, empty_c_string) {
    const StringView sv = "";
    EXPECT_EQ(0U, sv.size());
    EXPECT_EQ('\0', sv[0]);
}

TEST(construct, empty_std_string) {
    const std::string orig_data;
    const StringView sv = orig_data;
    EXPECT_EQ(0U, sv.size());
    EXPECT_EQ('\0', sv[0]);
}

TEST(construct, empty_char_array) {
    const StringView sv{"foobar", 0};
    EXPECT_EQ(0U, sv.size());
}

TEST(construct, copy) {
    const StringView orig_data = "foobar";
    ASSERT_EQ(6U, orig_data.size());
    ASSERT_STREQ("foobar", orig_data.data());

    const StringView sv = orig_data;
    EXPECT_EQ(orig_data.data(), sv.data());
    EXPECT_EQ(orig_data.size(), sv.size());
    for (size_t i = 0; i != orig_data.size(); ++i) {
        EXPECT_EQ(&orig_data[i], &sv[i]) << i;  // совпадают адреса!
    }
}
////////////////////////////////////////////////////////////////////////////////

TEST(npos, check_type_and_value) {
    EXPECT_EQ(typeid(size_t), typeid(StringView::npos));
    EXPECT_EQ(1U, -StringView::npos);
}

////////////////////////////////////////////////////////////////////////////////

TEST(substr, check_types) {
    const StringView sv = "foobar";
    EXPECT_EQ(typeid(StringView), typeid(sv.substr(0)));
    EXPECT_EQ(typeid(StringView), typeid(sv.substr(0, 0)));
}

TEST(subsr1, normal_range) {
    const size_t size = 3;
    const StringView orig_data = "foo";
    ASSERT_EQ(size, orig_data.size());

    for (size_t i = 0; i <= size; ++i) {
        const StringView sv = orig_data.substr(i);
        EXPECT_EQ(orig_data.data() + i, sv.data()) << i;
        EXPECT_EQ(size - i, sv.size()) << i;
    }
}

TEST(substr1, do_not_use_strlen) {
    const size_t size = 3;
    const char buf[size] = {};

    const size_t index = 1;
    const StringView sv = StringView(buf, size).substr(index);
    EXPECT_EQ(buf + index, sv.data());
    EXPECT_EQ(size - index, sv.size());
}

TEST(subsr1, index_exceeds_size) {
    const size_t size = 3;
    const StringView orig_data = "foo";
    ASSERT_EQ(size, orig_data.size());

    for (size_t i : {size, size + 1, size + 100, StringView::npos - 1, StringView::npos}) {
        const StringView sv = orig_data.substr(i);
        EXPECT_EQ(orig_data.data() + size, sv.data()) << i;
        EXPECT_EQ(0U, sv.size()) << i;
    }
}

TEST(substr2, normal_range) {
    const size_t size = 3;
    const StringView orig_data = "foo";
    ASSERT_EQ(size, orig_data.size());

    for (size_t i = 0; i <= size; ++i) {
        for (size_t j = i; j <= size; ++j) {
            const StringView sv = orig_data.substr(i, j);
            EXPECT_EQ(orig_data.data() + i, sv.data()) << i << "," << j;
            EXPECT_EQ(j - i, sv.size()) << i << "," << j;
        }
    }
}

TEST(substr2, do_not_use_strlen) {
    const size_t size = 5;
    const char buf[size] = {};

    const size_t index = 1;
    const size_t end_index = 4;
    const StringView sv = StringView(buf, size).substr(index, end_index);
    EXPECT_EQ(buf + index, sv.data());
    EXPECT_EQ(end_index - index, sv.size());
}

TEST(substr2, end_exceeds_size) {
    const size_t size = 3;
    const StringView orig_data = "foo";
    ASSERT_EQ(size, orig_data.size());

    for (size_t i = 0; i <= size; ++i) {
        for (size_t j : {size + 1, size + 100, StringView::npos - 1, StringView::npos}) {
            const StringView sv = orig_data.substr(i, j);
            EXPECT_EQ(orig_data.data() + i, sv.data()) << i << "," << j;
            EXPECT_EQ(size - i, sv.size()) << i << "," << j;
        }
    }
}

TEST(substr2, end_precedes_index) {
    const size_t size = 3;
    const StringView orig_data = "foo";
    ASSERT_EQ(size, orig_data.size());

    for (size_t i = 1; i <= size; ++i) {
        for (size_t j = 0; j <= i; ++j) {
            const StringView sv = orig_data.substr(i, j);
            EXPECT_EQ(orig_data.data() + i, sv.data()) << i << "," << j;
            EXPECT_EQ(0U, sv.size()) << i << "," << j;
        }
    }
}

TEST(substr2, index_exceeds_size) {
    const size_t size = 3;
    const StringView orig_data = "foo";
    ASSERT_EQ(size, orig_data.size());

    for (size_t i : {size + 1, size + 100, StringView::npos - 1, StringView::npos}) {
        for (size_t j : {i - 1, i, i + 1, StringView::npos - 1, StringView::npos}) {
            const StringView sv = orig_data.substr(i, j);
            EXPECT_EQ(orig_data.data() + size, sv.data()) << i << "," << j;
            EXPECT_EQ(0U, sv.size()) << i << "," << j;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

TEST(find, check_types) {
    const StringView sv = "";
    EXPECT_EQ(typeid(size_t), typeid(sv.find('c')));
    EXPECT_EQ(typeid(size_t), typeid(sv.find("")));
    EXPECT_EQ(typeid(size_t), typeid(sv.find(StringView(""))));
}

TEST(find, c_string) {
    const StringView sv = "foo bar foo bar";

    EXPECT_EQ(0U, sv.find('f'));
    EXPECT_EQ(3U, sv.find(' '));
    EXPECT_EQ(6U, sv.find('r'));

    EXPECT_EQ(0U, sv.find(""));
    EXPECT_EQ(0U, sv.find("foo"));
    EXPECT_EQ(4U, sv.find("bar"));
    EXPECT_EQ(1U, sv.find("oo bar foo bar"));
    EXPECT_EQ(0U, sv.find("foo bar foo bar"));

    EXPECT_EQ(0U, sv.find(StringView("")));
    EXPECT_EQ(0U, sv.find(StringView("foo")));
    EXPECT_EQ(4U, sv.find(StringView("bar")));
    EXPECT_EQ(1U, sv.find(StringView("oo bar foo bar")));
    EXPECT_EQ(0U, sv.find(StringView("foo bar foo bar")));
    EXPECT_EQ(0U, sv.find(sv));

    EXPECT_EQ(StringView::npos, sv.find('z'));
    EXPECT_EQ(StringView::npos, sv.find('\0'));

    EXPECT_EQ(StringView::npos, sv.find("z"));
    EXPECT_EQ(StringView::npos, sv.find("baz"));
    EXPECT_EQ(StringView::npos, sv.find("foo bar foo bar foo"));

    EXPECT_EQ(StringView::npos, sv.find(StringView("z")));
    EXPECT_EQ(StringView::npos, sv.find(StringView("baz")));
    EXPECT_EQ(StringView::npos, sv.find(StringView("foo bar foo bar foo")));
}

TEST(find, empty_case) {
    const StringView sv = "";

    EXPECT_EQ(0U, sv.find(""));
    EXPECT_EQ(0U, sv.find(StringView{"", 0}));

    EXPECT_EQ(StringView::npos, sv.find('\0'));
    EXPECT_EQ(StringView::npos, sv.find('a'));
    EXPECT_EQ(StringView::npos, sv.find("a"));
    EXPECT_EQ(StringView::npos, sv.find(StringView("a")));
    EXPECT_EQ(StringView::npos, sv.find(StringView("\0", 1)));
}

TEST(find, strings_with_zeros) {
    const StringView sv{"foo\0bar\0foo\0bar", 15};

    EXPECT_EQ(0U, sv.find('f'));
    EXPECT_EQ(3U, sv.find('\0'));
    EXPECT_EQ(6U, sv.find('r'));

    EXPECT_EQ(0U, sv.find("foo"));
    EXPECT_EQ(4U, sv.find("bar"));

    EXPECT_EQ(3U, sv.find(StringView{"\0", 1}));
    EXPECT_EQ(0U, sv.find(StringView{"foo\0bar", 7}));
    EXPECT_EQ(4U, sv.find(StringView{"bar\0foo", 7}));
    EXPECT_EQ(0U, sv.find(StringView{"foo\0bar\0foo\0bar", 15}));
    EXPECT_EQ(0U, sv.find(sv));

    EXPECT_EQ(StringView::npos, sv.find('z'));

    EXPECT_EQ(StringView::npos, sv.find("z"));
    EXPECT_EQ(StringView::npos, sv.find("baz"));

    EXPECT_EQ(StringView::npos, sv.find(StringView{"\0\0", 2}));
    EXPECT_EQ(StringView::npos, sv.find(StringView{"foo\0bar\0foo\0baz", 15}));
    EXPECT_EQ(StringView::npos, sv.find(StringView{"foo\0bar\0foo\0bar\0", 16}));
}

TEST(find, all_zeros) {
    const size_t size = 5;
    const char zeros1[size + 1] = {};
    const char zeros2[size + 1] = {};

    const StringView sv{zeros1, size};

    EXPECT_EQ(0U, sv.find('\0'));
    EXPECT_EQ(0U, sv.find(""));

    for (size_t i = 0; i != size; ++i) {
        EXPECT_EQ(0U, sv.find(StringView{zeros2, i}));
    }
    EXPECT_EQ(StringView::npos, sv.find(StringView{zeros1, size + 1}));  // тот же адрес
    EXPECT_EQ(StringView::npos, sv.find(StringView{zeros2, size + 1}));  // другой адрес
}

////////////////////////////////////////////////////////////////////////////////

TEST(startsWith, check_types) {
    const StringView sv = "";
    EXPECT_EQ(typeid(bool), typeid(sv.startsWith('a')));
    EXPECT_EQ(typeid(bool), typeid(sv.startsWith("")));
    EXPECT_EQ(typeid(bool), typeid(sv.startsWith(sv)));
}

TEST(startsWith, c_string) {
    const StringView sv = "foo bar foo bar";

    EXPECT_TRUE(sv.startsWith('f'));
    EXPECT_TRUE(sv.startsWith(""));
    EXPECT_TRUE(sv.startsWith("foo b"));
    EXPECT_TRUE(sv.startsWith("foo bar foo bar"));
    EXPECT_TRUE(sv.startsWith(StringView{"foo b"}));
    EXPECT_TRUE(sv.startsWith(sv));
    EXPECT_TRUE(sv.startsWith(sv.substr(0, 0)));
    EXPECT_TRUE(sv.startsWith(sv.substr(0, 3)));
    EXPECT_TRUE(sv.substr(0, 7).startsWith(sv.substr(8, 7)));

    EXPECT_FALSE(sv.startsWith('r'));
    EXPECT_FALSE(sv.startsWith("foobar"));
    EXPECT_FALSE(sv.startsWith("bar foo"));
    EXPECT_FALSE(sv.startsWith(StringView{"bar foo"}));
    EXPECT_FALSE(sv.startsWith(sv.substr(1)));
    EXPECT_FALSE(sv.substr(8).startsWith(sv));
}

TEST(startsWith, strings_with_zeros) {
    const StringView sv{"foo\0bar\0foo\0bar", 15};

    EXPECT_TRUE(sv.startsWith('f'));
    EXPECT_TRUE(sv.startsWith(""));
    EXPECT_TRUE(sv.startsWith("foo"));
    EXPECT_TRUE(sv.startsWith(StringView{"foo\0b", 5}));
    EXPECT_TRUE(sv.startsWith(sv));
    EXPECT_TRUE(sv.startsWith(sv.substr(0, 0)));
    EXPECT_TRUE(sv.startsWith(sv.substr(0, 5)));
    EXPECT_TRUE(sv.substr(0, 7).startsWith(sv.substr(8, 7)));

    EXPECT_FALSE(sv.startsWith('r'));
    EXPECT_FALSE(sv.startsWith("foobar"));
    EXPECT_FALSE(sv.startsWith("bar foo"));
    EXPECT_FALSE(sv.startsWith(StringView{"bar\0foo", 7}));
    EXPECT_FALSE(sv.startsWith(sv.substr(1)));
    EXPECT_FALSE(sv.substr(0, 7).startsWith(sv));
}

TEST(startsWith, empty_cases) {
    const StringView sv{""};

    EXPECT_TRUE(sv.startsWith(""));
    EXPECT_TRUE(sv.startsWith(StringView{"_", 0}));

    EXPECT_FALSE(sv.startsWith('_'));
    EXPECT_FALSE(sv.startsWith('\0'));
    EXPECT_FALSE(sv.startsWith("_"));
    EXPECT_FALSE(sv.startsWith(StringView{"\0", 1}));
}

////////////////////////////////////////////////////////////////////////////////

TEST(endsWith, check_types) {
    const StringView sv = "";
    EXPECT_EQ(typeid(bool), typeid(sv.endsWith('a')));
    EXPECT_EQ(typeid(bool), typeid(sv.endsWith("")));
    EXPECT_EQ(typeid(bool), typeid(sv.endsWith(sv)));
}

TEST(endsWith, c_string) {
    const StringView sv = "foo bar foo bar";

    EXPECT_TRUE(sv.endsWith('r'));
    EXPECT_TRUE(sv.endsWith(""));
    EXPECT_TRUE(sv.endsWith("o bar"));
    EXPECT_TRUE(sv.endsWith("foo bar foo bar"));
    EXPECT_TRUE(sv.endsWith(StringView{"o bar"}));
    EXPECT_TRUE(sv.endsWith(sv));
    EXPECT_TRUE(sv.endsWith(sv.substr(15)));
    EXPECT_TRUE(sv.endsWith(sv.substr(12)));
    EXPECT_TRUE(sv.substr(0, 7).endsWith(sv.substr(8, 7)));

    EXPECT_FALSE(sv.endsWith('f'));
    EXPECT_FALSE(sv.endsWith("foobar"));
    EXPECT_FALSE(sv.endsWith("bar foo"));
    EXPECT_FALSE(sv.endsWith(StringView{"bar foo"}));
    EXPECT_FALSE(sv.endsWith(sv.substr(0, 14)));
    EXPECT_FALSE(sv.substr(8).endsWith(sv));
}

TEST(endsWith, strings_with_zeros) {
    const StringView sv{"foo\0bar\0foo\0bar", 15};

    EXPECT_TRUE(sv.endsWith('r'));
    EXPECT_TRUE(sv.endsWith(""));
    EXPECT_TRUE(sv.endsWith("bar"));
    EXPECT_TRUE(sv.endsWith(StringView{"o\0bar", 5}));
    EXPECT_TRUE(sv.endsWith(sv));
    EXPECT_TRUE(sv.endsWith(sv.substr(0, 0)));
    EXPECT_TRUE(sv.endsWith(sv.substr(0, 7)));
    EXPECT_TRUE(sv.substr(0, 7).endsWith(sv.substr(8, 7)));

    EXPECT_FALSE(sv.endsWith('f'));
    EXPECT_FALSE(sv.endsWith("foobar"));
    EXPECT_FALSE(sv.endsWith("bar foo"));
    EXPECT_FALSE(sv.endsWith(StringView{"bar\0foo", 7}));
    EXPECT_FALSE(sv.endsWith(sv.substr(0, 14)));
    EXPECT_FALSE(sv.substr(8).endsWith(sv));
}

TEST(endsWith, empty_cases) {
    const StringView sv{""};

    EXPECT_TRUE(sv.endsWith(""));
    EXPECT_TRUE(sv.endsWith(StringView{"_", 0}));

    EXPECT_FALSE(sv.endsWith('_'));
    EXPECT_FALSE(sv.endsWith('\0'));
    EXPECT_FALSE(sv.endsWith("_"));
    EXPECT_FALSE(sv.endsWith(StringView{"\0", 1}));
}

////////////////////////////////////////////////////////////////////////////////

TEST(findFirstOf, check_types) {
    StringView sv = "";
    const size_t offset = 0;
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstOf('a')));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstOf('a', offset)));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstOf("")));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstOf("", offset)));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstOf(sv)));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstOf(sv, offset)));
}

TEST(findFirstOf, c_string) {
    StringView sv = "abcdefghaij";

    EXPECT_EQ(0U, sv.findFirstOf('a'));
    EXPECT_EQ(0U, sv.findFirstOf('a', 0));
    EXPECT_EQ(8U, sv.findFirstOf('a', 1));
    EXPECT_EQ(8U, sv.findFirstOf('a', 8));
    EXPECT_EQ(StringView::npos, sv.findFirstOf('a', 9));
    EXPECT_EQ(StringView::npos, sv.findFirstOf('a', 11));
    EXPECT_EQ(StringView::npos, sv.findFirstOf('a', 15));  // индекс может выйти за размер
    EXPECT_EQ(StringView::npos, sv.findFirstOf('a', StringView::npos));

    const char* str_set = "xxxyzabca";  // может содержать дубликаты
    EXPECT_EQ(0U, sv.findFirstOf(str_set));
    EXPECT_EQ(0U, sv.findFirstOf(str_set, 0));
    EXPECT_EQ(1U, sv.findFirstOf(str_set, 1));
    EXPECT_EQ(2U, sv.findFirstOf(str_set, 2));
    EXPECT_EQ(8U, sv.findFirstOf(str_set, 3));
    EXPECT_EQ(8U, sv.findFirstOf(str_set, 4));
    EXPECT_EQ(8U, sv.findFirstOf(str_set, 8));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(str_set, 9));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(str_set, 11));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(str_set, 15));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(str_set, StringView::npos));

    const StringView sv_set = str_set;
    EXPECT_EQ(0U, sv.findFirstOf(sv_set));
    EXPECT_EQ(0U, sv.findFirstOf(sv_set, 0));
    EXPECT_EQ(1U, sv.findFirstOf(sv_set, 1));
    EXPECT_EQ(2U, sv.findFirstOf(sv_set, 2));
    EXPECT_EQ(8U, sv.findFirstOf(sv_set, 3));
    EXPECT_EQ(8U, sv.findFirstOf(sv_set, 4));
    EXPECT_EQ(8U, sv.findFirstOf(sv_set, 8));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(sv_set, 9));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(sv_set, 11));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(sv_set, 15));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(sv_set, StringView::npos));

    EXPECT_EQ(StringView::npos, sv.findFirstOf('x'));
    EXPECT_EQ(StringView::npos, sv.findFirstOf("xyz"));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(StringView("xyz")));

    EXPECT_EQ(StringView::npos, sv.findFirstOf(""));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(StringView("")));
}

TEST(findFirstOf, strings_with_zeros) {
    StringView sv{"ab\0defghaij", 11};

    EXPECT_EQ(0U, sv.findFirstOf('a'));
    EXPECT_EQ(0U, sv.findFirstOf('a', 0));
    EXPECT_EQ(8U, sv.findFirstOf('a', 1));
    EXPECT_EQ(8U, sv.findFirstOf('a', 8));
    EXPECT_EQ(StringView::npos, sv.findFirstOf('a', 9));
    EXPECT_EQ(StringView::npos, sv.findFirstOf('a', 11));
    EXPECT_EQ(StringView::npos, sv.findFirstOf('a', 15));  // индекс может выйти за размер
    EXPECT_EQ(StringView::npos, sv.findFirstOf('a', StringView::npos));

    EXPECT_EQ(2U, sv.findFirstOf('\0'));
    EXPECT_EQ(StringView::npos, sv.findFirstOf('\0', 3));

    const StringView sv_set{"xxxyz\0abca", 10};  // может содержать дубликаты и нули}
    EXPECT_EQ(0U, sv.findFirstOf(sv_set));
    EXPECT_EQ(0U, sv.findFirstOf(sv_set, 0));
    EXPECT_EQ(1U, sv.findFirstOf(sv_set, 1));
    EXPECT_EQ(2U, sv.findFirstOf(sv_set, 2));
    EXPECT_EQ(8U, sv.findFirstOf(sv_set, 3));
    EXPECT_EQ(8U, sv.findFirstOf(sv_set, 4));
    EXPECT_EQ(8U, sv.findFirstOf(sv_set, 8));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(sv_set, 9));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(sv_set, 11));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(sv_set, 15));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(sv_set, StringView::npos));

    EXPECT_EQ(StringView::npos, sv.findFirstOf('x'));
    EXPECT_EQ(StringView::npos, sv.findFirstOf("xyz"));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(StringView("xyz")));

    EXPECT_EQ(StringView::npos, sv.findFirstOf(""));
    EXPECT_EQ(StringView::npos, sv.findFirstOf(StringView("")));
}

////////////////////////////////////////////////////////////////////////////////

TEST(findFirstNotOf, check_types) {
    StringView sv = "";
    const size_t offset = 0;
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstNotOf("")));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstNotOf("", offset)));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstNotOf('a')));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstNotOf('a', offset)));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstNotOf(sv)));
    EXPECT_EQ(typeid(size_t), typeid(sv.findFirstNotOf(sv, offset)));
}

TEST(findFirstNotOf, c_string) {
    StringView sv = "abcdefghaij";

    EXPECT_EQ(1U, sv.findFirstNotOf('a'));
    EXPECT_EQ(1U, sv.findFirstNotOf('a', 0));
    EXPECT_EQ(1U, sv.findFirstNotOf('a', 1));
    EXPECT_EQ(2U, sv.findFirstNotOf('a', 2));
    EXPECT_EQ(9U, sv.findFirstNotOf('a', 8));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf('a', 11));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf('a', 15));  // индекс может выйти за размер
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf('a', StringView::npos));

    const char* str_set = "xxxyzabca";  // может содержать дубликаты
    EXPECT_EQ(3U, sv.findFirstNotOf(str_set));
    EXPECT_EQ(3U, sv.findFirstNotOf(str_set, 0));
    EXPECT_EQ(3U, sv.findFirstNotOf(str_set, 1));
    EXPECT_EQ(3U, sv.findFirstNotOf(str_set, 2));
    EXPECT_EQ(3U, sv.findFirstNotOf(str_set, 3));
    EXPECT_EQ(4U, sv.findFirstNotOf(str_set, 4));
    EXPECT_EQ(9U, sv.findFirstNotOf(str_set, 8));
    EXPECT_EQ(9U, sv.findFirstNotOf(str_set, 9));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf(str_set, 11));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf(str_set, 15));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf(str_set, StringView::npos));

    const StringView sv_set = str_set;
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set));
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set, 0));
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set, 1));
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set, 2));
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set, 3));
    EXPECT_EQ(4U, sv.findFirstNotOf(sv_set, 4));
    EXPECT_EQ(9U, sv.findFirstNotOf(sv_set, 8));
    EXPECT_EQ(9U, sv.findFirstNotOf(sv_set, 9));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf(sv_set, 11));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf(sv_set, 15));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf(sv_set, StringView::npos));

    EXPECT_EQ(0U, sv.findFirstNotOf(""));
    EXPECT_EQ(0U, sv.findFirstNotOf(StringView("")));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf("", 11));
}

TEST(findFirstNotOf, strings_with_zeros) {
    StringView sv{"ab\0defghaij", 11};

    EXPECT_EQ(1U, sv.findFirstNotOf('a'));
    EXPECT_EQ(1U, sv.findFirstNotOf('a', 0));
    EXPECT_EQ(1U, sv.findFirstNotOf('a', 1));
    EXPECT_EQ(2U, sv.findFirstNotOf('a', 2));
    EXPECT_EQ(9U, sv.findFirstNotOf('a', 8));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf('a', 11));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf('a', 15));  // индекс может выйти за размер
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf('a', StringView::npos));

    EXPECT_EQ(0U, sv.findFirstNotOf('\0'));
    EXPECT_EQ(3U, sv.findFirstNotOf('\0', 2));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf('\0', 11));

    const StringView sv_set{"xxxyz\0abca", 10};  // может содержать дубликаты и нули}
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set));
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set, 0));
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set, 1));
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set, 2));
    EXPECT_EQ(3U, sv.findFirstNotOf(sv_set, 3));
    EXPECT_EQ(4U, sv.findFirstNotOf(sv_set, 4));
    EXPECT_EQ(9U, sv.findFirstNotOf(sv_set, 8));
    EXPECT_EQ(9U, sv.findFirstNotOf(sv_set, 9));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf(sv_set, 11));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf(sv_set, 15));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf(sv_set, StringView::npos));

    EXPECT_EQ(0U, sv.findFirstNotOf(""));
    EXPECT_EQ(0U, sv.findFirstNotOf(StringView("")));
    EXPECT_EQ(StringView::npos, sv.findFirstNotOf("", 11));
}

////////////////////////////////////////////////////////////////////////////////

TESTING_MAIN()
