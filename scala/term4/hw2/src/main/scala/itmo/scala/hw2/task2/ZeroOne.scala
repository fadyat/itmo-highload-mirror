package itmo.scala.hw2.task2

sealed trait ZeroOne

object ZeroOne {
  case object Zero extends ZeroOne
  case object One  extends ZeroOne
}
