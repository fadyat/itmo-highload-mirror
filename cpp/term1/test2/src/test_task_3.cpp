#include "expression.hpp"
#include "expression.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>

#include "custom_new_delete.hpp"
#include "simple_test.h"

using std::stringstream;
using std::string;

void assertWithThrow(bool condition, string const& msg) {
    if (!condition) {
        throw std::runtime_error(msg);
    }
}

string printToString(const Expression* e) {
    stringstream ss;
    e->print(ss);
    return ss.str();
}

string makeSimplifiedWithCtxString(const string& expressionRepr, const string& ctxRepr) {
    stringstream ss{};
    ss << "simplified(<ctx>) mismatch with ctx=" << ctxRepr << ", expr=" << expressionRepr;
    return ss.str();
}

string makeSimplifiedWithCtxString(const Expression* expression, const string& ctxRepr) {
    return makeSimplifiedWithCtxString("print()=" + printToString(expression), ctxRepr);
}


TEST(task_3, full_simplification) {
    {
        Expression* val = new Val(10);
        Expression* simplifiedVal = val->simplified(Context());

        EXPECT_EQ("10", printToString(simplifiedVal));

        EXPECT_EQ(10, simplifiedVal->eval(Context())) << "Val(10), simplified in empty context, should be evaled to 10";

        delete simplifiedVal;
        delete val;
    }

    {
        Context ctx;
        ctx.setVar("x", 100);

        Expression* var = new Var("x");
        Expression* simplifiedVar = var->simplified(ctx);

        EXPECT_EQ("100", printToString(simplifiedVar));

        EXPECT_EQ(100, simplifiedVar->eval()) << "Var(x) simplified with context, where x = 100, should be evaled to 100";

        delete simplifiedVar;
        delete var;
    }

    {
        Expression* mul = new Mul(new Var("x"), new Var("y"));
        Expression* sum = new Add(mul, new Var("y"));

        Context full_ctx;
        full_ctx.setVar("x", 2);
        full_ctx.setVar("y", 3);

        Expression* mulSimplifiedToValue = mul->simplified(full_ctx);
        Val *value = dynamic_cast<Val *>(mulSimplifiedToValue);
        EXPECT_TRUE(value);
        EXPECT_EQ(6, value->eval());
        delete mulSimplifiedToValue;
        delete sum;
    }
}

TEST(task_3, partial_simplification) {
    {
        Context x_ctx;
        x_ctx.setVar("x", 2);

        Context y_ctx;
        y_ctx.setVar("y", 3);

        Expression* mul = new Mul(new Var("x"), new Var("y"));
        Expression* sum = new Add(mul, new Var("y"));
        const string exprRepr = "Add(Mul(Var(x), Var(y)), Var(y))";

        Expression* simplifiedWithX = sum->simplified(x_ctx);
        Expression* simplifiedWithY = sum->simplified(y_ctx);

        EXPECT_EQ("(2 * y + y)", printToString(simplifiedWithX)) << makeSimplifiedWithCtxString(sum, "{x: 2}");

        EXPECT_EQ(9, simplifiedWithX->eval(y_ctx)) << "x * y + y, simplified with {x = 2}, should be evaled to 5 in context {y = 3}";


        EXPECT_EQ("(x * 3 + 3)", printToString(simplifiedWithY)) << makeSimplifiedWithCtxString(sum, "{y: 3");
        EXPECT_EQ(9, simplifiedWithY->eval(x_ctx)) << "x * y + y, simplified with {y = 3}, should be evaled to 5 in context {x = 2}";

        {
            Expression* sumSimplifiedToValue = simplifiedWithX->simplified(y_ctx);
            Val *value = dynamic_cast<Val *>(sumSimplifiedToValue);
            EXPECT_TRUE(value);
            EXPECT_EQ(9, value->eval());
            delete sumSimplifiedToValue;
        }

        delete simplifiedWithY;
        delete simplifiedWithX;
        delete sum;
    }
}

TEST(task_3, count_allocations) {
    {
        Expression* sum = new Add(
            new Val(10),
            new Add(
                new Val(20),
                new Var("x")
            )
        );

        Context ctx;
        ctx.setVar("x", 2);

        MethodsCounterHolder h;
        Expression* simplified = sum->simplified(ctx);
        // все выражение сворачивается до значений, ничего промежуточного не создается
        EXPECT_EQ(5UL, GetRegisteredNewCalls<Val>());
        EXPECT_EQ(0UL, GetRegisteredNewCalls<Var>());
        EXPECT_EQ(0UL, GetRegisteredNewCalls<Add>());
        EXPECT_EQ(0UL, GetRegisteredNewCalls<Mul>());

        delete sum;
        delete simplified;

    }

    {
        Expression* sum = new Add(
            new Val(10),
            new Add(
                new Val(20),
                new Var("x")
            )
        );

        MethodsCounterHolder h;
        Expression* simplified = sum->simplified({});
        // ровно по исходному числу склонированных сущностей, больше — избыточно
        EXPECT_EQ(2UL, GetRegisteredNewCalls<Val>());
        EXPECT_EQ(1UL, GetRegisteredNewCalls<Var>());
        EXPECT_EQ(2UL, GetRegisteredNewCalls<Add>());
        EXPECT_EQ(0UL, GetRegisteredNewCalls<Mul>());

        delete sum;
        delete simplified;
    }

    {
        Expression* sum = new Add(
            new Var("y"),
            new Add(
                new Val(20),
                new Var("x")
            )
        );

        Context ctx;
        ctx.setVar("x", 2);

        MethodsCounterHolder h;
        Expression* simplified = sum->simplified(ctx);
        // эффективно сворачивается правая ветка выражения
        EXPECT_EQ(3UL, GetRegisteredNewCalls<Val>());
        EXPECT_EQ(1UL, GetRegisteredNewCalls<Var>());
        EXPECT_EQ(1UL, GetRegisteredNewCalls<Add>());
        EXPECT_EQ(0UL, GetRegisteredNewCalls<Mul>());

        delete sum;
        delete simplified;
    }

    {
        Expression* mul = new Mul(
            new Var("y"),
            new Mul(
                new Val(20),
                new Var("x")
            )
        );

        Context ctx;
        ctx.setVar("x", 2);

        MethodsCounterHolder h;
        Expression* simplified = mul->simplified(ctx);
        // эффективно сворачивается правая ветка выражения
        EXPECT_EQ(3UL, GetRegisteredNewCalls<Val>());
        EXPECT_EQ(1UL, GetRegisteredNewCalls<Var>());
        EXPECT_EQ(0UL, GetRegisteredNewCalls<Add>());
        EXPECT_EQ(1UL, GetRegisteredNewCalls<Mul>());

        delete mul;
        delete simplified;
    }
}

TESTING_MAIN()
