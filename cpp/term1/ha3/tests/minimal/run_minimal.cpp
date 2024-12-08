#include <cassert>
#include <string>
#include <iostream>

#include "whatever.hpp"

using utils::whatever;
using utils::whatever_cast;
using std::string;
using std::cout;
using std::cerr;
using std::endl;

void contruct_test()
{
    whatever def;
    whatever copy_on_type(42);
    assert(def.empty());
    def = 3.14;
    def = string("2.71");
    assert(!def.empty());
    whatever e;
    assert(e.empty());
}

template<class VALUE_TYPE, class WHATEVER_PTR_TYPE>
void check_ptr_cast(WHATEVER_PTR_TYPE a, bool should_fail)
{
    VALUE_TYPE *res = whatever_cast<VALUE_TYPE>(a);
    std::cout << res << std::endl;
    assert((should_fail && res == nullptr) ||
        (!should_fail && res != nullptr));
}

void retrieve_value_test()
{
    whatever ia(42);
    check_ptr_cast<double>(&ia, true);
    check_ptr_cast<const double>(&ia, true);

    check_ptr_cast<int>(&ia, false);
    check_ptr_cast<const int> (&ia, false);

    assert(*whatever_cast<int>(&ia) == 42);
}

void swap_test()
{
    whatever a(5), b(string("6"));
    swap(a, b);
    int *bref = whatever_cast<int>(&b);
    std::string *aref = whatever_cast<std::string>(&a);
    assert(bref && *bref == 5);
    assert(aref && *aref == "6");
    std::cout << bref << " " << aref << std::endl;
}

int main()
{
    contruct_test();
    retrieve_value_test();
    swap_test();
    return 0;
}
