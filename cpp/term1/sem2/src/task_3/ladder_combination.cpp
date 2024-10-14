#include "ladder_combination.hpp"

uint64_t ladderCombinations(uint8_t steps) {
    static uint64_t cache[21] = {0, 1};
    static uint8_t precalculated = 1;

    if (precalculated < steps) {
        for (uint8_t i = precalculated + 1; i <= steps; ++i) {
            cache[i] = cache[i - 1] + cache[i - 2];
        }

        precalculated = steps;
    }

    return cache[steps];
}
