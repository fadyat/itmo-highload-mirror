package itmo.scala.hw1.weekday

import org.scalacheck.Prop.forAll
import org.scalacheck.{Arbitrary, Gen, Properties}

object WeekDayPropSpec extends Properties("WeekDay") {

  implicit private val arbitraryWeekDay: Arbitrary[WeekDay] = Arbitrary(
    Gen.oneOf[WeekDay](WeekDay.values)
  )

  property("convert int to weekday and back") = forAll { day: Int =>
    WeekDay.toInt(WeekDay.fromInt(day)) == (day % 7 + 7) % 7
  }

  property("correctly get next day") = forAll { day: Int =>
    WeekDay.toInt(WeekDay.nextDay(WeekDay.fromInt(day))) == (day % 7 + 8) % 7
  }

  property("correctly get prev day") = forAll { day: Int =>
    WeekDay.toInt(WeekDay.prevDay(WeekDay.fromInt(day))) == (day % 7 + 6) % 7
  }

  property("correctly weekday to name and back") = forAll { day: WeekDay =>
    WeekDay.fromNameOpt(WeekDay.name(day)).contains(day)
  }

  property("correctly daysBetween day and next day") = forAll { day: WeekDay =>
    WeekDay.daysBetween(day, WeekDay.nextDay(day)) == 1
  }

  property("correctly daysBetween day and prev day") = forAll { day: WeekDay =>
    WeekDay.daysBetween(day, WeekDay.prevDay(day)) == 6
  }
}
