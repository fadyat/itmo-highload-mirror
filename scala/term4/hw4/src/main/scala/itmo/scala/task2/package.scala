package itmo.scala

import cats.{Functor, Semigroup}
import cats.syntax.all._

package object task2 {

  def const[F[_]: Functor, T](const: T)(other: F[_]): F[T]               = other.map(_ => const)
  def fst[F[_]: Functor, T](other: F[(T, _)]): F[T]                      = other.map(_._1)
  def snd[F[_]: Functor, T](other: F[(_, T)]): F[T]                      = other.map(_._2)
  def tupled[F[_]: Functor, A, B](other: F[A], f: A => B): F[(A, B)]     = other.map(x => (x, f(x)))
  def times[F[_]: Functor, A: Semigroup](other: F[A], repeat: Int): F[A] = other.map(_.combineN(repeat))

  // do not change
  implicit final class RepeatSyntax[F[_], A](private val self: F[A]) extends AnyVal {
    def times(repeat: Int)(implicit F: Functor[F], S: Semigroup[A]): F[A] =
      task2.times[F, A](self, repeat)
  }

  // do not change
  implicit final class TupledSyntax[F[_], A](private val self: F[A]) extends AnyVal {
    def tupled[B](f: A => B)(implicit F: Functor[F]): F[(A, B)] =
      task2.tupled(self, f)
  }
}
