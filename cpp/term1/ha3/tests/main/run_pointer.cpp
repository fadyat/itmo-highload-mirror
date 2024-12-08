#include <cassert>
#include "whatever.hpp"

void ptr_test() {
    int tmp{ 2 };

    utils::whatever a(&tmp);
    int **tmp_ptr_ptr = utils::whatever_cast<int *>(&a);

    assert(&tmp == *tmp_ptr_ptr);
}

int main()
{
    ptr_test();
    return 0;
}
