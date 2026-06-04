package itmo.scala.hw2.task3

sealed trait Subject

object Subject {
  case object Math       extends Subject
  case object Physics    extends Subject
  case object Chemistry  extends Subject
  case object Biology    extends Subject
  case object History    extends Subject
  case object English    extends Subject
  case object Literature extends Subject
  case object Art        extends Subject
  case object Music      extends Subject
  case object Sport      extends Subject
}
