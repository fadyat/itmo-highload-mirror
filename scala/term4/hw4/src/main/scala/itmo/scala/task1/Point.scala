package itmo.scala.task1

import cats.Monoid
import cats.syntax.all._

final case class Point[+T](x: T, y: T, z: T)

object Point {
  implicit def monoid[A: Monoid]: Monoid[Point[A]] = new Monoid[Point[A]] {
    override def empty: Point[A]                             = Point(Monoid[A].empty, Monoid[A].empty, Monoid[A].empty)
    override def combine(x: Point[A], y: Point[A]): Point[A] = Point(x.x |+| y.x, x.y |+| y.y, x.z |+| y.z)
  }
}
