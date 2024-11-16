#include "corpus_index.hpp"
#include "corpus_index.hpp"

#include <cassert>
#include <sstream>

#include "../simple_test.h"

constexpr const char kURL1[] = "http://example.com/first";
constexpr const char kURL2[] = "http://example.com/second";
constexpr const char kURL3[] = "http://example.com/third";

TEST(index, no_docs) {
    const cls_06::CorpusIndex index{{}};

    EXPECT_EQ(0U, index.find("").size());
    EXPECT_EQ(0U, index.find("hello").size());
}

TEST(index, empty_text) {
    const cls_06::CorpusIndex index{{ {kURL1, ""}, }};

    EXPECT_EQ(0U, index.find("").size());
    EXPECT_EQ(0U, index.find("hello").size());
}

TEST(index, blank_text) {
    const cls_06::CorpusIndex index{{ {kURL1, "   \r \n \t  "}, }};

    EXPECT_EQ(0U, index.find("").size());
    EXPECT_EQ(0U, index.find(" ").size());
    EXPECT_EQ(0U, index.find("hello").size());
}

TEST(index, single_word) {
    const cls_06::Corpus corpus{ {kURL1, "abc"}, };
    const cls_06::CorpusIndex index{corpus};

    EXPECT_EQ(0U, index.find("").size());
    EXPECT_EQ(0U, index.find(" ").size());
    EXPECT_EQ(0U, index.find("hello").size());

    auto res = index.find("abc");
    ASSERT_EQ(1U, res.size());
    const auto* doc = res[0];
    ASSERT_TRUE(doc);
    EXPECT_EQ(corpus[0].uri, doc->uri);
    EXPECT_EQ(corpus[0].text, doc->text);
}

TEST(index, repeated_word) {
    const cls_06::Corpus corpus{ {kURL1, "abc abc  abc  abc \n abc"}, };
    const cls_06::CorpusIndex index{corpus};

    EXPECT_EQ(0U, index.find("").size());
    EXPECT_EQ(0U, index.find(" ").size());
    EXPECT_EQ(0U, index.find("hello").size());
    EXPECT_EQ(0U, index.find("abc ").size());
    EXPECT_EQ(0U, index.find(" abc").size());
    EXPECT_EQ(0U, index.find(" abc ").size());

    auto res = index.find("abc");
    ASSERT_EQ(1U, res.size());
    const auto* doc = res[0];
    ASSERT_TRUE(doc);
    EXPECT_EQ(corpus[0].uri, doc->uri);
    EXPECT_EQ(corpus[0].text, doc->text);
}

TEST(index, two_words) {
    const cls_06::Corpus corpus{ {kURL1, "abc def"}, };
    const cls_06::CorpusIndex index{corpus};

    EXPECT_EQ(0U, index.find("").size());
    EXPECT_EQ(0U, index.find("hello").size());

    auto res_abc = index.find("abc");
    ASSERT_EQ(1U, res_abc.size());
    const auto* doc_abc = res_abc[0];
    ASSERT_TRUE(doc_abc);
    EXPECT_EQ(corpus[0].uri, doc_abc->uri);
    EXPECT_EQ(corpus[0].text, doc_abc->text);

    // для каждого слова не создаются новые копии документов!
    auto res_def = index.find("def");
    ASSERT_EQ(1U, res_def.size());
    const auto* doc_def = res_def[0];
    ASSERT_EQ((const void*)doc_abc, (const void*)doc_def);
}

TEST(index, three_docs) {
    const cls_06::Corpus corpus{
        {kURL1, "abc def"},
        {kURL2, "abc def def abra shvabra kadabra def"},
        {kURL3, "foo bar abc abc"},
    };
    const cls_06::CorpusIndex index{corpus};

    EXPECT_EQ(0U, index.find("").size());
    EXPECT_EQ(0U, index.find("hello").size());

    auto res_abc = index.find("abc");
    ASSERT_EQ(3U, res_abc.size());
    EXPECT_EQ(kURL3, res_abc[0]->uri);  // 3 раза
    EXPECT_EQ(kURL1, res_abc[1]->uri);  // 1 раз, kURL1 < kURL2
    EXPECT_EQ(kURL2, res_abc[2]->uri);

    auto res_def = index.find("def");
    ASSERT_EQ(2U, res_def.size());
    EXPECT_EQ(kURL2, res_def[0]->uri);  // 2 раза
    EXPECT_EQ(kURL1, res_def[1]->uri);  // 1 раз

    auto res_bar = index.find("bar");
    ASSERT_EQ(1U, res_bar.size());
    EXPECT_EQ(kURL3, res_bar[0]->uri);  // 1 раза
}

TEST(index, punctuation) {
    for (int i = 1; i < 128; ++i) {
        char c = static_cast<char>(i);
        if (std::ispunct(c)) {
            const cls_06::CorpusIndex index{{
                {kURL1, std::string{"hello"} + c + std::string{"world"}},
            }};

            EXPECT_EQ(1U, index.find("hello").size()) << "char " << i << " = " << c;
            EXPECT_EQ(1U, index.find("world").size()) << "char " << i << " = " << c;
        }
    }
}

TEST(index, stress_test) {
    std::string kLoremIpsum = R"(
        Lorem ipsum dolor sit amet, consectetur adipiscing elit,
        sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
        Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris
        nisi ut aliquip ex ea commodo consequat.
        Duis aute irure dolor in reprehenderit in voluptate velit esse cillum
        dolore eu fugiat nulla pariatur.
        Excepteur sint occaecat cupidatat non proident,
        sunt in culpa qui officia deserunt mollit anim id est laborum
    )";
    std::string kDeFinibus = R"(
        Sed ut perspiciatis unde omnis iste natus error sit voluptatem
        accusantium doloremque laudantium, totam rem aperiam,
        eaque ipsa quae ab illo inventore veritatis et quasi architecto
        beatae vitae dicta sunt explicabo.
        Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit,
        sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt.
        Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur,
        adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore
        et dolore magnam aliquam quaerat voluptatem.
        Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis
        suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur?
        Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse
        quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat
        quo voluptas nulla pariatur?
    )";
    const cls_06::CorpusIndex index{{
        {kURL1, kLoremIpsum},
        {kURL2, kDeFinibus},
        {kURL3, kLoremIpsum + kDeFinibus},
    }};

    for (const std::string word : {"ipsum", "amet", "sunt", "reprehenderit"}) {
        for (int repeat = 0; repeat != 10000; ++repeat) {
            auto res = index.find(word);
            ASSERT_EQ(3U, res.size());
            EXPECT_EQ(kURL3, res[0]->uri);
            EXPECT_EQ(kURL1, res[1]->uri);
            EXPECT_EQ(kURL2, res[2]->uri);
        }
    }
}


TESTING_MAIN()
