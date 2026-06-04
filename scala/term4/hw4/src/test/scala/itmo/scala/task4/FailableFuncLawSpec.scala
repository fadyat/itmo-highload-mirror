package itmo.scala.task4

import cats.Eq
import cats.laws.discipline.MonadErrorTests
import org.scalacheck.{Arbitrary, Cogen}
import org.scalatest.funsuite.AnyFunSuite
import org.scalatestplus.scalacheck.Checkers
import org.typelevel.discipline.scalatest.FunSuiteDiscipline

class FailableFuncLawSpec extends AnyFunSuite with FunSuiteDiscipline with Checkers {

  checkAll(
    name = "FailableFunction.MonadErrorLaws",
    ruleSet =
      MonadErrorTests[FailableFunction[Int, String, *], String](FailableFunction.monadErrorFailableFunction)
        .monadError[Int, Int, String]
  )

  implicit private def eqFailable[E: Eq, A: Eq]: Eq[Failable[E, A]] = {
    case (Failable.Success(x), Failable.Success(y)) => Eq[A].eqv(x, y)
    case (Failable.Failure(x), Failable.Failure(y)) => Eq[E].eqv(x, y)
    case _                                          => false
  }

  implicit private def eqFuncInt[E: Eq, A: Eq]: Eq[FailableFunction[Int, E, A]] =
    (x: FailableFunction[Int, E, A], y: FailableFunction[Int, E, A]) =>
      // Check eq for two prime numbers and one composite number
      Eq[Failable[E, A]].eqv(x(1423), y(1423)) &&
        Eq[Failable[E, A]].eqv(x(2423), y(2423)) &&
        Eq[Failable[E, A]].eqv(x(1242), y(1242))

  implicit private def arbitrary[A: Cogen, B: Arbitrary, C: Arbitrary]: Arbitrary[FailableFunction[A, B, C]] =
    Arbitrary(Arbitrary.arbFunction1[A, Either[B, C]].arbitrary.map(FailableFunction.fromEither))
}
