package itmo.scala.hw1.servicelevel

import Level.Economy

class ServiceLevelAdvance[L <: Economy] {
  def advance[N <: L]: ServiceLevelAdvance[N] = new ServiceLevelAdvance[N]
}
