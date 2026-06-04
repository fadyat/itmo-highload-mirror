package itmo.scala

object DivisionByZeroException extends ArithmeticException("Division by zero exception") {
  def raise: Nothing = throw DivisionByZeroException
}
