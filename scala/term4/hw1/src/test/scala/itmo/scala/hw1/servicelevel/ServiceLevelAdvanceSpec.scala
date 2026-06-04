package itmo.scala.hw1.servicelevel

import itmo.scala.hw1.servicelevel.Level._
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers

class ServiceLevelAdvanceSpec extends AnyFlatSpec with Matchers {

  "ServiceLevelAdvance" should "correctly determine ServiceLevel type as Special1b" in {
    assertCompiles(
      """
        |val special1bLevel: ServiceLevelAdvance[Special1b] =
        |  new ServiceLevelAdvance[Economy]
        |    .advance[UpgradedEconomy]
        |    .advance[Special1b]
        |
        |""".stripMargin
    )
  }

  it should "correctly determine ServiceLevel type as Platinum" in {
    assertCompiles(
      """
        |val specialPlLevel: ServiceLevelAdvance[Platinum] =
        |  new ServiceLevelAdvance[Economy]
        |    .advance[Business]
        |    .advance
        |""".stripMargin
    )
  }

  it should "implement compile time checking" in {
    assertDoesNotCompile(
      """
        |val eliteLevel =
        |  new ServiceLevelAdvance[Economy]
        |    .advance[Elite]
        |    .advance[Business]
        |""".stripMargin
    )
  }

}
