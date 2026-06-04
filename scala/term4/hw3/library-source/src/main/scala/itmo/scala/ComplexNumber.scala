package itmo.scala

final case class ComplexNumber(real: Double, imaginary: Double) {
  def *(other: ComplexNumber): ComplexNumber =
    ComplexNumber(
      (real * other.real) - (imaginary * other.imaginary),
      (real * other.imaginary) + (imaginary * other.real)
    )

  def +(other: ComplexNumber): ComplexNumber =
    ComplexNumber(
      real + other.real,
      imaginary + other.imaginary
    )

  def ~=(other: ComplexNumber): Boolean = (real - other.real).abs < 1e-6 && (imaginary - other.imaginary).abs < 1e-6
}
