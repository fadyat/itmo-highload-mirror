#include "find_nearest_same.hpp"


ptrdiff_t findNearestSame(const char *s, const char *c) {
    const char *lptr = c - 1, *rptr = c + 1;

    while (lptr >= s || *rptr != '\0') {
        if (*rptr == *c) {
            return rptr - c;
        }

        if (lptr >= s && *lptr == *c) {
            return lptr - c;
        }

        if (*rptr != '\0') {
            ++rptr;
        }

        if (lptr >= s) {
            --lptr;
        }
    }

    return 0;
}