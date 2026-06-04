package itmo.scala.task1

import cats.Monoid

final case class Last[+T](value: Option[T])

object Last {
  implicit def monoid[A]: Monoid[Last[A]] = new Monoid[Last[A]] {
    override def empty: Last[A]                           = Last(None)
    override def combine(x: Last[A], y: Last[A]): Last[A] = Last(y.value.orElse(x.value))
  }
}
