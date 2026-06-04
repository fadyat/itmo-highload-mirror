package itmo.scala.task1

import scala.concurrent.{ExecutionContext, Future}

object Futures {

  /** Реализуйте функцию, которая выполнит свертку (fold) входящей последовательности из [[Future]], используя
    * переданный комбинатор и начальное значение для свертки. Если какая-либо из исходных [[Future]] зафейлилась, то
    * '''должна вернуться ошибка от нее'''
    */
  def foldF[A, B](in: List[Future[A]], zero: B, op: (B, A) => B)(
      implicit executionContext: ExecutionContext
  ): Future[B] =
    in.foldLeft(Future.successful(zero)) { (accF, fa) =>
      for {
        acc <- accF
        a   <- fa
      } yield op(acc, a)
    }

  /** Реализуйте функцию, которая выполнит свертку (fold) входящей последовательности из [[Future]], используя
    * переданный асинхронный комбинатор и начальное значение для свертки. Если какая-либо из исходных [[Future]]
    * зафейлилась, то должна вернуться ошибка от нее. Если комбинатор зафейлился, то должна вернуться ошибка от него.
    */
  def flatFoldF[A, B](in: List[Future[A]], zero: B, op: (B, A) => Future[B])(
      implicit executionContext: ExecutionContext
  ): Future[B] =
    in.foldLeft(Future.successful(zero)) { (accF, fa) =>
      for {
        acc <- accF
        a   <- fa
        b   <- op(acc, a)
      } yield b
    }

  /** Реализуйте функцию [[fullSequence]], похожую на [[Future.sequence]], но в отличие от нее, возвращающую все
    * успешные и не успешные результаты. Возвращаемое тип функции - кортеж из двух списков, в левом хранятся результаты
    * успешных выполнений, в правом результаты неуспешных выполнений. Не допускается использование методов объекта
    * [[scala.concurrent.Await]].
    */
  def fullSequence[A](futures: List[Future[A]])(
      implicit ex: ExecutionContext
  ): Future[(List[A], List[Throwable])] =
    futures
      .foldLeft(Future.successful((List.empty[A], List.empty[Throwable]))) {
        (accF, fa) =>
          accF.flatMap { case (as, es) =>
            fa.map(a => (a :: as, es)).recover { case e => (as, e :: es) }
          }
      }
      .map { case (as, es) => (as.reverse, es.reverse) }

  /** Реализуйте traverse c помощью метода [[Future.sequence]]
    */
  def traverse[A, B](in: List[A])(fn: A => Future[B])(
      implicit ex: ExecutionContext
  ): Future[List[B]] =
    Future.sequence(in.map(a => Future(fn(a)).flatten))

  /** Реализуйте алгоритм map/reduce. Исходный список обрабатывается параллельно (конкурентно) с помощью применения
    * функции `map`` к каждому элементу. Результаты работы функции `map`` должны быть свернуты в одно значение функцией
    * `reduce`. Если в ходе выполнения какой-либо операции возникло исключение - эту обработку нужно игнорировать. Если
    * ни один вызов `map` не завершился успешно, вернуть [[None]]
    */
  def mapReduce[A, T, T1 >: T](in: List[A], map: A => Future[T], reduce: (T1, T1) => T1)(
      implicit ex: ExecutionContext
  ): Future[Option[T1]] =
    Future
      .sequence(in.map(a => Future(map(a)).flatten.map(Some(_)).recover { case _ => None }))
      .map(_.flatten.reduceLeftOption(reduce))

}
