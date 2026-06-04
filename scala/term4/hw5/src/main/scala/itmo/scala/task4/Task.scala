package itmo.scala.task4

import scala.concurrent.duration.{DurationInt, FiniteDuration}

/** Задача
  *
  * @param jiraId
  *   идентификатор в джире
  * @param logic
  *   сама задача в виде синхронного вычисления
  * @param timeout
  *   максимальное время которое можно затратить на задачу
  */
case class Task[F[_]](
    jiraId: String,
    logic: F[TaskResult],
    timeout: FiniteDuration = 5.seconds,
)
