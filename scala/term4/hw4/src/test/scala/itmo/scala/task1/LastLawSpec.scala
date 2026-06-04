package itmo.scala.task1

import cats.Eq
import cats.kernel.laws.discipline.MonoidTests
import org.scalacheck.Arbitrary
import org.scalatest.funsuite.AnyFunSuite
import org.scalatestplus.scalacheck.Checkers
import org.typelevel.discipline.scalatest.FunSuiteDiscipline

class LastLawSpec extends AnyFunSuite with FunSuiteDiscipline with Checkers {

  checkAll(
    name = "Last.MonoidLaws",
    ruleSet = MonoidTests[Last[String]].monoid
  )

  implicit private def eq[A: Eq]: Eq[Last[A]] = {
    case (a, b) => Eq[Option[A]].eqv(a.value, b.value)
  }
  implicit private def arbitrary[A: Arbitrary]: Arbitrary[Last[A]] =
    Arbitrary(Arbitrary.arbOption[A].arbitrary.map(Last(_)))
}
