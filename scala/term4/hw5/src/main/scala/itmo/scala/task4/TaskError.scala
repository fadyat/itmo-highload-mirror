package itmo.scala.task4

/** Неуспешный результат выполнения задачи message - оправдание, которым пользуется разработчик если задача не была
  * сделана.
  */
sealed trait TaskError {
  def message: String
}

object TaskError {

  /** Во время выполнения задачи возникли непреодолимые трудности
    */
  case class ResultError(message: String) extends TaskError

  /** Во время выполнения задачи возникли неожидаемые непреодолимые трудности
    */
  case class UnexpectedError(cause: Throwable) extends TaskError {
    override def message: String = cause.getMessage
  }

  /** Дедлайн прошел, задача перестала быть актуальной
    */
  case object Timeout extends TaskError {
    override val message = "timeout"
  }
}
