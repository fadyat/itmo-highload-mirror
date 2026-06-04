package itmo.scala.hw2.task2

import scala.annotation.tailrec

object Transformation {

  /** Получить список, который содержит длину строк
    */
  def lengthOfStrings(input: List[String]): List[Int] = input.map(_.length)

  /** Разделите каждое слово на буквы и верните результирующую коллекцию
    */
  def splitWords(input: List[String]): List[Char] = input.flatMap(_.toList)

  /** Получить список, который содержит длину строк. Оставьте только те элементы, которые **начинаются с большой буквы**
    */
  def lengthOfUpStrings(input: List[String]): List[Int] = input.filter(_.headOption.exists(_.isUpper)).map(_.length)

  /** Верните первый элемент списка
    */
  def headElement(input: List[String]): Option[String] = input.headOption

  /** Верните последний элемент списка
    */
  def lastElement(input: List[String]): Option[String] = input.lastOption

  /** Верните элементы со 2 по 5
    */
  def splitFrom2To5(input: List[String]): List[String] = input.slice(1, 5)

  /** Верните первые 4 элемента
    */
  def take4Elements(input: List[String]): List[String] = input.take(4)

  /** Уберите из списка все элементы с заглавными буквами
    */
  def removeUpStrings(input: List[String]): List[String] = input.filterNot(_.exists(_.isUpper))

  /** Сгруппируйте элементы списка по длине строк
    */
  def groupByLength(input: List[String]): Map[Int, List[String]] = input.groupBy(_.length)

  /** Сгруппируйте элементы списка по длине строк и получите количество элементов в этих группах
    *
    * Note: подсказка - используйте '''только''' groupMapReduce (один раз)
    */
  def groupByLengthAndCount(input: List[String]): Map[Int, Int] = input.groupMapReduce(_.length)(_ => 1)(_ + _)

  /** Сосчитайте суммарную длину всех строк в списке, используя `.sum`
    */
  def sumOfLength(input: List[String]): Int = input.map(_.length).sum

  /** Отформатируйте красиво спискок в строку (`[ "a", "b" ]`)
    *
    * Note: подсказка используйте одну из вариаций `.mkString`
    */
  def formatList(input: List[String]): String = input.map(s => s"\"$s\"").mkString("[", ", ", "]")

  /** Сосчитайте суммарную длину всех строк в массиве, используя хвостовую рекурсию и Pattern Matching и ничего больше
    */
  def sumOfLengthTailRec(input: List[String]): Int = {
    @tailrec
    def loop(lst: List[String], acc: Int): Int = lst match {
      case Nil          => acc
      case head :: tail => loop(tail, acc + head.length)
    }

    loop(input, 0)
  }

  /** Сосчитайте суммарную длину всех строк в массиве, используя `.foldLeft`
    */
  def sumOfLengthFoldLeft(input: List[String]): Int = input.foldLeft(0)((acc, str) => acc + str.length)

  /** Отсортируйте строки в лексикографическом порядке
    */
  def sort(input: List[String]): List[String] = input.sorted

  /** Отсортируйте строки по длине, если длины равные то сортировка должна быть в лексикографическом порядке
    */
  def sortByLen(input: List[String]): List[String] = input.sortBy(s => (s.length, s))
}
