#include "expression.hpp"
#include "expression.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <type_traits>

#include "simple_test.h"

using std::stringstream;
using std::string;


static_assert(!std::is_convertible_v<Var, string>);


string printToString(const Expression* e) {
    stringstream ss;
    e->print(ss);
    return ss.str();
}

string makeEvalWithCtxString(const string& expressionRepr, const string& ctxRepr) {
    stringstream ss;
    ss << "eval(<ctx>) mismatch with ctx=" << ctxRepr << ", expr=" << expressionRepr;
    return ss.str();
}

string makeEvalWithCtxString(const Expression* expression, const string& ctxRepr) {
    return makeEvalWithCtxString("print()=" + printToString(expression), ctxRepr);
}

TEST(task_2, prints_with_var) {
    {
        Expression* var = new Var("x");

        EXPECT_EQ("x", printToString(var)) << "for Var(x)";
        delete var;
    }

    {
        Expression* sum = new Add(new Val(2), new Var("x"));
        Expression* mul = new Mul(sum, new Var("x"));

        EXPECT_EQ("(2 + x) * x", printToString(mul)) << "for Mul(Add(Val(2), Var(\"x\")), Var(\"x\"))";
        delete mul;
    }
}

TEST(task_2, evals) {
    {
        Expression* val = new Val(10);

        EXPECT_EQ(10, val->eval(Context())) << makeEvalWithCtxString(val, "{}");

        delete val;
    }

    {
        Context ctx;
        ctx.setVar("x", 100);

        Expression* var = new Var("x");
        
        EXPECT_EQ(100, var->eval(ctx)) << makeEvalWithCtxString(var, "{x: 100}");

        delete var;
    }

    {
        Expression* sum = new Add(new Val(2), new Var("x"));
        Expression* mul = new Mul(sum, new Var("x"));

        Context ctx;
        ctx.setVar("x", 10);

        EXPECT_EQ(120, mul->eval(ctx)) << makeEvalWithCtxString(mul, "{x: 10}");
        delete mul;
    }
}

TEST(task_2, unset_var) {
    auto* var = new Var("x");

    try {
        [[maybe_unused]] auto res = var->eval();
        FAIL() << "no exception";
    } catch (const std::runtime_error&) {
        // OK
    }  catch (simple_test::assertion_fault&) {
        throw;
    } catch (...) {
        FAIL() << "Wrong type of exception";
    }

    delete var;
}

TESTING_MAIN()
