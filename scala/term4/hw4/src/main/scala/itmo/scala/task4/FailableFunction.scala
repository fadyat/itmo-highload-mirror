package itmo.scala.task4

import cats.MonadError
import itmo.scala.task4.Failable.Failure
import itmo.scala.task4.Failable.Success
import scala.annotation.tailrec

trait FailableFunction[-In, +Err, +Out] {
  def apply(in: In): Failable[Err, Out]
}

object FailableFunction {
  def fromFunction[In, Out](f: In => Out): FailableFunction[In, Nothing, Out] =
    (in: In) => Failable.Success(f(in))

  def fail[Err](err: Err): FailableFunction[Any, Err, Nothing] =
    _ => Failable.Failure(err)

  def success[Out](out: Out): FailableFunction[Any, Nothing, Out] =
    _ => Failable.Success(out)

  def fromEither[In, Err, Out](f: In => Either[Err, Out]): FailableFunction[In, Err, Out] =
    (in: In) => Failable.fromEither(f(in))

  implicit def monadErrorFailableFunction[In, Err]: MonadError[FailableFunction[In, Err, *], Err] =
    new MonadError[FailableFunction[In, Err, *], Err] {

      override def raiseError[A](e: Err): FailableFunction[In, Err, A] =
        _ => Failable.Failure(e)

      override def handleErrorWith[A](fa: FailableFunction[In, Err, A])(
          f: Err => FailableFunction[In, Err, A]
      ): FailableFunction[In, Err, A] =
        (in: In) =>
          fa(in) match {
            case s @ Success(_) => s
            case Failure(e)     => f(e)(in)
          }

      override def pure[A](x: A): FailableFunction[In, Err, A] =
        _ => Failable.Success(x)

      override def flatMap[A, B](fa: FailableFunction[In, Err, A])(
          f: A => FailableFunction[In, Err, B]
      ): FailableFunction[In, Err, B] =
        (in: In) =>
          fa(in) match {
            case Success(x)     => f(x)(in)
            case e @ Failure(_) => e
          }

      override def tailRecM[A, B](a: A)(
          f: A => FailableFunction[In, Err, Either[A, B]]
      ): FailableFunction[In, Err, B] =
        (in: In) => {
          @tailrec
          def loop(current: A): Failable[Err, B] =
            f(current)(in) match {
              case e @ Failure(_)        => e
              case Success(Left(next))   => loop(next)
              case Success(Right(value)) => Success(value)
            }

          loop(a)
        }
    }
}
