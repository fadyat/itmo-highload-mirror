package itmo.scala.task3

import itmo.scala.ComplexNumber

trait Combine[T] {
  def combine(left: T, right: T): T
}

object Combine {
  def apply[T](implicit combine: Combine[T]): Combine[T] = combine

  def combine[T: Combine](left: T, right: T): T                  = Combine[T].combine(left, right)
  def combineAll[T: Combine](head: T, tail: T*): T               = tail.foldLeft(head)(combine)
  def maybeCombine[T: Combine](value: T, valueOpt: Option[T]): T = valueOpt.fold(value)(combine(value, _))

  implicit val combineInt: Combine[Int] = new Combine[Int] {
    override def combine(left: Int, right: Int): Int = left + right
  }

  implicit val combineString: Combine[String] = new Combine[String] {
    override def combine(left: String, right: String): String = left.concat(right)
  }

  implicit val combineComplexNumber: Combine[ComplexNumber] = new Combine[ComplexNumber] {
    override def combine(left: ComplexNumber, right: ComplexNumber): ComplexNumber = left + right
  }
}
