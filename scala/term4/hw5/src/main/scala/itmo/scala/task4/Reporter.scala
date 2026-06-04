package itmo.scala.task4

import cats.Applicative
import cats.effect.std.Console

trait Reporter[F[_]] {
  def start(task: Task[F]): F[Unit]
  def error(task: Task[F], err: TaskError): F[Unit]
  def success(task: Task[F], success: TaskSuccess): F[Unit]
}

object Reporter {
  def apply[F[_]](implicit reporter: Reporter[F]): Reporter[F] = reporter
  def noop[F[_]: Applicative]: Reporter[F]                     = new Noop[F]
  implicit def fromConsole[F[_]: Console]: Reporter[F]         = new Impl[F]

  def start[F[_]: Reporter](task: Task[F]): F[Unit]                         = Reporter[F].start(task)
  def error[F[_]: Reporter](task: Task[F], err: TaskError): F[Unit]         = Reporter[F].error(task, err)
  def success[F[_]: Reporter](task: Task[F], success: TaskSuccess): F[Unit] = Reporter[F].success(task, success)

  private class Impl[F[_]: Console] extends Reporter[F] {
    override def start(task: Task[F]): F[Unit] =
      Console[F].println(s"start task ${task.jiraId}")

    override def error(task: Task[F], err: TaskError): F[Unit] =
      Console[F].println(s"task ${task.jiraId} was failed, reason: ${err.message}")

    override def success(task: Task[F], success: TaskSuccess): F[Unit] =
      Console[F].println(s"task ${task.jiraId} was completed, amount of value: ${success.valueSize}")
  }

  private class Noop[F[_]: Applicative] extends Reporter[F] {
    override def start(task: Task[F]): F[Unit]                         = Applicative[F].unit
    override def error(task: Task[F], err: TaskError): F[Unit]         = Applicative[F].unit
    override def success(task: Task[F], success: TaskSuccess): F[Unit] = Applicative[F].unit
  }
}
