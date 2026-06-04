package itmo.scala.hw1.weekday

sealed trait WeekDay

object WeekDay {
  case object Monday    extends WeekDay
  case object Tuesday   extends WeekDay
  case object Wednesday extends WeekDay
  case object Thursday  extends WeekDay
  case object Friday    extends WeekDay
  case object Saturday  extends WeekDay
  case object Sunday    extends WeekDay

  val values: Seq[WeekDay]               = Seq(Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday)
  private val numbers: Map[WeekDay, Int] = values.zipWithIndex.toMap

  def nextDay(day: WeekDay): WeekDay               = values((numbers(day) + 1) % 7)
  def prevDay(day: WeekDay): WeekDay               = values((numbers(day) + 6) % 7)
  def isWeekend(day: WeekDay): Boolean             = day == Saturday || day == Sunday
  def isWeekday(day: WeekDay): Boolean             = !isWeekend(day)
  def daysToWeekend(day: WeekDay): Int             = if (isWeekend(day)) 0 else (numbers(Saturday) - numbers(day))
  def daysBetween(from: WeekDay, to: WeekDay): Int = (numbers(to) - numbers(from) + 7) % 7
  def fromInt(day: Int): WeekDay                   = values((day % 7 + 7) % 7)
  def toInt(day: WeekDay): Int                     = numbers(day)
  def fromNameOpt(day: String): Option[WeekDay]    = values.find(_.toString().equalsIgnoreCase(day))
  def name(day: WeekDay): String                   = day.toString()
}
