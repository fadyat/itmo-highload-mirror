package itmo.scala.task1

import org.scalatest.flatspec.AsyncFlatSpec
import org.scalatest.matchers.should.Matchers

import scala.concurrent.Future

class FuturesSpec extends AsyncFlatSpec with Matchers {

  "Futures.foldF" should "return zero if list is empty" in {
    Futures.foldF[Int, Int](List.empty[Future[Int]], 0, _ + _)
      .map(_ shouldBe 0)
  }

  it should "return sum of all elements" in {
    Futures.foldF[Int, Int](List(Future.successful(1), Future.successful(2), Future.successful(3)), 0, _ + _)
      .map(_ shouldBe 6)
  }

  it should "return sum of all elements with initial value" in {
    Futures.foldF[Int, Int](List(Future.successful(1), Future.successful(2), Future.successful(3)), 10, _ + _)
      .map(_ shouldBe 16)
  }

  it should "fail if any of the futures fails" in {
    val exception = new Exception("Catch me if you can!")
    Futures.foldF[Int, Int](List(Future.successful(1), Future.failed(exception), Future.successful(3)), 0, _ + _)
      .failed.map(_ shouldBe exception)
  }

  it should "fold list of futures into a single future using combining function" in {
    val input = List(1, 2, 3, 4, 5, 6).map(Future.successful)

    for {
      sum <- Futures.foldF[Int, Int](input, 0, _ + _)
      mul <- Futures.foldF[Int, Int](input, 1, _ * _)
    } yield {
      sum shouldBe 21
      mul shouldBe 720
    }
  }

  "Futures.flatFoldF" should "return zero if list is empty" in {
    Futures.flatFoldF[Int, Int](List.empty[Future[Int]], 0, (b, a) => Future.successful(b + a))
      .map(_ shouldBe 0)
  }

  it should "return sum of all elements" in {
    Futures.flatFoldF[Int, Int](
      List(Future.successful(1), Future.successful(2), Future.successful(3)),
      0,
      (b, a) => Future.successful(b + a)
    ).map(_ shouldBe 6)
  }

  it should "fold list of futures into a single future using async combining function" in {
    val input   = List(1, 2, 3, 4, 5, 6).map(Future.successful)
    val sum     = (a: Int, b: Int) => Future.successful(a + b)
    val product = (a: Int, b: Int) => Future.successful(a * b)

    for {
      sum <- Futures.flatFoldF[Int, Int](input, 0, sum)
      mul <- Futures.flatFoldF[Int, Int](input, 1, product)
    } yield {
      sum shouldBe 21
      mul shouldBe 720
    }
  }

  it should "fail if any of the futures fails" in {
    val exception = new Exception("Catch me if you can!")
    Futures.flatFoldF[Int, Int](
      List(Future.successful(1), Future.failed(exception), Future.successful(3)),
      0,
      (b, a) => Future.successful(b + a)
    ).failed.map(_ shouldBe exception)
  }

  it should "fail if reduce function fails" in {
    val exception = new Exception("Catch me if you can!")
    Futures.flatFoldF[Int, Int](
      List(Future.successful(1), Future.successful(2), Future.successful(3)),
      0,
      (_, _) => Future.failed(exception)
    ).failed.map(_ shouldBe exception)
  }

  it should "fail if reduce function throws exception" in {
    val exception = new Exception("Catch me if you can!")
    Futures.flatFoldF[Int, Int](
      List(Future.successful(1), Future.successful(2), Future.successful(3)),
      0,
      (_, _) => throw exception
    ).failed.map(_ shouldBe exception)
  }
}
