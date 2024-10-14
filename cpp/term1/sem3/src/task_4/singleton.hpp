#pragma once

inline int inc() {
    static int counter = 0;
    return counter++;
}
