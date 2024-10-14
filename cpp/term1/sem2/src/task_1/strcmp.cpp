#include "strcmp.hpp"


int myStrCmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        ++s1;
        ++s2;
    }

    return static_cast<unsigned char>(*s1) - static_cast<unsigned char>(*s2);
}
