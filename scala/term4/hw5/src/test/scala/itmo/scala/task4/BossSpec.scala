package itmo.scala.task4

import cats.effect.testing.scalatest.AsyncIOSpec
import cats.effect.testkit.TestControl
import cats.effect.{Deferred, IO, Ref}
import org.scalatest.Assertion
import org.scalatest.freespec.AsyncFreeSpec
import org.scalatest.matchers.should.Matchers

import scala.concurrent.duration.DurationInt

class BossSpec extends AsyncFreeSpec with AsyncIOSpec with Matchers {

  "Boss.start" - {
    "wait success task" in {
      implicit val noopReporter: Reporter[IO] = Reporter.noop[IO]

      TestControl.executeEmbed[Assertion] {
        Boss.make[IO].use { boss =>
          boss.start(Task[IO]("1", IO.sleep(5.seconds).as(Right(TaskSuccess(0))), 15.seconds))
            .flatMap(_.get)
            .map(_ shouldBe Right(TaskSuccess(0)))
        }
      }
    }

    "wait 2 success task" in {
      implicit val noopReporter: Reporter[IO] = Reporter.noop[IO]

      val task1 = Task[IO]("1", IO.sleep(5.seconds).as(Right(TaskSuccess(1))), 15.seconds)
      val task2 = Task[IO]("2", IO.sleep(4.seconds).as(Right(TaskSuccess(2))), 15.seconds)

      TestControl.executeEmbed[Assertion] {
        Boss.make[IO].use { boss =>
          for {
            x    <- boss.start(task1)
            y    <- boss.start(task2)
            res1 <- x.get
            res2 <- y.get
          } yield {
            res1 shouldBe Right(TaskSuccess(1))
            res2 shouldBe Right(TaskSuccess(2))
          }
        }
      }
    }

    "call 2 task in parallel" in {
      implicit val noopReporter: Reporter[IO] = Reporter.noop[IO]

      TestControl.executeEmbed[Assertion] {
        for {
          calls <- IO.ref(3)
          task1 = Task[IO](
            "1",
            calls.update(_ + 1) >>
              IO.sleep(5.seconds) >>
              calls.updateAndGet(_ * 2).map(n => Right(TaskSuccess(n))),
            15.seconds
          )
          task2 = Task[IO](
            "2",
            IO.sleep(3.seconds) >>
              calls.updateAndGet(_ + 1).map(n => Right(TaskSuccess(n))),
            15.seconds
          )
          _ <- Boss.make[IO].use { boss =>
            for {
              x    <- boss.start(task1)
              y    <- boss.start(task2)
              res1 <- x.get
              res2 <- y.get
            } yield {
              res1 shouldBe Right(TaskSuccess(10))
              res2 shouldBe Right(TaskSuccess(5))
            }
          }
        } yield succeed
      }
    }

    "call success reporter" in {
      val task: Task[IO] = Task[IO]("1", IO.sleep(5.seconds).as(Right(TaskSuccess(0))), 15.seconds)

      TestControl.executeEmbed[Assertion] {
        for {
          implicit0(reporter: TestReporter[IO]) <- testReporter

          result      <- Boss.make[IO].use(_.start(task).flatMap(_.get))
          startCall   <- reporter.startRef.get
          successCall <- reporter.successRef.get
          errorCall   <- reporter.errorRef.get
        } yield {
          result shouldBe Right(TaskSuccess(0))

          startCall shouldBe List(task)
          successCall shouldBe List(task -> TaskSuccess(0))
          errorCall shouldBe Nil
        }
      }
    }

    "cancel on timeout" in {
      implicit val noopReporter: Reporter[IO] = Reporter.noop[IO]

      TestControl.executeEmbed[Assertion] {
        Boss.make[IO].use { boss =>
          boss.start(Task[IO]("1", IO.sleep(20.seconds).as(Right(TaskSuccess(0))), 15.seconds))
            .flatMap(_.get)
            .map(_ shouldBe Left(TaskError.Timeout))
        }
      }
    }

    "call reporter on cancel" in {
      val task: Task[IO] = Task[IO]("1", IO.sleep(20.seconds).as(Right(TaskSuccess(0))), 15.seconds)

      TestControl.executeEmbed[Assertion] {
        for {
          implicit0(reporter: TestReporter[IO]) <- testReporter

          result      <- Boss.make[IO].use(_.start(task).flatMap(_.get))
          startCall   <- reporter.startRef.get
          successCall <- reporter.successRef.get
          errorCall   <- reporter.errorRef.get
        } yield {
          result shouldBe Left(TaskError.Timeout)

          startCall shouldBe List(task)
          successCall shouldBe Nil
          errorCall shouldBe List(task -> TaskError.Timeout)
        }
      }
    }

    "catch error" in {
      implicit val noopReporter: Reporter[IO] = Reporter.noop[IO]
      val exception                           = new Exception("Catch me if you can!")

      TestControl.executeEmbed[Assertion] {
        Boss.make[IO].use { boss =>
          boss.start(Task[IO]("1", IO.raiseError(exception), 15.seconds))
            .flatMap(_.get)
            .map(_ shouldBe Left(TaskError.UnexpectedError(exception)))
        }
      }
    }

    "call reporter on unexpected error" in {
      val exception      = new Exception("Catch me if you can!")
      val task: Task[IO] = Task[IO]("1", IO.raiseError(exception), 15.seconds)

      TestControl.executeEmbed[Assertion] {
        for {
          implicit0(reporter: TestReporter[IO]) <- testReporter

          result      <- Boss.make[IO].use(_.start(task).flatMap(_.get))
          startCall   <- reporter.startRef.get
          successCall <- reporter.successRef.get
          errorCall   <- reporter.errorRef.get
        } yield {
          result shouldBe Left(TaskError.UnexpectedError(exception))

          startCall shouldBe List(task)
          successCall shouldBe Nil
          errorCall shouldBe List(task -> TaskError.UnexpectedError(exception))
        }
      }
    }

    "call reporter on expected error" in {
      val task = Task[IO]("1", IO.pure(Left(TaskError.ResultError("not okey"))), 1.seconds)

      TestControl.executeEmbed[Assertion] {
        for {
          implicit0(reporter: TestReporter[IO]) <- testReporter

          result      <- Boss.make[IO].use(_.start(task).flatMap(_.get))
          startCall   <- reporter.startRef.get
          successCall <- reporter.successRef.get
          errorCall   <- reporter.errorRef.get
        } yield {
          result shouldBe Left(TaskError.ResultError("not okey"))

          startCall shouldBe List(task)
          successCall shouldBe Nil
          errorCall shouldBe List(task -> TaskError.ResultError("not okey"))
        }
      }
    }

    "start task only once" in {
      TestControl.executeEmbed[Assertion] {
        for {
          count                                 <- IO.ref(0)
          implicit0(reporter: TestReporter[IO]) <- testReporter

          task = Task[IO]("1", count.update(_ + 1).flatMap(_ => IO.sleep(5.second)).as(Right(TaskSuccess(0))), 6.second)

          result      <- Boss.make[IO].use(_.start(task).flatMap(await => IO.race(await.get, await.get).map(_.merge)))
          startCall   <- reporter.startRef.get
          successCall <- reporter.successRef.get
          errorCall   <- reporter.errorRef.get
          taskStartCount <- count.get
        } yield {
          result shouldBe Right(TaskSuccess(0))

          taskStartCount shouldBe 1

          startCall shouldBe List(task)
          successCall shouldBe List(task -> TaskSuccess(0))
          errorCall shouldBe Nil
        }
      }
    }

    "cancel all tasks after close resource" in {
      TestControl.executeEmbed[Assertion] {
        for {
          waiter                                <- Deferred[IO, Unit]
          implicit0(reporter: TestReporter[IO]) <- testReporter

          task = Task[IO]("1", waiter.complete(()) >> IO.sleep(5.seconds).as(Right(TaskSuccess(0))), 10.seconds)
          // we use `waiter` no ensure that task will be started before we close boss Resource
          await <- Boss.make[IO].use(_.start(task).flatTap(_ => waiter.get))

          startCall   <- reporter.startRef.get
          successCall <- reporter.successRef.get
          errorCall   <- reporter.errorRef.get

          awaitResult <- await.get

          startCallCheck   <- reporter.startRef.get
          successCallCheck <- reporter.successRef.get
          errorCallCheck   <- reporter.errorRef.get
        } yield {
          awaitResult shouldBe Left(TaskError.Timeout)

          startCall shouldBe List(task)
          successCall shouldBe Nil
          errorCall shouldBe List(task -> TaskError.Timeout)

          startCallCheck shouldBe startCall
          successCallCheck shouldBe successCall
          errorCallCheck shouldBe errorCall
        }
      }
    }

    "not memoize tasks" in {
      TestControl.executeEmbed[Assertion] {
        for {
          count                                 <- IO.ref(0)
          implicit0(reporter: TestReporter[IO]) <- testReporter

          task = Task[IO](
            "1",
            count.updateAndGet(_ + 1)
              .flatTap(n => IO.sleep(n.second))
              .map(n => Right(TaskSuccess(n))),
            6.second
          )

          _ <- Boss.make[IO].use { boss =>
            for {
              await1 <- boss.start(task)
              _      <- IO.sleep(1.milli)
              // we use TestControl.executeEmbed, so it's deterministic
              await2 <- boss.start(task)

              result1 <- await1.get
              result2 <- await2.get
            } yield {
              result1 shouldBe Right(TaskSuccess(1))
              result2 shouldBe Right(TaskSuccess(2))
            }
          }
          startCall      <- reporter.startRef.get
          successCall    <- reporter.successRef.get
          errorCall      <- reporter.errorRef.get
          taskStartCount <- count.get
        } yield {
          taskStartCount shouldBe 2

          startCall shouldBe List(task, task)
          successCall shouldBe List(task -> TaskSuccess(1), task -> TaskSuccess(2))
          errorCall shouldBe Nil
        }
      }
    }

  }

  private def testReporter: IO[TestReporter[IO]] = for {
    startRef   <- IO.ref(List.empty[Task[IO]])
    errorRef   <- IO.ref(List.empty[(Task[IO], TaskError)])
    successRef <- IO.ref(List.empty[(Task[IO], TaskSuccess)])
  } yield new TestReporter(startRef, errorRef, successRef)

  private class TestReporter[F[_]](
      val startRef: Ref[F, List[Task[F]]],
      val errorRef: Ref[F, List[(Task[F], TaskError)]],
      val successRef: Ref[F, List[(Task[F], TaskSuccess)]]
  ) extends Reporter[F] {
    override def start(task: Task[F]): F[Unit]                         = startRef.update(_ :+ task)
    override def error(task: Task[F], err: TaskError): F[Unit]         = errorRef.update(_ :+ (task -> err))
    override def success(task: Task[F], success: TaskSuccess): F[Unit] = successRef.update(_ :+ (task -> success))
  }
}
