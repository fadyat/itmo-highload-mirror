package itmo.scala.task2

import cats.effect.IO
import cats.effect.testing.scalatest.AsyncIOSpec
import cats.effect.testkit.TestControl
import cats.syntax.traverse._
import org.scalatest.freespec.AsyncFreeSpec
import org.scalatest.matchers.should.Matchers

import scala.concurrent.duration.DurationInt

class IOEvalsSpec extends AsyncFreeSpec with AsyncIOSpec with Matchers {

  "IOEvals.foldF" - {
    "return zero if list is empty" in {
      IOEvals.foldF[Int, Int](List.empty[IO[Int]], 0, _ + _)
        .map(_ shouldBe 0)
    }

    "return sum of all elements" in {
      IOEvals.foldF[Int, Int](List(IO.pure(1), IO.pure(2), IO.pure(3)), 0, _ + _)
        .map(_ shouldBe 6)
    }

    "return sum of all elements with initial value" in {
      IOEvals.foldF[Int, Int](List(IO.pure(1), IO.pure(2), IO.pure(3)), 10, _ + _)
        .map(_ shouldBe 16)
    }

    "fail if any of the IO fails" in {
      val exception = new Exception("Catch me if you can!")
      IOEvals.foldF[Int, Int](List(IO.pure(1), IO.raiseError(exception), IO.pure(3)), 0, _ + _)
        .attempt.map(_ shouldBe Left(exception))
    }

    "fold list of IO into a single IO using combining function" in {
      val input = List(1, 2, 3, 4, 5, 6).map(IO.pure)

      for {
        sum <- IOEvals.foldF[Int, Int](input, 0, _ + _)
        mul <- IOEvals.foldF[Int, Int](input, 1, _ * _)
      } yield {
        sum shouldBe 21
        mul shouldBe 720
      }
    }
  }

  "IOEvals.flatFoldF" - {
    "return zero if list is empty" in {
      IOEvals.flatFoldF[Int, Int](List.empty[IO[Int]], 0, (b, a) => IO.pure(b + a))
        .map(_ shouldBe 0)
    }

    "return sum of all elements" in {
      IOEvals.flatFoldF[Int, Int](List(IO.pure(1), IO.pure(2), IO.pure(3)), 0, (b, a) => IO.pure(b + a))
        .map(_ shouldBe 6)
    }

    "return sum of all elements with initial value" in {
      IOEvals.flatFoldF[Int, Int](List(IO.pure(1), IO.pure(2), IO.pure(3)), 10, (b, a) => IO.pure(b + a))
        .map(_ shouldBe 16)
    }

    "fold list of IO into a single future using async combining function" in {
      val input   = List(1, 2, 3, 4, 5, 6).map(IO.pure)
      val sum     = (a: Int, b: Int) => IO.pure(a + b)
      val product = (a: Int, b: Int) => IO.pure(a * b)

      for {
        sum <- IOEvals.flatFoldF[Int, Int](input, 0, sum)
        mul <- IOEvals.flatFoldF[Int, Int](input, 1, product)
      } yield {
        sum shouldBe 21
        mul shouldBe 720
      }
    }

    "fail if any of the futures fails" in {
      val exception = new Exception("Catch me if you can!")
      IOEvals.flatFoldF[Int, Int](
        List(IO.pure(1), IO.raiseError(exception), IO.pure(3)),
        0,
        (b, a) => IO.pure(b + a)
      ).attempt.map(_ shouldBe Left(exception))
    }

    "fail if reduce function fails" in {
      val exception = new Exception("Catch me if you can!")
      IOEvals.flatFoldF[Int, Int](
        List(IO.pure(1), IO.pure(2), IO.pure(3)),
        0,
        (_, _) => IO.raiseError(exception)
      ).attempt.map(_ shouldBe Left(exception))
    }

    "fail if reduce function throws exception" in {
      val exception = new Exception("Catch me if you can!")
      IOEvals.flatFoldF[Int, Int](
        List(IO.pure(1), IO.pure(2), IO.pure(3)),
        0,
        (_, _) => throw exception
      ).attempt.map(_ shouldBe Left(exception))
    }

  }

