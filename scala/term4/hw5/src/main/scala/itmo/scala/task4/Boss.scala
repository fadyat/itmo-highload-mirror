package itmo.scala.task4

import cats.Monad
import cats.effect.std.Supervisor
import cats.effect.syntax.all._
import cats.effect.{Deferred, Resource, Temporal}
import cats.syntax.all._

import java.util.concurrent.TimeoutException

/** Необходимо реализовать инстанс [[Boss]], который
  *   - может отдать задачу на выполнение с помощью [[Boss.start]] (при этом поток выполнения не должен блокироваться,
  *     вызов [[Boss.start]] не должен дожидаться результатов выполнения задачи)
  *   - задача должна начать выполняться '''сразу''' после вызова [[Boss.start]]
  *   - [[Boss.start]] возвращает [[Await]][F], вызов [[Await.get]] блокирует поток выполнения до тех пор, пока задача
  *     не будет выполнена
  *
  * Помимо того, что задача должна выполняться асинхронно, требуется:
  *   - печатать в консоль информацию о том, когда задача начала выполняться
  *   - печатать в консоль информацию о том, когда задача закончила выполняться / отменилась и тд с результатом задачи,
  *     текстом ошибки
  *   - если задача превышает максимальное время выполнения, необходимо отменять задачу, результат выполнения такой
  *     задачи должен быть [[itmo.scala.task4.TaskError.Timeout]]
  *   - если во время выполнении задачи произошла неожидаемая ошибка, то результат выполнения должен быть
  *     [[itmo.scala.task4.TaskError.UnexpectedError]]
  *
  * То есть, печать информации о процессе выполнения задач должен производиться в консоль в ЛЮБОМ СЛУЧАЕ
  */
trait Boss[F[_]] {
  def start(task: Task[F]): F[Await[F]]
}

object Boss {

  def make[F[_]: Reporter: Temporal]: Resource[F, Boss[F]] =
    Supervisor[F].map(new Impl[F](_))

  private class Impl[F[_]: Reporter: Temporal](
      supervisor: Supervisor[F]
  ) extends Boss[F] {

    override def start(task: Task[F]): F[Await[F]] =
      for {
        deferred <- Deferred[F, TaskResult]
        _        <- supervisor.supervise(runTask(task, deferred))
      } yield Await(deferred.get)

    private def runTask(
        task: Task[F],
        result: Deferred[F, TaskResult]
    ): F[Unit] = {

      def finish(r: TaskResult): F[Unit] =
        r.fold(Reporter.error(task, _), Reporter.success(task, _)) *> result.complete(r).void

      val body: F[Unit] =
        task.logic.timeout(task.timeout).attempt.flatMap {
          case Right(res)                => finish(res)
          case Left(_: TimeoutException) => finish(Left(TaskError.Timeout))
          case Left(e)                   => finish(Left(TaskError.UnexpectedError(e)))
        }

      (Reporter.start(task) *> body).onCancel {
        result
          .complete(Left(TaskError.Timeout))
          .ifM(Reporter.error(task, TaskError.Timeout), Monad[F].unit)
      }
    }

  }
}
