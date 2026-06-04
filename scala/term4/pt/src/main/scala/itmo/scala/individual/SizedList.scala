package itmo.scala.individual

/** A list whose length is encoded in its type.
  *
  * Length is a Peano numeral (`Zero` / `Succ[N]`), so size checks run at compile time. Concatenation is the showcase:
  * `SList[N, A] ++ SList[M, A]` returns `SList[N + M, A]`, and any mismatch with the expected length is rejected.
  *
  * Implementation: `SList` is a thin wrapper over a plain `List[A]`. The type-level size is proved by the phantom
  * [[SizedList.Plus]] witness; the runtime value delegates to `List.++`, so concatenation is stack-safe and O(N). The
  * private constructor keeps the invariant sound — outside code can never build an `SList[_5, A]` from a three-element
  * list, so the size in the type and the size of the underlying list cannot drift apart.
  */
object SizedList {

  sealed trait Nat
  sealed trait Zero           extends Nat
  sealed trait Succ[N <: Nat] extends Nat

  /** Sugar for writing small lengths in types.
    *
    * Instead of `SList[Succ[Succ[Succ[Succ[Succ[Zero]]]]], Int]` you write `SList[_5, Int]`.
    *
    * Only up to `_12` because these aliases are written by hand — Scala 2.13 can't generate them from a literal `5`. To
    * go higher, either add more lines below, or use a macro.
    */
  type _0  = Zero
  type _1  = Succ[_0]
  type _2  = Succ[_1]
  type _3  = Succ[_2]
  type _4  = Succ[_3]
  type _5  = Succ[_4]
  type _6  = Succ[_5]
  type _7  = Succ[_6]
  type _8  = Succ[_7]
  type _9  = Succ[_8]
  type _10 = Succ[_9]
  type _11 = Succ[_10]
  type _12 = Succ[_11]

  /** Phantom witness that `A + B = Out`.
    *
    * Carries no runtime work — it's only used to propagate the resulting length type when resolving implicits. The
    * induction mirrors Peano addition: `Zero + B = B`, and `Succ[A] + B = Succ[A + B]`.
    */
  trait Plus[A <: Nat, B <: Nat] {
    type Out <: Nat
  }
  object Plus {
    type Aux[A <: Nat, B <: Nat, O <: Nat] = Plus[A, B] { type Out = O }

    implicit def zero[B <: Nat]: Aux[Zero, B, B] = new Plus[Zero, B] { type Out = B }

    implicit def succ[A <: Nat, B <: Nat, O <: Nat](implicit rec: Aux[A, B, O]): Aux[Succ[A], B, Succ[O]] = {
      val _ = rec
      new Plus[Succ[A], B] { type Out = Succ[O] }
    }
  }

  /** Sized list: a `List[A]` whose length lives in the type `N`.
    *
    * The constructor is private to the package, so no one outside can build an `SList[N, A]` whose underlying list has
    * a different length than what `N` claims. Every public constructor (`empty`, `::`, `++`) preserves the invariant.
    */
  final class SList[N <: Nat, +A] private[SizedList] (val toList: List[A]) extends AnyVal {

    def ::[B >: A](b: B): SList[Succ[N], B] = new SList(b :: toList)

    /** Concatenation with a compile-time length check.
      *
      * The result type `SList[O, B]` is pinned by the `Plus.Aux[N, M, O]` witness; the value is built by the standard
      * `List.++`, which is stack-safe.
      */
    def ++[M <: Nat, B >: A, O <: Nat](that: SList[M, B])(implicit p: Plus.Aux[N, M, O]): SList[O, B] = {
      val _ = p
      new SList(toList ++ that.toList)
    }
  }

  object SList {
    def empty[A]: SList[Zero, A] = new SList(Nil)
  }
}
