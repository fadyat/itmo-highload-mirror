package itmo.scala

package object task2 {

  implicit val showComplex: Show[ComplexNumber] = new Show[ComplexNumber] {
    def show(c: ComplexNumber): String = {
      (c.real, c.imaginary) match {
        case (0, 0)          => "0"
        case (r, 0)          => s"$r"
        case (0, i)          => s"${i}i"
        case (r, i) if i > 0 => s"$r + ${i}i"
        case (r, i)          => s"$r - ${-i}i"
      }
    }
  }

}
