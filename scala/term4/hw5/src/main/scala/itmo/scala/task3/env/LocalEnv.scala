package itmo.scala.task3.env

import cats.Monad
import cats.effect.Ref
import cats.effect.std.Env
import cats.syntax.all._
import scala.collection.immutable.Iterable

trait LocalEnv[F[_]] extends Env[F] {
  def set(name: String, value: String): F[Unit]
}

object LocalEnv {

  def apply[F[_]](implicit localEnv: LocalEnv[F]): LocalEnv[F] = localEnv

  def fromEnv[F[_]: Ref.Make: Monad](env: Env[F]): F[LocalEnv[F]] =
    for {
      initial <- env.entries
      ref     <- Ref.of[F, Map[String, String]](initial.toMap)
    } yield new LocalEnv[F] {
      def set(name: String, value: String): F[Unit] = ref.update(_.updated(name, value))
      def get(name: String): F[Option[String]]      = ref.get.map(_.get(name))
      def entries: F[Iterable[(String, String)]]    = ref.get.map(_.toList)
    }

}
