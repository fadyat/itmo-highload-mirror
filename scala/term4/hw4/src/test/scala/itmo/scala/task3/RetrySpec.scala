package itmo.scala.task3

import cats.{Monad, MonadError}
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers

class RetrySpec extends AnyFlatSpec with Matchers {

  "retry" should "works for big number of retries" in {
    def myRetry[F[_]: Monad, A](start: A)(f: A => F[A]): F[A] = retry(start)(f)

    myRetry(100000)(a => if (a == 0) Option.empty else Some(a - 1)) shouldBe None
  }

  "retry syntax" should "works for big number of retries" in {
    // (505) 5-034-455
    val correctNumber = 5055_034_455L
    val prefix        = 5050_000_000L
    var callNumber    = prefix + 5_000_000L

    def tryCallSaul: Either[String, String] = {
      callNumber += 1
      Either.cond(callNumber == correctNumber, "Saul found", "Try again")
    }

    def myRetry[F[_], A](fa: => F[A])(implicit monad: MonadError[F, ?]): F[A] =
      fa.retry

    myRetry(tryCallSaul) shouldBe Right("Saul found")
  }

}
