package itmo.scala.task1

import cats.implicits.catsSyntaxSemigroup
import cats.kernel.Monoid
import itmo.scala.task1.Tree.{Branch, Leaf, Nil}
import itmo.scala.task1.syntax._
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers

class TreeSpec extends AnyFlatSpec with Matchers {

  "Semigroup for Tree" should "be associative for branch" in {
    Tree.toList(branchOne |+| branchTwo |+| branchThree).sorted shouldBe
      Tree.toList(branchOne |+| (branchTwo |+| branchThree)).sorted

    Tree.toList(branchOne |+| branchThree |+| branchTwo).sorted shouldBe
      Tree.toList(branchOne |+| (branchThree |+| branchTwo)).sorted

    Tree.toList(branchTwo |+| branchThree |+| branchOne).sorted shouldBe
      Tree.toList(branchTwo |+| (branchThree |+| branchOne)).sorted

    Tree.toList(branchTwo |+| branchOne |+| branchThree).sorted shouldBe
      Tree.toList(branchTwo |+| (branchOne |+| branchThree)).sorted

    Tree.toList(branchThree |+| branchOne |+| branchTwo).sorted shouldBe
      Tree.toList(branchThree |+| (branchOne |+| branchTwo)).sorted

    Tree.toList(branchThree |+| branchTwo |+| branchOne).sorted shouldBe
      Tree.toList(branchThree |+| (branchTwo |+| branchOne)).sorted
  }

  it should "be associative for leaf" in {
    Tree.toList(leafOne |+| leafTwo |+| leafThree).sorted shouldBe
      Tree.toList(leafOne |+| (leafTwo |+| leafThree)).sorted
  }

  it should "be associative for Nil" in {
    Tree.toList(leafOne |+| leafTwo |+| Nil).sorted shouldBe Tree.toList(leafOne |+| (leafTwo |+| Nil)).sorted
    Tree.toList(branchOne |+| branchTwo |+| Nil).sorted shouldBe Tree.toList(branchOne |+| (branchTwo |+| Nil)).sorted
  }

  it should "be associative for leaf and branch" in {
    Tree.toList(leafOne |+| leafTwo |+| branchOne).sorted shouldBe
      Tree.toList(leafOne |+| (leafTwo |+| branchOne)).sorted

    Tree.toList(leafOne |+| branchTwo |+| leafThree).sorted shouldBe
      Tree.toList(leafOne |+| (branchTwo |+| leafThree)).sorted

    Tree.toList(branchThree |+| leafTwo |+| leafThree).sorted shouldBe
      Tree.toList(branchThree |+| (leafTwo |+| leafThree)).sorted

    Tree.toList(branchOne |+| leafTwo |+| branchTwo).sorted shouldBe
      Tree.toList(branchOne |+| (leafTwo |+| branchTwo)).sorted
  }

  "Monoid for Tree" should "have monoid instance" in {
    empty[Nothing] shouldBe Nil
  }

  it should "right combine with empty" in {
    leafInt |+| empty[Int] shouldBe leafInt
  }

  it should "left combine with empty" in {
    empty[Int] |+| leafInt shouldBe leafInt
  }

  "Tree.fold syntax" should "fold all elements in Tree" in {
    branchThree.fold shouldBe 12
  }

  it should "work for empty tree" in {
    empty[Int].fold
  }

  private def empty[T]: Tree[T] =
    Monoid[Tree[T]].empty

  private lazy val leafInt: Tree[Int]     = Leaf(1)
  private lazy val leafOne: Tree[Int]     = Leaf(1)
  private lazy val leafTwo: Tree[Int]     = Leaf(2)
  private lazy val leafThree: Tree[Int]   = Leaf(3)
  private lazy val branchOne: Tree[Int]   = Branch(1, leafTwo, leafThree)
  private lazy val branchTwo: Tree[Int]   = Branch(2, leafOne, Nil)
  private lazy val branchThree: Tree[Int] = Branch(3, branchOne, branchTwo)
}
