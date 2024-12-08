#include <cassert>
#include <vector>
#include "whatever.hpp"

int main()
{
    std::vector<int> vec = {1, 2, 3, 4, 5, 6};
    utils::whatever w(vec);

    assert(utils::whatever_cast<std::vector<int> &>(w).at(5) == 6);
    return 0;
}
