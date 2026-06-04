package itmo.scala.task0

import cats.effect.IO

object Factorial {

  def factorial(n: Int): IO[BigInt] =
    if (n <= 1) IO.pure(1)
    else for { prev <- IO.defer(factorial(n - 1)) } yield prev * n

}
