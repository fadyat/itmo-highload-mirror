package itmo.scala.task3

import cats.Show
import cats.effect.std.{Console, Queue}

import java.nio.charset.Charset

class TestConsole[F[_]](
    val input: Queue[F, String],
    val output: Queue[F, String]
) extends Console[F] {

  override def readLineWithCharset(
      charset: Charset
  ): F[String] =
    input.take

  override def println[A](a: A)(implicit
      S: Show[A]
  ): F[Unit] =
    output.offer(S.show(a))

  override def print[A](a: A)(implicit
      S: Show[A]
  ): F[Unit] = ???

  override def error[A](a: A)(implicit
      S: Show[A]
  ): F[Unit] = ???

  override def errorln[A](a: A)(implicit
      S: Show[A]
  ): F[Unit] = ???

}
