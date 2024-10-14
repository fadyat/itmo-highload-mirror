#pragma once

#include <cstddef>

enum class WeekDay {
    MONDAY = 1,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY,
};

const char *GetDayOfWeekName(WeekDay w);

bool IsWeekend(WeekDay w);

WeekDay GetDayOfWeek(size_t year, size_t month, size_t day);
