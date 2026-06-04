package itmo.scala.hw2.task2

import org.scalatest.concurrent.{Signaler, TimeLimitedTests}
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers
import org.scalatest.time.Span
import org.scalatest.time.SpanSugar._

class TransformationSpec extends AnyFlatSpec with Matchers with TimeLimitedTests {
  private val input: List[String] = List("something", "Anything", "EVERYTHING", "noThing", "objective", "search")

  "Transformation.lengthOfStrings" should "work" in {
    Transformation.lengthOfStrings(List.empty) shouldBe List.empty
    Transformation.lengthOfStrings(input) shouldBe List(9, 8, 10, 7, 9, 6)
  }

  "Transformation.splitWords" should "work" in {
    Transformation.splitWords(List.empty) shouldBe List.empty
    Transformation.splitWords(input) shouldBe List(
      's', 'o', 'm', 'e', 't', 'h', 'i', 'n', 'g',
      'A', 'n', 'y', 't', 'h', 'i', 'n', 'g',
      'E', 'V', 'E', 'R', 'Y', 'T', 'H', 'I', 'N', 'G',
      'n', 'o', 'T', 'h', 'i', 'n', 'g',
      'o', 'b', 'j', 'e', 'c', 't', 'i', 'v', 'e',
      's', 'e', 'a', 'r', 'c', 'h'
    )
  }

  "Transformation.lengthOfUpStrings" should "work" in {
    Transformation.lengthOfUpStrings(input) shouldBe List(8, 10)
    Transformation.lengthOfUpStrings(List.empty) shouldBe List.empty
  }

  "Transformation.headElement" should "work" in {
    Transformation.headElement(List.empty) shouldBe None
    Transformation.headElement(input) shouldBe Some("something")
  }

  "Transformation.lastElement" should "work" in {
    Transformation.lastElement(List.empty) shouldBe None
    Transformation.lastElement(input) shouldBe Some("search")
  }

  "Transformation.splitFrom2To5" should "work" in {
    Transformation.splitFrom2To5(List.empty) shouldBe List.empty
    Transformation.splitFrom2To5(input) shouldBe List("Anything", "EVERYTHING", "noThing", "objective")
  }

  "Transformation.take4Elements" should "work" in {
    Transformation.take4Elements(List.empty) shouldBe List.empty
    Transformation.take4Elements(input) shouldBe List("something", "Anything", "EVERYTHING", "noThing")
  }

  "Transformation.removeUpStrings" should "work" in {
    Transformation.removeUpStrings(List.empty) shouldBe List.empty
    Transformation.removeUpStrings(input) shouldBe List("something", "objective", "search")
  }

  "Transformation.groupByLength" should "work" in {
    Transformation.groupByLength(List.empty) shouldBe Map.empty
    Transformation.groupByLength(input) shouldBe Map(
      9  -> List("something", "objective"),
      8  -> List("Anything"),
      7  -> List("noThing"),
      10 -> List("EVERYTHING"),
      6  -> List("search")
    )
  }

  "Transformation.groupByLengthAndCount" should "work" in {
    Transformation.groupByLengthAndCount(List.empty) shouldBe Map.empty
    Transformation.groupByLengthAndCount(input) shouldBe Map(
      9  -> 2,
      8  -> 1,
      7  -> 1,
      10 -> 1,
      6  -> 1
    )
  }

  "Transformation.sumOfLength" should "work" in {
    Transformation.sumOfLength(List.empty) shouldBe 0
    Transformation.sumOfLength(input) shouldBe 49
  }

  "Transformation.formatList" should "work" in {
    Transformation.formatList(List.empty) shouldBe "[]"
    Transformation.formatList(input) shouldBe
      """["something", "Anything", "EVERYTHING", "noThing", "objective", "search"]"""
  }

  "Transformation.sumOfLengthTailRec" should "work" in {
    Transformation.sumOfLengthTailRec(List.empty) shouldBe 0
    Transformation.sumOfLengthTailRec(input) shouldBe 49
  }

  "Transformation.sumOfLengthFoldLeft" should "work" in {
    Transformation.sumOfLengthFoldLeft(List.empty) shouldBe 0
    Transformation.sumOfLengthFoldLeft(input) shouldBe 49
  }

  "Transformation.sort" should "work" in {
    Transformation.sort(List.empty) shouldBe List.empty
    Transformation.sort(input) shouldBe List("Anything", "EVERYTHING", "noThing", "objective", "search", "something")
  }

  "Transformation.sortByLen" should "work" in {
    Transformation.sortByLen(List.empty) shouldBe List.empty

    val sortByLenInput = "ab" :: "bb" :: "aa" :: input
    Transformation.sortByLen(sortByLenInput) shouldBe
      List("aa", "ab", "bb", "search", "noThing", "Anything", "objective", "something", "EVERYTHING")
  }

  override def timeLimit: Span               = 10.seconds
  override val defaultTestSignaler: Signaler = _ => {
    println("TransformationSpec: Test timeout")
    System.exit(1)
  }
}
