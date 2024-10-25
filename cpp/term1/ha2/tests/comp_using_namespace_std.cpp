#include "bignum.hpp"

namespace templates
{
    struct vector
    {
        vector(int a)
        : a_{a}
        {}

        int a_;
    };

    struct string
    {
        string(char const* str)
        : str_{str}
        {}

        char const* const str_;
    };
}

using namespace templates;

int main()
{
    vector(42);
    string("Do not use \"using namespace std;\"");
    return 0;
}

