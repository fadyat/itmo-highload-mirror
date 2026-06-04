package itmo.scala.hw1.schoolclass

import itmo.scala.hw1.schoolclass.Knowledge._
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers

class SchoolClassSpec extends AnyFlatSpec with Matchers {

  "SchoolClass" should "correctly determine School Class type as KnowNothing" in {
    assertCompiles(
      """
        |val geniusAndEnlightenedClass: SchoolClass[KnowNothing] =
        |  new SchoolClass(Seq(new Genius))
        |    .accept(Seq(new Enlightened))
        |    .accept(Seq(new Normal))
        |    .accept(Seq(new PoorlyEducated))
        |    .accept(Seq(new KnowSomething))
        |    .accept(Seq(new Aggressive))
        |    .accept(Seq(new KnowNothing))
        |""".stripMargin
    )
  }

  it should "correctly determine School Class type as KnowSomething" in {
    assertCompiles(
      """
        |val geniusAndEnlightenedClass: SchoolClass[KnowSomething] =
        |  new SchoolClass(Nil)
        |    .accept(Seq(new Enlightened))
        |    .accept(Seq(new KnowSomething))
        |""".stripMargin
    )
  }

  it should "implement compile time checking" in {
    assertDoesNotCompile(
      """
        |val geniusAndEnlightenedClass: SchoolClass[Genius] =
        |  new SchoolClass(Seq(new Genius))
        |    .accept(Seq(new KnowNothing))
        |""".stripMargin
    )
  }

}
