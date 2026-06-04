package itmo.scala.task3.env

import cats.effect.IO
import cats.effect.std.Env
import cats.effect.testing.scalatest.AsyncIOSpec
import org.scalatest.freespec.AsyncFreeSpec
import org.scalatest.matchers.should.Matchers

class LocalEnvSpec extends AsyncFreeSpec with AsyncIOSpec with Matchers {

  "LocalEnv " - {
    "get values from Env" in {
      for {
        env <- LocalEnv.fromEnv[IO](TestEnv(Map("a" -> "b", "c" -> "d")))
        a1  <- env.get("a")
        a2  <- env.get("c")
      } yield {
        a1 shouldBe Some("b")
        a2 shouldBe Some("d")
      }
    }

    "rewrite values from Env" in {
      for {
        env <- LocalEnv.fromEnv[IO](TestEnv(Map("a" -> "b")))
        a1  <- env.get("a")
        _   <- env.set("a", "c")
        a2  <- env.get("a")
      } yield {
        a1 shouldBe Some("b")
        a2 shouldBe Some("c")
      }
    }

    "use local storage" in {
      val realEnv = Env.make[IO]

      for {
        env <- LocalEnv.fromEnv(realEnv)
        a1  <- env.get("a")
        _   <- env.set("a", "c")
        a2  <- env.get("a")
        a3  <- realEnv.get("a")
      } yield {
        a1 shouldBe None
        a2 shouldBe Some("c")
        a3 shouldBe None
      }
    }

    "entries return from both envs" in {
      for {
        env <- LocalEnv.fromEnv[IO](TestEnv(Map("a" -> "b", "c" -> "d")))
        _   <- env.set("a", "new b")
        a2  <- env.entries
      } yield a2 should contain theSameElementsAs Seq(("a", "new b"), ("c", "d"))
    }
  }

}
