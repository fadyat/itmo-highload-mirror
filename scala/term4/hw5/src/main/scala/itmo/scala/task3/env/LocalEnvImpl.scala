package itmo.scala.task3.env

import cats.Monad
import cats.effect.Ref
import cats.effect.std.Env
import cats.syntax.all._

import scala.collection.immutable.Iterable

private class LocalEnvImpl[F[_]: Env: Monad](
    overrides: Ref[F, Map[String, String]]
) extends LocalEnv[F] {

  override def set(name: String, value: String): F[Unit] = overrides.update(_.updated(name, value))

  override def get(name: String): F[Option[String]] =
    for {
      local  <- overrides.get
      result <- local.get(name) match {
        case some @ Some(_) => some.pure[F]
        case None           => Env[F].get(name)
      }
    } yield result

  override def entries: F[Iterable[(String, String)]] =
    for {
      local <- overrides.get
      base  <- Env[F].entries
    } yield base.toMap ++ local

}
