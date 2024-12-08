#include <cassert>
#include "whatever.hpp"

namespace
{
    struct Int
    {
        int value;
    };
}


void array_test()
{
    {
        const int const_arr[] = {1, 2, 3};
        utils::whatever any(const_arr);
        assert(utils::whatever_cast<const int *>(any) == const_arr);
        assert(nullptr == utils::whatever_cast<int *>(&any));

        float arr[] = {1.f, 2.f};
        any = arr;
        assert(utils::whatever_cast<float *>(any) == arr);
        assert(nullptr == utils::whatever_cast<const float *>(&any));
    }

    {
        double arr[] = {1., 2., 3., 4., 5.};
        utils::whatever any(arr);
        assert(utils::whatever_cast<double *>(any) == arr);
        assert(nullptr == utils::whatever_cast<const double *>(&any));

        const Int const_arr[] = {{1}, {2}, {3}, {4}};
        any = const_arr;
        assert(utils::whatever_cast<const Int *>(any) == const_arr);
        assert(nullptr == utils::whatever_cast<Int *>(&any));
    }
}

int main()
{
    array_test();
    return 0;
}
