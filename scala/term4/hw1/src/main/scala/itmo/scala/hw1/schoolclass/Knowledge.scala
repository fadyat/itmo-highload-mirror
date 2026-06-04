package itmo.scala.hw1.schoolclass

object Knowledge {
  class KnowNothing
  class Aggressive     extends KnowNothing
  class KnowSomething  extends KnowNothing
  class PoorlyEducated extends KnowSomething
  class Normal         extends PoorlyEducated
  class Enlightened    extends Normal
  class Genius         extends Enlightened
}
