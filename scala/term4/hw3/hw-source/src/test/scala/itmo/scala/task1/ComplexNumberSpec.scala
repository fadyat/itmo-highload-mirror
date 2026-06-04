package itmo.scala.task1

import itmo.scala.{ComplexNumber, DivisionByZeroException}
import org.scalatest.concurrent.{Signaler, TimeLimitedTests}
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers
import org.scalatest.time.Span
import org.scalatest.time.SpanSugar._

class ComplexNumberSpec extends AnyFlatSpec with Matchers with TimeLimitedTests {
  "ComplexNumber.*" should "work simple cases" in {
    ComplexNumber(1, 1) * ComplexNumber(1, 1) shouldBe ComplexNumber(0, 2)
    ComplexNumber(1, 1) * ComplexNumber(1, -1) shouldBe ComplexNumber(2, 0)
  }

  "ComplexNumber.+" should "work simple cases" in {
    ComplexNumber(1, 1) + ComplexNumber(1, 1) shouldBe ComplexNumber(2, 2)
    ComplexNumber(1, 1) + ComplexNumber(1, -1) shouldBe ComplexNumber(2, 0)
  }

  "ComplexNumber.-" should "work simple cases" in {
    ComplexNumber(1, 1) - ComplexNumber(1, 1) shouldBe ComplexNumber(0, 0)
    ComplexNumber(1, 1) - ComplexNumber(1, -1) shouldBe ComplexNumber(0, 2)
  }

  "ComplexNumber.~=" should "work simple cases" in {
    (ComplexNumber(1, 1) ~= ComplexNumber(1, 1)) shouldBe true
    (ComplexNumber(1, 1) ~= ComplexNumber(1, -1)) shouldBe false
  }

  "ComplexNumber./" should "work simple cases" in {
    ComplexNumber(1, 1) / ComplexNumber(1, 1) shouldBe ComplexNumber(1, 0)
    ComplexNumber(1, 1) / ComplexNumber(1, -1) shouldBe ComplexNumber(0, 1)
    ComplexNumber(1, 1) / ComplexNumber(0, -1) shouldBe ComplexNumber(-1, 1)
    an[DivisionByZeroException.type] should be thrownBy { ComplexNumber(1, 1) / ComplexNumber(0, 0) }
  }

  ".i syntax" should "create ComplexNumber from 1 + 1.i" in {
    assertCompiles(
      """
        |  val simpleOne: ComplexNumber = 1 + 1.i
        |""".stripMargin
    )
  }

  it should "create ComplexNumber for different operations" in {
    assertCompiles(
      """
        |  val iOne: ComplexNumber         = 1.i
        |  val iOnePointOne: ComplexNumber = 1.0.i
        |  val iSum: ComplexNumber         = 1.i + 1.i
        |  val iSumNumeric: ComplexNumber  = 1 + 1.i
        |  val iSumNumeric2: ComplexNumber = 1.i + 1
        |  val iSubNumeric: ComplexNumber  = 1 - 1.i
        |  val iSubNumeric2: ComplexNumber = 1.i - 1
        |  val iMul: ComplexNumber         = 1.i * 1.i
        |  val iMulNumeric: ComplexNumber  = 1 * 1.i
        |  val iMulNumeric2: ComplexNumber = 1.i * 1
        |  val iDiv: ComplexNumber         = 1.i / 1.i
        |  val iDivNumeric: ComplexNumber  = 1 / 1.i
        |  val iDivNumeric2: ComplexNumber = 1.i / 1
        |""".stripMargin
    )
  }

  it should "create ComplexNumber from any Numeric" in {
    assertCompiles(
      """
        |  def foo[T: Numeric](img: T): ComplexNumber = img.i
        |""".stripMargin
    )
  }

  "Numeric syntax" should "create ComplexNumber from operations with ComplexNumber" in {
    assertCompiles(
      """
        |  def foo[T:Numeric](num: T): Unit = {
        |    val c: ComplexNumber = ComplexNumber(1, 1)
        |    val c2: ComplexNumber = c + num
        |    val c3: ComplexNumber = c - num
        |    val c4: ComplexNumber = c * num
        |    val c5: ComplexNumber = c / num
        |    val c6: ComplexNumber = num + c
        |    val c7: ComplexNumber = num - c
        |    val c8: ComplexNumber = num * c
        |    val c9: ComplexNumber = num / c
        |  }
        |""".stripMargin
    )
  }

  override def timeLimit: Span               = 10.seconds
  override val defaultTestSignaler: Signaler = _ => {
    println("ComplexNumberSpec: Test timeout")
    System.exit(1)
  }
}
