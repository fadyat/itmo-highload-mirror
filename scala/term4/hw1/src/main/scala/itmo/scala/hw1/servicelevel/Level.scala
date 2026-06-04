package itmo.scala.hw1.servicelevel

object Level {
  class Economy
  class UpgradedEconomy extends Economy
  class Special1b       extends UpgradedEconomy
  class ExtendedEconomy extends Economy
  class Business        extends ExtendedEconomy
  class Elite           extends Business
  class Platinum        extends Business
}
