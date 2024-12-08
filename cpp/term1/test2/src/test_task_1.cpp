#include "expression.hpp"
#include "expression.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <type_traits>

#include "simple_test.h"

using std::stringstream;
using std::string;



static_assert(std::is_abstract_v<Expression>);
static_assert(!std::is_convertible_v<Val, int64_t>);


string printToString(const Expression* e) {
    stringstream ss;
    e->print(ss);
    return ss.str();
}


TEST(task_1, prints) {
    {
        Val val(10);

        EXPECT_EQ("10", printToString(&val)) << "for Val(10)";
    }

    {
        Expression const* val = new Add(new Val(10), new Val(20));

        EXPECT_EQ("(10 + 20)", printToString(val)) << "for Add(Val(10), Val(20))";

        delete val;
    }

    {
        Expression const* val = new Mul(new Val(10), new Val(20));

        EXPECT_EQ("10 * 20", printToString(val)) << " for Mul(Val(10), Val(20))";

        delete val;
    }

    {
        Expression* sum = new Add(new Val(20), new Val(30));
        Expression const* mul = new Mul(new Val(10), sum);

        EXPECT_EQ("10 * (20 + 30)", printToString(mul)) << "for Mul(Val(10), Add(Val(20), Val(30))";

        delete mul;
    }
}

TESTING_MAIN()
