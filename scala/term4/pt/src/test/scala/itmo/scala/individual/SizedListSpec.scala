package itmo.scala.individual

import itmo.scala.individual.SizedList._
import itmo.scala.individual.SizedList.SList._

class SizedListSpec extends munit.FunSuite {

  private def sameSize[N <: Nat, M <: Nat, A, B](a: SList[N, A], b: SList[M, B])(implicit ev: N =:= M): Unit = {
    val _ = (a, b, ev)
  }

  test("runtime length matches the compile-time one") {
    val l0  = empty[Int]
    val l5  = 1 :: 2 :: 3 :: 4 :: 5 :: empty[Int]
    val l11 = 1 :: 2 :: 3 :: 4 :: 5 :: 6 :: 7 :: 8 :: 9 :: 10 :: 11 :: empty[Int]
    assertEquals(l0.toList, List.empty[Int])
    assertEquals(l5.toList.size, 5)
    assertEquals(l11.toList.size, 11)
  }

  test("concat: 5 + 6 = 11 compiles and preserves order") {
    val a   = 1 :: 2 :: 3 :: 4 :: 5 :: empty[Int]
    val b   = 6 :: 7 :: 8 :: 9 :: 10 :: 11 :: empty[Int]
    val c   = a ++ b
    val ref = 0 :: 0 :: 0 :: 0 :: 0 :: 0 :: 0 :: 0 :: 0 :: 0 :: 0 :: empty[Int]
    sameSize(c, ref)
    assertEquals(c.toList, List(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11))
  }

  test("concat with empty: 0 + N = N and N + 0 = N") {
    val a  = 1 :: 2 :: 3 :: empty[Int]
    val la = empty[Int] ++ a
    val ra = a ++ empty[Int]
    sameSize(la, a)
    sameSize(ra, a)
    assertEquals(la.toList, List(1, 2, 3))
    assertEquals(ra.toList, List(1, 2, 3))
  }

  test("associativity: (2+3)+4 == 2+(3+4) == 9") {
    val a     = 1 :: 2 :: empty[Int]
    val b     = 3 :: 4 :: 5 :: empty[Int]
    val c     = 6 :: 7 :: 8 :: 9 :: empty[Int]
    val left  = (a ++ b) ++ c
    val right = a ++ (b ++ c)
    sameSize(left, right)
    assertEquals(left.toList, right.toList)
  }

  test("does not compile: trying to assign 5+6 to a list of length 10") {
    val errs = compileErrors(
      """
      val a: SList[_5, Int]  = 1 :: 2 :: 3 :: 4 :: 5 :: empty[Int]
      val b: SList[_6, Int]  = 6 :: 7 :: 8 :: 9 :: 10 :: 11 :: empty[Int]
      val bad: SList[_10, Int] = a ++ b
      """
    )
    assert(errs.nonEmpty, "expected a compile error but the snippet compiled")
  }

  test("does compile: 5 + 6 = 11 at the type level") {
    val a: SList[_5, Int]  = 1 :: 2 :: 3 :: 4 :: 5 :: empty[Int]
    val b: SList[_6, Int]  = 6 :: 7 :: 8 :: 9 :: 10 :: 11 :: empty[Int]
    val c: SList[_11, Int] = a ++ b
    assertEquals(c.toList.size, 11)
  }

  test("does not compile: sameSize on different lengths (5 and 6)") {
    val errs = compileErrors(
      """
      val a = 1 :: 2 :: 3 :: 4 :: 5 :: empty[Int]
      val b = 1 :: 2 :: 3 :: 4 :: 5 :: 6 :: empty[Int]
      sameSize(a, b)
      """
    )
    assert(errs.nonEmpty)
  }

  test("does not compile: cons yields Succ[N], not N") {
    val errs = compileErrors(
      """
      val a: SList[Zero, Int] = 1 :: empty[Int]
      """
    )
    assert(errs.nonEmpty)
  }

  test("`::` preserves insertion order in the underlying list") {
    val xs = 3 :: 2 :: 1 :: empty[Int]
    assertEquals(xs.toList, List(3, 2, 1))
  }

  test("length addition is commutative: N + M and M + N yield the same length type") {
    val a = 1 :: 2 :: 3 :: empty[Int]
    val b = 4 :: 5 :: empty[Int]
    sameSize(a ++ b, b ++ a)
    assertEquals((a ++ b).toList, List(1, 2, 3, 4, 5))
    assertEquals((b ++ a).toList, List(4, 5, 1, 2, 3))
  }

  test("SList is covariant in the element type") {
    val ints: SList[_2, Int] = 1 :: 2 :: empty[Int]
    val anys: SList[_2, Any] = ints
    assertEquals(anys.toList, List[Any](1, 2))
  }

  test("does not compile: outside code can't forge a lying SList via the raw constructor") {
    val errs = compileErrors(
      """
      val bad: SList[_5, Int] = new SList[_5, Int](List(1, 2, 3))
      """
    )
    assert(errs.nonEmpty)
  }
}
