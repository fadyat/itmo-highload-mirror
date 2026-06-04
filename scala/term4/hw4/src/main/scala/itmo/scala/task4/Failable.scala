package itmo.scala.task4

sealed trait Failable[+E, +A]

object Failable {
  final case class Success[+A](value: A) extends Failable[Nothing, A]
  final case class Failure[+E](error: E) extends Failable[E, Nothing]

  def fromEither[E, A](either: Either[E, A]): Failable[E, A] = either match {
    case Right(out) => Success(out)
    case Left(err)  => Failure(err)
  }
}
