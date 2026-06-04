package itmo.scala.task1

import cats.Eq
import cats.kernel.laws.discipline.MonoidTests
import org.scalacheck.Arbitrary
import org.scalatest.funsuite.AnyFunSuite
import org.scalatestplus.scalacheck.Checkers
import org.typelevel.discipline.scalatest.FunSuiteDiscipline

class PointLawSpec extends AnyFunSuite with FunSuiteDiscipline with Checkers {

  checkAll(
    name = "Point.MonoidLaws",
    ruleSet = MonoidTests[Point[Int]].monoid
  )

  implicit private def eq[A: Eq]: Eq[Point[A]] = {
    case (a, b) =>
      Eq[A].eqv(a.x, b.x) &&
      Eq[A].eqv(a.y, b.y) &&
      Eq[A].eqv(a.z, b.z)
  }

  implicit private def arbitrary[A: Arbitrary]: Arbitrary[Point[A]] =
    Arbitrary(
      for {
        x <- Arbitrary.arbitrary[A]
        y <- Arbitrary.arbitrary[A]
        z <- Arbitrary.arbitrary[A]
      } yield Point(x, y, z)
    )
}
