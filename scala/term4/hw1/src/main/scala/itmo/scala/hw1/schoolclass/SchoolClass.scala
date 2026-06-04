package itmo.scala.hw1.schoolclass

import itmo.scala.hw1.schoolclass.Knowledge.{Genius, KnowNothing}

class SchoolClass[K >: Genius <: KnowNothing](seq: Seq[K]) {
  def accept[N >: K <: KnowNothing](s: Seq[N]): SchoolClass[N] = new SchoolClass[N](seq ++ s)
}
