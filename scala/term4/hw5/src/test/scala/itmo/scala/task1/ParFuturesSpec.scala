package itmo.scala.task1

import org.scalatest.concurrent.TimeLimitedTests
import org.scalatest.flatspec.AnyFlatSpec
import org.scalatest.matchers.should.Matchers
import org.scalatest.time.Span

import java.util.concurrent.atomic.AtomicInteger
import java.util.concurrent.{ExecutorService, Executors}
import scala.concurrent.duration.DurationInt
import scala.concurrent.{Await, ExecutionContext, Future}

class ParFuturesSpec extends AnyFlatSpec with Matchers with TimeLimitedTests {

  "Futures.fullSequence" should "process list of success futures" in new WithLimitedExecutionContext {

    /** best answer will process task with 9 runnable, good answer will process task with 12 runnable
      */
    val limit = 12

    implicit val executionContext: ExecutionContext = limitedExec
    val fut1: Future[Int]                           = fut(1)
    val fut2: Future[Int]                           = fut(2)
    val fut3: Future[Int]                           = fut(3)

    await(Futures.fullSequence[Int](List(fut1, fut2, fut3))) shouldBe (List(1, 2, 3), List())

    if (counter.get() > limit) {
      fail(
        s"""
        |Runnable limit reached, You can do better :)
        | Your steps: ${counter.get()};
        | Limit for good solution: 12;
        | Limit for best solution: 9;
        |""".stripMargin
      )
    }
  }

  it should "process list of success and failures" in new WithLimitedExecutionContext {

    /** best answer will process task with 7 runnable; good answer will process task with 8 runnable
      */
    val limit = 8

    implicit val executionContext: ExecutionContext = limitedExec
    val ex1: Exception                              = new Exception("ex1")
    val ex2: Exception                              = new Exception("ex2")
    val failed1: Future[Nothing]                    = Future.failed(ex1)
    val failed2: Future[Nothing]                    = Future.failed(ex2)
    val fut1: Future[Int]                           = fut(1)

    await(Futures.fullSequence[Int](List(fut1, failed1, failed2))) shouldBe (List(1), List(ex1, ex2))

    if (counter.get() > limit) {
      fail(
        s"""
           |Runnable limit reached, You can do better :)
           | Your steps: ${counter.get()};
           | Limit for good solution: 8;
           | Limit for best solution: 7;
           |""".stripMargin
      )
    }
  }

  it should "process list of failures" in new WithLimitedExecutionContext {

    /** best answer will process task with 4 runnable
      */
    val limit = 4

    implicit val executionContext: ExecutionContext = limitedExec
    val ex1: Exception                              = new Exception("ex1")
    val ex2: Exception                              = new Exception("ex2")
    val failed1: Future[Nothing]                    = Future.failed(ex1)
    val failed2: Future[Nothing]                    = Future.failed(ex2)

    await(Futures.fullSequence[Int](List(failed1, failed2))) shouldBe (List(), List(ex1, ex2))

    if (counter.get() > limit) {
      fail(
        s"""
           |Runnable limit reached, You can do better :)
           | Your steps: ${counter.get()};
           | Limit for good solution: 4;
           |""".stripMargin
      )
    }
  }

  "Futures.traverse" should "behave as a scala Future.traverse" in runAndAwait { implicit ec =>
    val xs: List[Int] = (1 to 3).toList

    val actual: Future[List[Int]]   = Futures.traverse(xs)(fut)
    val expected: Future[List[Int]] = Future.traverse(xs)(fut)

    for {
      actualResult   <- actual
      expectedResult <- expected
    } yield {
      actualResult shouldBe expectedResult
      actualResult shouldBe xs
    }
  }

  it should "work with empty lists" in runAndAwait { implicit ec =>
    Futures.traverse(Nil)(fut).map(_ shouldBe Nil)
  }

  it should "correctly stop on Future failures" in runAndAwait { implicit ec =>
    val exception     = new Exception("Catch me if you can!")
    val xs: List[Int] = (1 to 100).toList

    Futures
      .traverse(xs) { elem => if (elem == 42) Future.failed(exception) else fut(elem) }
      .failed.map(_ shouldBe exception)
  }

  it should "correctly stop on exceptions" in runAndAwait { implicit ec =>
    val exception     = new Exception("Catch me if you can!")
    val xs: List[Int] = (1 to 100).toList

    Futures
      .traverse(xs) { elem => if (elem == 42) throw exception else fut(elem) }
      .failed
      .map(_ shouldBe exception)
  }

  "Futures.mapReduce" should "asynchronously map and reduce the list and skip failures" in runAndAwait { implicit ec =>
    val exception                 = new Exception("Catch me if you can!")
    val xs: List[Int]             = (1 to 10).toList
    val predicate: Int => Boolean = a => a == 5 || a == 6
    val map: Int => Future[Int]   = a =>
      if (predicate(a)) Future.failed(exception)
      else Future.successful(a * 2)

    val reduce: (Int, Int) => Int = _ + _

    Futures.mapReduce(xs, map, reduce)
      .map(_ shouldBe Some(xs.filterNot(predicate).map(_ * 2).sum))
  }

  it should "return None if all elements mapping is failed" in runAndAwait { implicit ec =>
    val exception                 = new Exception("Catch me if you can!")
    val xs: List[Int]             = (1 to 10).toList
    val map: Int => Future[Int]   = _ => Future.failed(exception)
    val reduce: (Int, Int) => Int = _ + _

    Futures.mapReduce(xs, map, reduce)
      .map(_ shouldBe None)
  }

  it should "return None if all elements mapping is exception" in runAndAwait { implicit ec =>
    val exception                 = new Exception("Catch me if you can!")
    val xs: List[Int]             = (1 to 10).toList
    val map: Int => Future[Int]   = _ => throw exception
    val reduce: (Int, Int) => Int = _ + _

    Futures.mapReduce(xs, map, reduce)
      .map(_ shouldBe None)
  }

  private def fut(i: Int)(implicit ex: ExecutionContext): Future[Int] = Future {
    Thread.sleep(1000 * i)
    i
  }(ex)

  private trait WithLimitedExecutionContext {
    val counter: AtomicInteger = new AtomicInteger(-1)

    def limitedExec: ExecutionContext = new ExecutionContext {
      val global: ExecutionContext = scala.concurrent.ExecutionContext.Implicits.global

      override def execute(runnable: Runnable): Unit = {
        counter.incrementAndGet()
        global.execute(runnable)
      }

      override def reportFailure(cause: Throwable): Unit = ???
    }

    protected def await[A](future: Future[A]): A = Await.result(future, 10.seconds)
  }

  private def runAndAwait[A](f: ExecutionContext => Future[A]): A = {
    // Create independent ExecutionContext for each test
    val executor: ExecutorService          = Executors.newFixedThreadPool(100)
    val executionContext: ExecutionContext = ExecutionContext.fromExecutor(executor)

    val result = Await.result(f(executionContext), 5.seconds)

    executor.shutdown()

    result
  }

  override def timeLimit: Span = 10.seconds
}
