package itmo.scala

trait Show[A] {
  def show(value: A): String
}

object Show {
  def apply[A](implicit show: Show[A]): Show[A] = show

  def show[A: Show](value: A): String = Show[A].show(value)

  implicit val showInt: Show[Int]         = _.toString
  implicit val showString: Show[String]   = identity
  implicit val showBoolean: Show[Boolean] = _.toString
  implicit val showDouble: Show[Double]   = _.toString
  implicit val showChar: Show[Char]       = _.toString
  implicit val showLong: Show[Long]       = _.toString + "L"
  implicit val showFloat: Show[Float]     = _.toString
  implicit val showShort: Show[Short]     = _.toString
  implicit val showByte: Show[Byte]       = _.toString
  implicit val showUnit: Show[Unit]       = _ => "()"

  implicit def showOption[A: Show]: Show[Option[A]] = {
    case Some(value) => s"Some(${Show[A].show(value)})"
    case None        => "None"
  }

  implicit def showList[A: Show]: Show[List[A]] =
    _.map(Show[A].show).mkString("[", ", ", "]")
}
