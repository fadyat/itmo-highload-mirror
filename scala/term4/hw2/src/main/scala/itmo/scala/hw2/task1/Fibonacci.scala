package itmo.scala.hw2.task1

import scala.annotation.tailrec

object Fibonacci {

  /** Напишите функцию, которая будет вычислять число Фибоначчи через рекурсию для заданного лимита. Ограничения задачи:
    *
    * На вход функции всегда передается валидное неотрицательное число Максимально может быть `Long.MAX_VALUE` чисел,
    * поэтому нужно учесть возможность переполнения стэка при подсчетах. Так как `0 <= limit <= Long.MAX_VALUE`, то в
    * качестве результата возвращать `BigInt`
    *
    * @param limit
    *   номер N-ого числа Фибоначчи
    * @return
    *   N-ное число Фибоначчи
    *
    * @example
    *   {{{
    *   fib(0) // 0
    *   fib(1) // 1
    *   fib(2) // 1
    *   fib(3) // 2
    *   fib(4) // 3
    *   fib(5) // 5
    *   }}}
    */
  def fib(limit: Int): BigInt = {
    val n          = limit.max(0)
    val highestBit = 31 - Integer.numberOfLeadingZeros(n)

    @annotation.tailrec
    def loop(i: Int, a: BigInt, b: BigInt): BigInt =
      if (i < 0) a
      else {
        val c = a * (b * 2 - a)
        val d = a * a + b * b
        if (((n >> i) & 1) == 0) loop(i - 1, c, d)
        else loop(i - 1, d, c + d)
      }

    loop(highestBit, 0, 1)
  }

  /** Фибоначчи, делящиеся на k
    *
    * Рассмотрим последовательность Фибоначчи:
    * {{{
    * 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711 ...
    * }}}
    * Давайте рассмотрим последовательность, которая берет только числа, делящиеся на (`k` > 1):
    * {{{
    *    (k = 2): 0, 2, 8, 34, 144, 610, 2584, 10946, 46368, 196418, 832040, 3524578, 14930352, 63245986, 267914296, 1134903170, 4807526976 ...
    *    (k = 3): 0, 3, 21, 144, 987, 6765, 46368, 317811, 2178309, 14930352, 102334155, 701408733, 4807526976, 32951280099, 225851433717, ...
    *    (k = 4): 0, 8, 144, 2584, 46368, 832040, 14930352, 267914296, 4807526976, 86267571272, 1548008755920, ...
    *    ...
    * }}}
    * Необходимо реализовать метод, который берет на вход (k > 1, limit >= 0) и возвращает n-ый эелемент
    * последовательности для делителя k.
    *
    * @example
    *   {{{
    *    fibDiv(2, 0) // 0
    *    fibDiv(2, 1) // 2
    *    fibDiv(2, 2) // 8
    *
    *    fibDiv(3, 0) // 0
    *    fibDiv(3, 1) // 3
    *    fibDiv(3, 2) // 21
    *
    *    fibDiv(10, 2) // 832040
    *   }}}
    * @param k
    *   делитель
    * @param limit
    *   номер N-ого числа Фибоначчи делящегося на k без остатка
    */
  def fibDiv(k: Int, limit: Int): BigInt =
    if (limit == 0) 0
    else if (k == 1) fib(limit)
    else {

      @tailrec
      def pisanoPeriod(a: Int, b: Int, acc: Vector[Int]): Vector[Int] = {
        val next = (a + b) % k
        if (b == 0 && next == 1) acc
        else pisanoPeriod(b, next, acc :+ b)
      }

      val pisano           = pisanoPeriod(0, 1, Vector(0))
      val divisibleIndices = pisano.zipWithIndex.collect { case (v, idx) if v == 0 => idx }

      val fullCycles     = limit / divisibleIndices.size
      val remainderIndex = limit % divisibleIndices.size
      val nIndex         = fullCycles * pisano.size + divisibleIndices(remainderIndex)

      fib(nIndex)
    }
}
