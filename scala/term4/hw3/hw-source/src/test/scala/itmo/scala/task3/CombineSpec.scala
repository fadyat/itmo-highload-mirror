package itmo.scala.task3

import itmo.scala.ComplexNumber
import org.scalatest.concurrent.{Signaler, TimeLimitedTests}
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers
import org.scalatest.time.Span
import org.scalatest.time.SpanSugar._

class CombineSpec extends AnyFlatSpec with Matchers with TimeLimitedTests {

  "Combine" should "combine Strings" in {
    Combine.combineAll("a", "b", "cd") shouldBe "abcd"
    Combine.combineAll("aaaa") shouldBe "aaaa"

    Combine.combine("a", "bba") shouldBe "abba"
    Combine.maybeCombine("a", Some("bba")) shouldBe "abba"
    Combine.maybeCombine("a", None) shouldBe "a"
  }

  it should "combine Ints" in {
    Combine.combineAll(1, 2, 3) shouldBe 6
    Combine.combineAll(0) shouldBe 0

    Combine.combine(-2, 3) shouldBe 1
  }

  it should "combine ComplexNumbers" in {
    Combine.combineAll(ComplexNumber(1, 1), ComplexNumber(1, 1)) shouldBe ComplexNumber(2, 2)
    Combine.combineAll(ComplexNumber(1, 1), ComplexNumber(-1, -1)) shouldBe ComplexNumber(0, 0)
    Combine.combineAll(ComplexNumber(1, 1)) shouldBe ComplexNumber(1, 1)

    Combine.combine(ComplexNumber(1, 0), ComplexNumber(1, 1)) shouldBe ComplexNumber(2, 1)
  }

  override def timeLimit: Span = 10.seconds

  override val defaultTestSignaler: Signaler = _ => {
    println("CombineSpec: Test timeout")
    System.exit(1)
  }
}
