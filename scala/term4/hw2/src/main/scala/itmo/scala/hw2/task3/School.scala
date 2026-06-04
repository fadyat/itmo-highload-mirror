package itmo.scala.hw2.task3

object School {

  /** Сформируйте журнал с оценками по предметам для каждого школьника.
    *
    * Note: школьники могут пересдать задание, и получить повышенную оценку, в исходном списке могут содержаться
    * несколько записей для одного и того же ученика для одного и того же предмета и задания - необходимо выбирать
    * максимальную оценку.
    *
    * @param rows
    *   список строк с оценками
    * @return
    *   журнал оценок для каждого студента
    */
  def calculateGrades(rows: List[Row]): Map[Student, Map[Subject, List[Grade]]] = {
    rows
      .groupMapReduce(r => (r.student, r.subject, r.taskName))(_.grade)(Grade.max)
      .groupMap { case ((student, subject, _), _) => (student, subject) } { case ((_, _, _), grade) => grade }
      .groupMap { case ((student, _), _) => student } { case ((_, subject), grades) => (subject, grades) }
      .view
      .mapValues(_.map { case (subject, grades) => subject -> grades.toList }.toMap)
      .toMap
  }

  /** Для каждого предмета определите минимальную оценку, которую получил ученик.
    *
    * Note: необходимо учитывать пересдачи и выбирать минимальную оценку после пересдачи.
    *
    * @param rows
    *   список строк с оценками
    * @return
    *   минимальные оценки полученные для каждого предмета
    */
  def minGradeForSubject(rows: List[Row]): Map[Subject, Grade] =
    rows.groupMapReduce(_.subject)(_.grade)(Grade.min)
}
