package itmo.scala.task1

import cats.Monoid

import scala.annotation.tailrec
import scala.{Nil => ListNil}

sealed trait Tree[+T]

object Tree {
  final case class Leaf[+T](value: T)                                  extends Tree[T]
  final case class Branch[+T](value: T, left: Tree[T], right: Tree[T]) extends Tree[T]
  case object Nil                                                      extends Tree[Nothing]

  def toList[T](tree: Tree[T]): List[T] = {
    @tailrec
    def loop(acc: List[T], stack: List[Tree[T]]): List[T] = stack match {
      case ListNil      => acc
      case head :: tail =>
        head match {
          case Leaf(value)                => loop(acc :+ value, tail)
          case Branch(value, left, right) => loop(acc :+ value, right :: left :: tail)
          case Nil                        => loop(acc, tail)
        }
    }

    loop(List.empty, List(tree))
  }

  implicit def monoid[A]: Monoid[Tree[A]] = new Monoid[Tree[A]] {
    override def empty: Tree[A]                           = Nil
    override def combine(x: Tree[A], y: Tree[A]): Tree[A] = (x, y) match {
      case (Nil, t)                 => t
      case (t, Nil)                 => t
      case (Leaf(v), other)         => Branch(v, Nil, other)
      case (Branch(v, l, r), other) => Branch(v, l, combine(r, other))
    }
  }
}
