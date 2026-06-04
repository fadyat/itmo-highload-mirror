package itmo.scala.task4

import cats.effect.{IO, IOApp}

import scala.concurrent.duration.DurationInt

object Main extends IOApp.Simple {

  private def program(boss: Boss[IO]): IO[Unit] =
    for {
      _ <- boss.start(Task[IO]("1", IO.sleep(5.seconds).as(Right(TaskSuccess(0))), 15.seconds))
      _ <- boss.start(Task[IO]("2", IO.sleep(3.seconds).as(Right(TaskSuccess(-1))), 2.seconds))
      x <- boss.start(Task[IO]("3", IO.sleep(7.seconds).as(Left(TaskError.ResultError("not okey"))), 10.seconds))
      _ <- x.get
      y <- boss.start(Task[IO]("4", IO.sleep(2.seconds).as(Right(TaskSuccess(100))), 1.seconds))
      _ <- y.get
    } yield ()

  override val run: IO[Unit] =
    Boss.make[IO]
      .use(program)

}
