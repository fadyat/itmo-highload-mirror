package itmo.scala.hw2.task1

import org.scalacheck.Gen.chooseNum
import org.scalacheck.Prop.forAll
import org.scalacheck.Properties

object FibonacciPropSpec extends Properties("Fibonacci") {

  property("fib: next = prev + prevPrev") = forAll(chooseNum[Int](0, 1000)) { n: Int =>
    Fibonacci.fib(n + 2) == (Fibonacci.fib(n + 1) + Fibonacci.fib(n))
  }

  property("fibDiv: should divide by K") =
    forAll(chooseNum[Int](1, 15)) { k: Int =>
      forAll(chooseNum[Int](0, 1000)) { n: Int =>
        Fibonacci.fibDiv(k, n) % k == 0
      }
    }

  property("fibDiv: divide by 1, same as fib") =
    forAll(chooseNum[Int](0, 1000)) { n: Int =>
      Fibonacci.fibDiv(1, n + 2) == (Fibonacci.fibDiv(1, n + 1) + Fibonacci.fibDiv(1, n))
    }

}
