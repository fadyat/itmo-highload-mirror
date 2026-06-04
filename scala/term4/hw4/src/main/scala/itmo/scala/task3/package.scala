package itmo.scala

import cats.syntax.all._
import cats.{Monad, MonadError}

package object task3 {

  def retry[F[_]: Monad, A](start: A)(f: A => F[A]): F[A] =
    Monad[F].tailRecM(start)(a => f(a).map(Left(_)))

  implicit class RetrySyntax[F[_], A](self: => F[A]) {
    def retry(implicit M: MonadError[F, _]): F[A] = {
      M.tailRecM(()) { _ =>
        M.handleError(
          M.map(self)(Right(_): Either[Unit, A])
        )(_ => Left(()))
      }
    }
  }
}
