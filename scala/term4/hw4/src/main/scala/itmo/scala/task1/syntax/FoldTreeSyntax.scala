package itmo.scala.task1.syntax

import cats.kernel.Monoid
import itmo.scala.task1.Tree
import itmo.scala.task1.syntax.FoldTreeSyntax.FoldableTreeOps
import itmo.scala.task1.Tree.{Branch, Leaf, Nil}
import scala.annotation.tailrec
import cats.syntax.all._

// Syntax обычно объявляют как trait, чтобы:
// 1. Вы могли собрать свой собственный syntax object с набором синтаксиса который вы используете у себя в проекте,
// и импортировать везде где требуется уже этот object можете посмотреть как реализован cats.syntax.AllSyntax и где он используется
// 2. Вы могли подмешать синтаксис в компаньон объект ваших классов, чтобы необходимые implicit автоматически тянулись без import
trait FoldTreeSyntax {
  implicit def foldableTreeOps[T](tree: Tree[T]): FoldableTreeOps[T] = new FoldableTreeOps[T](tree)
}

object FoldTreeSyntax {
  protected class FoldableTreeOps[T](private val tree: Tree[T]) extends AnyVal {
    def fold(implicit M: Monoid[T]): T = FoldableTreeOps.foldImpl(tree)
  }

  protected object FoldableTreeOps {
    def foldImpl[T](root: Tree[T])(implicit M: Monoid[T]): T = {

      @tailrec
      def loop(stack: List[Tree[T]], acc: T): T = stack match {
        case scala.collection.immutable.Nil => acc
        case Nil :: rest                    => loop(rest, acc)
        case Leaf(v) :: rest                => loop(rest, acc |+| v)
        case Branch(v, l, r) :: rest        => loop(l :: Leaf(v) :: r :: rest, acc)
      }

      loop(root :: scala.collection.immutable.Nil, M.empty)
    }
  }
}
