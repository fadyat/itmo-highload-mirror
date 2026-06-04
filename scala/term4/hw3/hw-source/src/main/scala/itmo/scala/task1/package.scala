package itmo.scala

package object task1 {

  implicit class ComplexNumberExtension(private val c: ComplexNumber) extends AnyVal {
    def -(other: ComplexNumber): ComplexNumber =
      ComplexNumber(
        c.real - other.real,
        c.imaginary - other.imaginary
      )

    def /(other: ComplexNumber): ComplexNumber = {
      if (other ~= ComplexNumber(0, 0)) DivisionByZeroException.raise

      val denom = other.real * other.real + other.imaginary * other.imaginary
      ComplexNumber(
        (c.real * other.real + c.imaginary * other.imaginary) / denom,
        (c.imaginary * other.real - c.real * other.imaginary) / denom
      )
    }
  }

  implicit def numericToComplex[T](x: T)(implicit num: Numeric[T]): ComplexNumber =
    ComplexNumber(num.toDouble(x), 0)

  implicit class NumericExtension[T](val n: T)(implicit num: Numeric[T]) {
    private def toComplex: ComplexNumber = ComplexNumber(num.toDouble(n), 0)
    def i: ComplexNumber                 = ComplexNumber(0, num.toDouble(n))

    def -(c: ComplexNumber): ComplexNumber = toComplex - c
    def /(c: ComplexNumber): ComplexNumber = toComplex / c
  }

}
