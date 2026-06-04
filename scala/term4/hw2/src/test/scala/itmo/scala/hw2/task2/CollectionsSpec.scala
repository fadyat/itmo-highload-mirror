package itmo.scala.hw2.task2

import itmo.scala.hw2.task2.ZeroOne.{One, Zero}
import org.scalatest.concurrent.{Signaler, TimeLimitedTests}
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers
import org.scalatest.time.Span
import org.scalatest.time.SpanSugar._

class CollectionsSpec extends AnyFlatSpec with Matchers with TimeLimitedTests {
  "Collections.findGaps" should "return no gaps for empty list" in {
    Collections.findGaps(List.empty) shouldBe None
  }

  it should "return no gaps for list with one element" in {
    Collections.findGaps(List(1)) shouldBe None
  }

  it should "return no gaps for list with two elements" in {
    Collections.findGaps(List(1, 2)) shouldBe None
  }

  it should "return no gaps for big sequence" in {
    Collections.findGaps(Range(3, 100000)) shouldBe None
  }

  it should "return gaps for list with gaps" in {
    Collections.findGaps(List(1, 3, 5, 7, 9)) shouldBe Some(List((1, 3), (3, 5), (5, 7), (7, 9)))
  }

  it should "return gaps for example" in {
    Collections.findGaps(List(1, 2, 3, 4)) shouldBe None
    Collections.findGaps(List(1, 2, 8)) shouldBe Some(List((2, 8)))
    Collections.findGaps(List(3, 5, 7)) shouldBe Some(List((3, 5), (5, 7)))
    Collections.findGaps(List(3, 7)) shouldBe Some(List((3, 7)))
  }

  it should "not depends from range size" in {
    Collections.findGaps(List(Int.MinValue, Int.MaxValue)) shouldBe Some(List((Int.MinValue -> Int.MaxValue)))
  }

  "Collections.count" should "works for example" in {
    Collections.count("") shouldBe List()
    Collections.count("a") shouldBe List(('a', 1))
    Collections.count("aa") shouldBe List(('a', 2))
    Collections.count("aaabaa") shouldBe List(('a', 3), ('b', 1), ('a', 2))
  }

  "Collections.sortUsers" should "works for example" in {
    Collections.sortUsers(List(
      User("Sidorov", "Ivan", "Ivanovich", 30),
      User("Ivanov", "Petr", "Petrovich", 13),
      User("Ivanov", "Sidor", "Sidor", 20)
    )) shouldBe List(
      User("Ivanov", "Sidor", "Sidor", 20),
      User("Ivanov", "Petr", "Petrovich", 13),
      User("Sidorov", "Ivan", "Ivanovich", 30),
    )
  }

  "Collections.maximizeZeros" should "return 0 for empty list" in {
    Collections.maximizeZeros(List.empty) shouldBe 0
  }

  it should "return 0 for list with one element" in {
    Collections.maximizeZeros(List(One)) shouldBe 0
    Collections.maximizeZeros(List(Zero)) shouldBe 0
  }

  it should "work for example" in {
    Collections.maximizeZeros(List(Zero, Zero, One, Zero, Zero, Zero, One, Zero, One, Zero, Zero)) shouldBe 5
    Collections.maximizeZeros(List(Zero, Zero, One, Zero, Zero, Zero, One, Zero, Zero, Zero, One)) shouldBe 6
    Collections.maximizeZeros(List(One, Zero, Zero, One, One, Zero, Zero, Zero, One, One, Zero, Zero, One)) shouldBe 3
    Collections.maximizeZeros(List(Zero, Zero, Zero)) shouldBe 2
    Collections.maximizeZeros(List(Zero, One, Zero, Zero)) shouldBe 3
    Collections.maximizeZeros(List(Zero, Zero, Zero, One)) shouldBe 3
    Collections.maximizeZeros(List(One, Zero, Zero, Zero)) shouldBe 3
    Collections.maximizeZeros(List(One)) shouldBe 0
  }

  override def timeLimit: Span               = 10.seconds
  override val defaultTestSignaler: Signaler = _ => {
    println("CollectionsSpec: Test timeout")
    System.exit(1)
  }
}
