#include "enum.hpp"
#include "enum.hpp"

#include <cstring>

#include "../simple_test.h"

int main() {
  EXPECT_TRUE(!std::strcmp(GetDayOfWeekName(WeekDay::MONDAY), "Monday"));
  EXPECT_TRUE(!std::strcmp(GetDayOfWeekName(static_cast<WeekDay>(2)), "Tuesday"));
  EXPECT_TRUE(!std::strcmp(GetDayOfWeekName(WeekDay::WEDNESDAY), "Wednesday"));
  EXPECT_TRUE(!std::strcmp(GetDayOfWeekName(static_cast<WeekDay>(4)), "Thursday"));
  EXPECT_TRUE(!std::strcmp(GetDayOfWeekName(static_cast<WeekDay>(5)), "Friday"));
  EXPECT_TRUE(!std::strcmp(GetDayOfWeekName(static_cast<WeekDay>(6)), "Saturday"));
  EXPECT_TRUE(!std::strcmp(GetDayOfWeekName(static_cast<WeekDay>(7)), "Sunday"));

  EXPECT_FALSE(IsWeekend(WeekDay::THURSDAY));
  EXPECT_TRUE(IsWeekend(WeekDay::SATURDAY));

  EXPECT_EQ(WeekDay::TUESDAY, GetDayOfWeek(2020, 10, 13));

  return 0;
}