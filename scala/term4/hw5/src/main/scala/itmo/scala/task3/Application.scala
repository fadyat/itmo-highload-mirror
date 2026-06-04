package itmo.scala.task3

import cats.effect.std.Console
import cats.effect.{IO, IOApp}
import cats.syntax.all._
import cats.{FlatMap, MonadThrow}
import itmo.scala.task3.env.LocalEnv

object Application extends IOApp.Simple {

  override val run: IO[Unit] = helloWorld[IO]()

  private def helloWorld[F[_]: Console: FlatMap](): F[Unit] =
    for {
      _    <- Console[F].println("What your name?")
      name <- Console[F].readLine
      _    <- Console[F].println(s"Hello $name!")
    } yield ()

  def consoleApplication[F[_]: Console: MonadThrow: LocalEnv]: F[Unit] = {
    def loop: F[Unit] =
      Console[F].readLine.attempt.flatMap {
        case Right("exit") => MonadThrow[F].unit
        case Right(line)   => handle(line) *> loop
        case Left(_)       => MonadThrow[F].unit
      }

    def handle(line: String): F[Unit] =
      line.split("\\s+").toList match {
        case "get" :: key :: Nil =>
          LocalEnv[F].get(key).flatMap {
            case Some(v) => Console[F].println(s"'$key' '$v'")
            case None    => Console[F].println("Not found")
          }

        case "set" :: key :: value :: Nil =>
          LocalEnv[F].set(key, value) *> Console[F].println(s"'$key' '$value'")

        case "list" :: Nil =>
          LocalEnv[F].entries.flatMap { entries =>
            entries.toList
              .sortBy(_._1)
              .traverse_ { case (k, v) => Console[F].println(s"'$k' '$v'") }
          }

        case _ =>
          Console[F].println("Unknown command")
      }

    loop
  }
}
