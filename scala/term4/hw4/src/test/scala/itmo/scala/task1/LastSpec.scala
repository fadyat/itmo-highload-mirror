package itmo.scala.task1

import cats.Monoid
import cats.syntax.semigroup._
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers

class LastSpec extends AnyFlatSpec with Matchers {
  "Last" should "have monoid instance" in {
    empty[Nothing] shouldBe Last(None)
  }

  it should "right combine with empty" in {
    Last(Some(1)) |+| empty shouldBe Last(Some(1))
  }

  it should "left combine with empty" in {
    empty[Int] |+| Last(Some(1)) shouldBe Last(Some(1))
  }

  it should "combine with non empty" in {
    Last(Some(1)) |+| Last(Some(2)) shouldBe Last(Some(2))
  }

  "Semigroup for Last" should "be associative" in {
    Last(Some(1)) |+| Last(Some(2)) |+| Last(Some(3)) shouldBe Last(Some(1)) |+|
      (Last(Some(2)) |+| Last(Some(3)))
  }

  private def empty[T]: Last[T] =
    Monoid[Last[T]].empty
}
