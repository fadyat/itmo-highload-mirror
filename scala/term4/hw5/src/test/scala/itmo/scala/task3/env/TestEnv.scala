package itmo.scala.task3.env

import cats.Applicative
import cats.effect.std.Env
import cats.syntax.all._

import scala.collection.immutable.Iterable

case class TestEnv[F[_]: Applicative](
    store: Map[String, String]
) extends Env[F] {

  override def get(name: String): F[Option[String]] =
    store.get(name).pure[F]

  override def entries: F[Iterable[(String, String)]] =
    store.pure[F].widen

}
