#include "expression.hpp"
#include "expression.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <type_traits>

#include "simple_test.h"

using std::stringstream;
using std::string;


static_assert(!std::is_copy_constructible_v<Mul>);
static_assert(!std::is_copy_assignable_v<Mul>);
static_assert(!std::is_copy_constructible_v<Add>);
static_assert(!std::is_copy_assignable_v<Add>);


string printToString(const Expression* e) {
    stringstream ss;
    e->print(ss);
    return ss.str();
}

std::string makeEvalWithCtxString(const string& expressionRepr, const string& ctxRepr) {
    stringstream ss{};
    ss << "eval(<ctx>) mismatch with ctx=" << ctxRepr << ", expr=" << expressionRepr;
    return ss.str();
}

TEST(task_1_5, evals) {
    {
        Expression* val = new Val(10);

        EXPECT_EQ(10, val->eval()) << makeEvalWithCtxString("Val(10)", "{}");

        delete val;
    }

    {
        Expression* val = new Mul(new Val(10), new Val(20));

        EXPECT_EQ(200, val->eval()) << makeEvalWithCtxString("Mul(Val(10), Val(20))", "{}");

        delete val;
    }

    {
        Expression* sum = new Add(new Val(3), new Val(7));
        Expression* mul = new Mul(sum, new Val(20));

        EXPECT_EQ(200, mul->eval()) << makeEvalWithCtxString("Mul(Add(Val(3), Val(7)), Val(20))", "{}");

        delete mul;
    }
}

TESTING_MAIN()
