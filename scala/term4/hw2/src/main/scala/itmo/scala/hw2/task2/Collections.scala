package itmo.scala.hw2.task2

import itmo.scala.hw2.task2.ZeroOne.Zero
import itmo.scala.hw2.task2.ZeroOne.One

/** Прежде чем делать [[Collections]], крайне рекомендуем вам сделать [[Transformation]]
  */
object Collections {

  /** В отсортированном списке (по возрастанию) найдите все пары двух соседних чисел, между которыми есть пробел:
    *
    * @example
    *   {{{
    *  findGaps(List(1, 2, 3, 4)) // None
    *  findGaps(List(1, 2, 8))    // Some(List((2, 8)))
    *  findGaps(List(3, 5, 7))    // Some(List((3, 5), (5, 7)))
    *   }}}
    * @param l
    *   отсортированный список уникальных значений
    * @return
    *   список пар чисел, между которыми есть пробел
    */
  def findGaps(l: Seq[Int]): Option[List[(Int, Int)]] = {
    val gaps = l.sliding(2).collect { case Seq(a, b) if b.toLong - a.toLong > 1 => (a, b) }.toList
    Option.when(gaps.nonEmpty)(gaps)
  }

  /** Подсчитайте количество '''последовательных''' вхождений каждого символа в строке
    * @param s
    *   строка
    * @return
    *   список пар (символ, количество '''последовательных''' вхождений)
    *
    * @example
    *   {{{
    *   count("")       // List()
    *   count("a")      // List(('a', 1))
    *   count("aa")     // List(('a', 2))
    *   count("aaabaa") // List(('a', 3), ('b', 1), ('a', 2))
    *   }}}
    */
  def count(s: String): List[(Char, Int)] = {
    s.foldLeft(List.empty[(Char, Int)]) {
      case ((pc, cnt) :: tail, c) if pc == c => (pc, cnt + 1) :: tail
      case (acc, c)                          => (c, 1) :: acc
    }.reverse
  }

  /** Отсортируйте людей в следующем порядке: фамилия (лексикографический порядок) -> возраст (по убыванию) -> имя
    * (лексикографический порядок) -> отчество (лексикографический порядок)
    *
    * @param users
    *   список людей (фамилия, имя, отчество, возраст)
    * @return
    *   отсортированный список людей
    *
    * @example
    *   {{{
    *   sortUsers(List(
    *     User("Sidorov", "Ivan", "Ivanovich", 30),
    *     User("Ivanov", "Petr", "Petrovich", 13),
    *     User("Ivanov", "Sidor", "Sidor", 20)
    *   ))
    *   // List(
    *   //   User("Ivanov", "Sidor", "Sidor", 20),
    *   //   User("Ivanov", "Petr", "Petrovich", 13),
    *   //   User("Sidorov", "Ivan", "Ivanovich", 30),
    *   // )
    *   }}}
    */
  def sortUsers(users: List[User]): List[User] = {
    users.sortBy(u => (u.lastName, -u.age, u.firstName, u.middleName))
  }

  /** Вам дана последовательность из `0` и `1`, ваша задача - удалить одну цифру так что бы в последовательности
    * осталось максимальное количество `0` подряд. В ответ выдать максимальное количество `0` подряд в получившейся
    * последовательности (после удаления одной цифры).
    *
    * Note:
    *   - удалить цифру можно только один раз;
    *   - нужно обязательно удалить хотя бы одну цифру (если список не пустой);
    *   - для пустого списка нужно вернуть `0` в качестве ответа.
    *
    * @param input
    *   последовательность из `0` и `1`
    * @return
    *   максимальное количество `0` подряд в получившейся последовательности после удаления одной цифры
    *
    * @example
    *   {{{
    *   maximizeZeros(List(Zero, Zero, One, Zero, Zero, Zero, One, Zero, One, Zero, Zero)) // 5
    *   maximizeZeros(List(Zero, Zero, One, Zero, Zero, Zero, One, Zero, Zero, Zero)) // 6
    *   maximizeZeros(List(Zero, Zero, Zero))      // 2
    *   maximizeZeros(List(Zero, One, Zero, Zero)) // 3
    *   maximizeZeros(List(Zero, Zero, Zero, One)) // 3
    *   maximizeZeros(List(One, Zero, Zero, Zero)) // 3
    *   maximizeZeros(List(One))                   // 0
    *   maximizeZeros(List.empty)                  // 0
    *   }}}
    */
  def maximizeZeros(input: List[ZeroOne]): Int = {
    val groups = input
      .foldLeft(List.empty[Int]) {
        case (cnt :: tail, Zero) => (cnt + 1) :: tail
        case (acc, One)          => 0 :: acc
        case (Nil, Zero)         => 1 :: Nil
      }

    groups match {
      case Nil      => 0
      case x :: Nil => math.max(x - (if (input.headOption.contains(Zero)) 1 else 0), 0)
      case _        => groups.sliding(2).map(_.sum).max
    }
  }
}
