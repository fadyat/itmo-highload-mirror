package itmo.scala.hw1.weekday

import itmo.scala.hw1.weekday.WeekDay._
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers

class WeekDaySpec extends AnyFlatSpec with Matchers {
  "WeekDay isWeekend" should "correctly work for weekend" in {
    WeekDay.isWeekend(WeekDay.fromInt(6)) shouldBe true
    WeekDay.isWeekend(WeekDay.fromInt(5)) shouldBe true
  }

  "WeekDay isWeekday" should "correctly work for weekend" in {
    WeekDay.isWeekday(WeekDay.fromInt(6)) shouldBe false
    WeekDay.isWeekday(WeekDay.fromInt(5)) shouldBe false
  }

  it should "correctly work for weekday" in {
    WeekDay.isWeekday(WeekDay.fromInt(0)) shouldBe true
    WeekDay.isWeekday(WeekDay.fromInt(1)) shouldBe true
    WeekDay.isWeekday(WeekDay.fromInt(2)) shouldBe true
    WeekDay.isWeekday(WeekDay.fromInt(3)) shouldBe true
    WeekDay.isWeekday(WeekDay.fromInt(4)) shouldBe true
  }

  "WeekDay daysToWeekend" should "correctly work for weekend" in {
    WeekDay.daysToWeekend(WeekDay.fromInt(6)) shouldBe 0
    WeekDay.daysToWeekend(WeekDay.fromInt(5)) shouldBe 0
  }

  it should "correctly work for weekday" in {
    WeekDay.daysToWeekend(WeekDay.fromInt(0)) shouldBe 5
    WeekDay.daysToWeekend(WeekDay.fromInt(1)) shouldBe 4
    WeekDay.daysToWeekend(WeekDay.fromInt(2)) shouldBe 3
    WeekDay.daysToWeekend(WeekDay.fromInt(3)) shouldBe 2
    WeekDay.daysToWeekend(WeekDay.fromInt(4)) shouldBe 1
  }

  "Weekday values" should "not be empty" in {
    WeekDay.values.isEmpty shouldBe false
  }

  it should "contains 7 days" in {
    WeekDay.values.distinct.length shouldBe 7
  }

  "Weekday names" should "be correct" in {
    assertCompiles("val days: List[WeekDay] = List(Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday)")
  }
}
