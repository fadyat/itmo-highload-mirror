package itmo.scala.task2

import itmo.scala.{ComplexNumber, Show}
import org.scalatest.concurrent.{Signaler, TimeLimitedTests}
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers
import org.scalatest.time.Span
import org.scalatest.time.SpanSugar._

class ShowSpec extends AnyFlatSpec with Matchers with TimeLimitedTests {

  "Show" should "create str for List[ComplexNumber]" in {
    Show.show(List(ComplexNumber(1, 1), ComplexNumber(1, 1))) shouldBe "[1.0 + 1.0i, 1.0 + 1.0i]"
    Show.show(List(ComplexNumber(1, 1), ComplexNumber(1, -1))) shouldBe "[1.0 + 1.0i, 1.0 - 1.0i]"
  }

  override def timeLimit: Span = 10.seconds

  override val defaultTestSignaler: Signaler = _ => {
    println("ShowSpec: Test timeout")
    System.exit(1)
  }
}
