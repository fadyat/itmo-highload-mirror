package itmo.scala.task1

import cats.Monoid
import cats.syntax.semigroup._
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers

class PointSpec extends AnyFlatSpec with Matchers {
  "Semigroup for Point" should "have instance" in {
    Point(1, 2, 3) |+| Point(3, 4, 5) shouldBe Point[Int](4, 6, 8)
  }

  it should "have instance for custom types" in {
    Point(one, one, one) |+| Point(two, two, two) shouldBe Point(three, three, three)
  }

  it should "be associative" in {
    Point(1, 2, 3) |+| Point(3, 4, 5) |+| Point(3, 4, 5) shouldBe Point(1, 2, 3) |+|
      (Point(3, 4, 5) |+| Point(3, 4, 5))
  }

  "Monoid for Point" should "have instance" in {
    empty[Int] shouldBe Point(0, 0, 0)
  }

  it should "left identity" in {
    empty[Int] |+| Point(3, 2, 1) shouldBe Point(3, 2, 1)
  }

  it should "right identity" in {
    Point(3, 2, 1) |+| empty[Int] shouldBe Point(3, 2, 1)
  }

  private def empty[T: Monoid]: Point[T] = Monoid[Point[T]].empty
  private lazy val one: PositiveInt      = PositiveInt(1)
  private lazy val two: PositiveInt      = PositiveInt(2)
  private lazy val three: PositiveInt    = PositiveInt(3)

  case class PositiveInt(number: Int)
  implicit private val positiveIntMonoid: Monoid[PositiveInt] =
    new Monoid[PositiveInt] {
      override def empty: PositiveInt                                   = PositiveInt(0)
      override def combine(x: PositiveInt, y: PositiveInt): PositiveInt = PositiveInt(x.number + y.number)
    }
}