  "IOEvals.fullSequence" - {
    "process list of success IO" in {
      val xs = (1 to 3).map(sleep).toList

      TestControl.executeEmbed {
        IOEvals.fullSequence[Int](xs)
          .map(_ shouldBe (List(1, 2, 3), List()))
      }
    }

    "process list of success and failures" in {
      val ex1: Exception       = new Exception("ex1")
      val ex2: Exception       = new Exception("ex2")
      val failed1: IO[Nothing] = IO.raiseError(ex1)
      val failed2: IO[Nothing] = IO.raiseError(ex2)
      val fut1: IO[Int]        = sleep(1)

      TestControl.executeEmbed {
        IOEvals.fullSequence[Int](List(fut1, failed1, failed2))
          .map(_ shouldBe (List(1), List(ex1, ex2)))
      }
    }

    "process list of failures" in {
      val ex1: Exception       = new Exception("ex1")
      val ex2: Exception       = new Exception("ex2")
      val failed1: IO[Nothing] = IO.raiseError(ex1)
      val failed2: IO[Nothing] = IO.raiseError(ex2)

      TestControl.executeEmbed {
        IOEvals.fullSequence[Int](List(failed1, failed2))
          .map(_ shouldBe (List(), List(ex1, ex2)))
      }
    }
  }

  "IOEvals.sequence" - {
    "behave as a sequence for IO" in {
      val xs: List[IO[Int]] = (1 to 3).map(sleep).toList

      val actual: IO[List[Int]]   = IOEvals.sequence(xs)
      val expected: IO[List[Int]] = xs.sequence

      TestControl.executeEmbed {
        for {
          actualResult   <- actual
          expectedResult <- expected
        } yield {
          actualResult shouldBe expectedResult
          actualResult shouldBe (1 to 3).toList
        }
      }
    }

    "work with empty lists" in {
      TestControl.executeEmbed {
        IOEvals.sequence[Int](Nil).map(_ shouldBe Nil)
      }
    }

    "correctly stop on failures" in {
      val exception = new Exception("Catch me if you can!")
      val xs        = (1 to 100)
        .map { elem => if (elem == 42) IO.raiseError(exception) else sleep(elem) }
        .toList

      TestControl.executeEmbed {
        IOEvals.sequence(xs)
          .attempt.map(_ shouldBe Left(exception))
      }
    }
  }

  "IOEvals.mapReduce" - {
    "asynchronously map and reduce the list and skip failures" in {
      val exception                 = new Exception("Catch me if you can!")
      val xs: List[Int]             = (1 to 10).toList
      val predicate: Int => Boolean = a => a == 5 || a == 6
      val map: Int => IO[Int]       = a =>
        if (predicate(a)) IO.raiseError(exception)
        else IO.pure(a * 2)

      val reduce: (Int, Int) => Int = _ + _

      TestControl.executeEmbed {
        IOEvals.mapReduce(xs, map, reduce)
          .map(_ shouldBe Some(xs.filterNot(predicate).map(_ * 2).sum))
      }
    }

    "return None if all elements mapping is failed" in {
      val exception                 = new Exception("Catch me if you can!")
      val xs: List[Int]             = (1 to 10).toList
      val map: Int => IO[Int]       = _ => IO.raiseError(exception)
      val reduce: (Int, Int) => Int = _ + _

      TestControl.executeEmbed {
        IOEvals.mapReduce(xs, map, reduce)
          .map(_ shouldBe None)
      }
    }

    "return None if all elements mapping is exception" in {
      val exception                 = new Exception("Catch me if you can!")
      val xs: List[Int]             = (1 to 10).toList
      val map: Int => IO[Int]       = _ => throw exception
      val reduce: (Int, Int) => Int = _ + _

      TestControl.executeEmbed {
        IOEvals.mapReduce(xs, map, reduce)
          .map(_ shouldBe None)
      }
    }
  }

  private def sleep(i: Int): IO[Int] =
    IO.sleep(1.seconds * i.toLong).as(i)

}
