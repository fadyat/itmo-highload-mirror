package itmo.scala.task2

import cats.effect.IO

object IOEvals {

  /** Реализуйте аналог [[itmo.scala.task1.Futures.foldF]] для [[IO]]
    */
  def foldF[A, B](in: List[IO[A]], zero: B, op: (B, A) => B): IO[B] =
    in.foldLeft(IO.pure(zero)) { (accIO, ioa) =>
      for {
        acc <- accIO
        a   <- ioa
      } yield op(acc, a)
    }

  /** Реализуйте аналог [[itmo.scala.task1.Futures.flatFoldF]] для [[IO]]
    */
  def flatFoldF[A, B](in: List[IO[A]], zero: B, op: (B, A) => IO[B]): IO[B] =
    in.foldLeft(IO.pure(zero)) { (accIO, ioa) =>
      for {
        acc <- accIO
        a   <- ioa
        b   <- op(acc, a)
      } yield b
    }

  /** Реализуйте аналог [[itmo.scala.task1.Futures.fullSequence]] для [[IO]]
    */
  def fullSequence[A](evals: List[IO[A]]): IO[(List[A], List[Throwable])] =
    evals
      .foldLeft(IO.pure((List.empty[A], List.empty[Throwable]))) {
        (accIO, io) =>
          for {
            (as, es) <- accIO
            either   <- io.attempt
          } yield either match {
            case Right(a) => (a :: as, es)
            case Left(e)  => (as, e :: es)
          }
      }
      .map { case (as, es) => (as.reverse, es.reverse) }

  /** Реализуйте sequence c помощью метода [[IO.traverse]]
    */
  def sequence[A](in: List[IO[A]]): IO[List[A]] = IO.traverse(in)(identity)

  /** Реализуйте аналог [[itmo.scala.task1.Futures.mapReduce]] для [[IO]]
    */
  def mapReduce[A, B, B1 >: B](in: List[A], map: A => IO[B], reduce: (B1, B1) => B1): IO[Option[B1]] =
    IO.parTraverse(in)(a => IO.defer(map(a)).attempt.map(_.toOption))
      .map(_.flatten.reduceLeftOption(reduce))

}
