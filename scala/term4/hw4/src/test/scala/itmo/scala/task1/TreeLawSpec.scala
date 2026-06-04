package itmo.scala.task1

import cats.Eq
import cats.kernel.laws.discipline.MonoidTests
import itmo.scala.task1.Tree.Leaf
import org.scalacheck.{Arbitrary, Gen}
import org.scalatest.funsuite.AnyFunSuite
import org.scalatestplus.scalacheck.Checkers
import org.typelevel.discipline.scalatest.FunSuiteDiscipline

class TreeLawSpec extends AnyFunSuite with FunSuiteDiscipline with Checkers {

  checkAll(
    name = "Tree.MonoidLaws",
    ruleSet = MonoidTests[Tree[Int]].monoid
  )

  implicit private def eq[A: Eq: Ordering]: Eq[Tree[A]] = {
    case (left, right) => Eq[List[A]].eqv(Tree.toList(left).sorted, Tree.toList(right).sorted)
  }

  implicit private def arbitrary[A: Arbitrary]: Arbitrary[Tree[A]] = {
    val leaf: Gen[Tree[A]] = Arbitrary.arbitrary[A].map(Leaf(_))
    val nil: Gen[Tree[A]]  = Gen.const(Tree.Nil)

    val treeGen: Gen[Tree[A]] = Gen.recursive[Tree[A]] { treeGen =>
      Gen.oneOf(
        leaf,
        leaf, // decrease chance of generating a branch
        nil,
        for {
          value <- Arbitrary.arbitrary[A]
          left  <- treeGen
          right <- treeGen
        } yield Tree.Branch(value, left, right)
      )
    }
    Arbitrary(treeGen)
  }
}
