package itmo.scala.hw2.task3

sealed trait Grade

object Grade {
  case object A  extends Grade
  case object B  extends Grade
  case object C  extends Grade
  case object D  extends Grade
  case object E  extends Grade
  case object FX extends Grade

  val min: (Grade, Grade) => Grade = {
    case (FX, _) | (_, FX) => FX
    case (E, _) | (_, E)   => E
    case (D, _) | (_, D)   => D
    case (C, _) | (_, C)   => C
    case (B, _) | (_, B)   => B
    case (A, _) | (_, A)   => A
  }

  val max: (Grade, Grade) => Grade = {
    case (A, _) | (_, A)   => A
    case (B, _) | (_, B)   => B
    case (C, _) | (_, C)   => C
    case (D, _) | (_, D)   => D
    case (E, _) | (_, E)   => E
    case (FX, _) | (_, FX) => FX
  }
}
