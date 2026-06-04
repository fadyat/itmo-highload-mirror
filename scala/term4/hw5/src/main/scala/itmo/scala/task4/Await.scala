package itmo.scala.task4

/** Интерфейс для "ожидания" выполнения задачи
  */
trait Await[F[_]] {
  def get: F[TaskResult]
}

object Await {
  def apply[F[_]](f: F[TaskResult]): Await[F] = new Await[F] {
    override val get: F[TaskResult] = f
  }
}
