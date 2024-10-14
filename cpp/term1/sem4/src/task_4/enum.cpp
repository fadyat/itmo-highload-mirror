#include "enum.hpp"

static const char *days[] = {
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday",
};

const char *GetDayOfWeekName(WeekDay w) {
    return days[static_cast<int>(w) - 1];
}

bool IsWeekend(WeekDay w) {
    return w == WeekDay::SATURDAY || w == WeekDay::SUNDAY;
}

// https://en.wikipedia.org/wiki/Zeller%27s_congruence
WeekDay GetDayOfWeek(size_t year, size_t month, size_t day) {
    size_t
            K = year % 100,
            J = year / 100,
            h = day + (13 * (month + 1) / 5) + K + (K / 4) + (J / 4) - 2 * J;

    return static_cast<WeekDay>((h + 5) % 7 + 1);
}