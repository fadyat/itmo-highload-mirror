package itmo.scala.task3

import cats.effect.IO
import cats.effect.std.{Console, Env, Queue}
import cats.effect.testing.scalatest.AsyncIOSpec
import itmo.scala.task3.env.{LocalEnv, TestEnv}
import org.scalatest.Assertion
import org.scalatest.freespec.AsyncFreeSpec
import org.scalatest.matchers.should.Matchers

import scala.concurrent.duration.{DurationInt, FiniteDuration}

class ApplicationSpec extends AsyncFreeSpec with AsyncIOSpec with Matchers {

  "Application with console " - {
    "exit" in {
      test(
        List("exit"),
        List.empty
      )
    }

    "get env from global context" in {
      test(
        List("get some_global_var", "exit"),
        List("'some_global_var' 'some_value '"),
        TestEnv(Map("some_global_var" -> "some_value "))
      )
    }

    "save vars" in {
      test(
        List("get HTTP_PORT", "set HTTP_PORT 8080", "get HTTP_PORT", "exit"),
        List("Not found", "'HTTP_PORT' '8080'", "'HTTP_PORT' '8080'")
      )
    }

    "rewrite env from global context" in {
      test(
        List("get some_global_var", "set some_global_var   42", "get some_global_var", "exit"),
        List("'some_global_var' 'some_value '", "'some_global_var' '42'", "'some_global_var' '42'"),
        TestEnv(Map("some_global_var" -> "some_value "))
      )
    }

    "proceed unknown command" in {
      test(
        List("hello! it's me", "get some_global_var 42", "set 1 1 1", "", "exit"),
        List("Unknown command", "Unknown command", "Unknown command", "Unknown command"),
        TestEnv(Map("some_global_var" -> "some_value "))
      )
    }

    "work correct for example" in {
      test(
        List(
          "get HTTP_PORT",
          "set HTTP_PORT 8080",
          "get HTTP_PORT",
          "set HTTP_PORT  80",
          "get HTTP_PORT",
          "get LANG",
          "set LANG C.UTF-16",
          "get LANG",
          "exit"
        ),
        List(
          "Not found",
          "'HTTP_PORT' '8080'",
          "'HTTP_PORT' '8080'",
          "'HTTP_PORT' '80'",
          "'HTTP_PORT' '80'",
          "'LANG' 'C.UTF-8'",
          "'LANG' 'C.UTF-16'",
          "'LANG' 'C.UTF-16'"
        ),
        TestEnv(Map("LANG" -> "C.UTF-8"))
      )

    }

    "not save local env var in global context" in {
      val realEnv = Env.make[IO]

      test(
        List("set HTTP_PORT 8080", "exit"),
        List("'HTTP_PORT' '8080'"),
        realEnv
      ) >> test(
        List("get HTTP_PORT", "exit"),
        List("Not found"),
        realEnv
      )
    }
  }

  private def test(
      inputList: List[String],
      expectedList: List[String],
      testEnv: Env[IO] = TestEnv(Map.empty[String, String]),
      timeout: FiniteDuration = 5.seconds
  ): IO[Assertion] =
    for {
      input  <- Queue.bounded[IO, String](inputList.size)
      output <- Queue.bounded[IO, String](expectedList.size)

      implicit0(env: LocalEnv[IO]) <- LocalEnv.fromEnv[IO](testEnv)

      fiber <- {
        implicit val console: Console[IO] = new TestConsole[IO](input, output)
        Application
          .consoleApplication[IO]
          .timeout(timeout)
          .productR(output.tryTakeN(None))
          .start
      }
      _      <- input.tryOfferN(inputList)
      output <- fiber.joinWithNever
    } yield output shouldBe expectedList

}
