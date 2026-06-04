package itmo.scala.hw2.task3

import org.scalatest.concurrent.{Signaler, TimeLimitedTests}
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers
import org.scalatest.time.Span
import org.scalatest.time.SpanSugar._

import scala.collection.immutable.TreeMap

class SchoolSpec extends AnyFlatSpec with Matchers with TimeLimitedTests {
  private val student1: Student = Student("Ivanov Ivan Ivanovich", 12)
  private val student2: Student = Student("Petrov Petr Petrovich", 11)
  private val student3: Student = Student("Sidorov Sidor Sidorovich", 14)
  private val student4: Student = Student("Koznakov Kozhakov Kozhakovich", 14)

  private val grades: List[Row] = List(
    Row(student1, "task1", Subject.Math, Grade.A),
    Row(student1, "task1", Subject.Math, Grade.B),
    Row(student1, "task2", Subject.Math, Grade.D),
    Row(student1, "task2", Subject.Math, Grade.C),
    Row(student1, "task5", Subject.Math, Grade.E),
    Row(student1, "task1", Subject.Music, Grade.E),
    Row(student1, "task1", Subject.Sport, Grade.A),
    Row(student2, "task1", Subject.Math, Grade.E),
    Row(student2, "task1", Subject.Math, Grade.E),
    Row(student2, "task3", Subject.Math, Grade.FX),
    Row(student3, "task3", Subject.Physics, Grade.A),
    Row(student3, "task1", Subject.Physics, Grade.B),
    Row(student4, "1", Subject.English, Grade.C),
    Row(student4, "2", Subject.English, Grade.D),
    Row(student4, "1", Subject.English, Grade.B),
  )

  "School.calculateGrades" should "be empty for empty list" in {
    School.calculateGrades(List.empty) shouldBe Map.empty
  }

  it should "return correct grades" in {
    def sortGradesTable(table: Map[Student, Map[Subject, List[Grade]]]): Map[Student, Map[Subject, List[Grade]]] = {
      implicit val studentOrdering: Ordering[Student] = Ordering.by(s => (s.name, s.age))
      implicit val subjectOrdering: Ordering[Subject] = Ordering.by(_.toString)
      implicit val gradeOrdering: Ordering[Grade]     = Ordering.by(_.toString)

      TreeMap.from(table.view.mapValues(_.view.mapValues(_.sorted).toMap).mapValues(TreeMap.from(_)).toMap)
    }

    val expectedGradeTable = sortGradesTable(Map(
      student1 -> Map(
        Subject.Sport -> List(Grade.A),
        Subject.Music -> List(Grade.E),
        Subject.Math  -> List(Grade.A, Grade.C, Grade.E),
      ),
      student2 -> Map(
        Subject.Math -> List(Grade.E, Grade.FX),
      ),
      student3 -> Map(
        Subject.Physics -> List(Grade.B, Grade.A),
      ),
      student4 -> Map(
        Subject.English -> List(Grade.B, Grade.D),
      ),
    ))

    val actualGradeTable = sortGradesTable(School.calculateGrades(grades))

    actualGradeTable shouldBe expectedGradeTable
  }

  "School.minGradeForSubject" should "be empty for empty list" in {
    School.minGradeForSubject(List.empty) shouldBe Map.empty
  }

  it should "be correct" in {
    School.minGradeForSubject(grades) shouldBe Map(
      Subject.Math    -> Grade.FX,
      Subject.Music   -> Grade.E,
      Subject.Sport   -> Grade.A,
      Subject.Physics -> Grade.B,
      Subject.English -> Grade.D,
    )
  }

  override def timeLimit: Span               = 10.seconds
  override val defaultTestSignaler: Signaler = _ => {
    println("SchoolSpec: Test timeout")
    System.exit(1)
  }
}
