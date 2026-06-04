package itmo.scala.hw2.task2

import org.scalacheck.Gen.chooseNum
import org.scalacheck.Prop.forAll
import org.scalacheck.{Gen, Properties}

object CollectionsPropSpec extends Properties("Collections") {

  val listOfLasNames: List[String] =
    List(
      "Ivanov",
      "Petrov",
      "Kuznetsov",
      "Pavlov",
      "Kozlov",
      "Kozlovsky",
    )

  val listOfFirstNames: List[String] =
    List(
      "Ivan",
      "Petr",
      "Alexey",
      "Andrey",
    )

  val listOfMiddleNames: List[String] =
    List(
      "Petrovich",
      "Sidorovich",
      "Alexeevich",
      "Sergeevich",
    )

  implicit private val genUser: Gen[User] =
    for {
      lastName   <- Gen.oneOf(Gen.oneOf(listOfLasNames), Gen.oneOf(listOfLasNames), Gen.alphaStr)
      firstName  <- Gen.oneOf(Gen.oneOf(listOfFirstNames), Gen.oneOf(listOfFirstNames), Gen.alphaStr)
      middleName <- Gen.oneOf(Gen.oneOf(listOfMiddleNames), Gen.oneOf(listOfMiddleNames), Gen.alphaStr)
      age        <- chooseNum[Int](1, 120)
    } yield User(
      lastName = lastName,
      firstName = firstName,
      middleName = middleName,
      age = age,
    )

  property("sortUsers: sort users by last name, age, first name, middle name") =
    forAll(Gen.nonEmptyListOf(genUser)) { users: List[User] =>
      val sortedUsers = Collections.sortUsers(users)
      sortedUsers.zip(sortedUsers.tail).forall {
        case (User(lastNameL, _, _, _), User(lastNameR, _, _, _)) if lastNameL > lastNameR         => false
        case (User(lastNameL, _, _, _), User(lastNameR, _, _, _)) if lastNameL < lastNameR         => true
        case (User(_, _, _, ageL), User(_, _, _, ageR)) if ageL > ageR                             => true
        case (User(_, _, _, ageL), User(_, _, _, ageR)) if ageL < ageR                             => false
        case (User(_, firstNameL, _, _), User(_, firstNameR, _, _)) if firstNameL > firstNameR     => false
        case (User(_, firstNameL, _, _), User(_, firstNameR, _, _)) if firstNameL < firstNameR     => true
        case (User(_, _, middleNameL, _), User(_, _, middleNameR, _)) if middleNameL > middleNameR => false
        case (User(_, _, middleNameL, _), User(_, _, middleNameR, _)) if middleNameL < middleNameR => true
        case (l, r)                                                                                => l == r
      }
    }

  property("count: zip str") = forAll(Gen.numStr) { originalStr: String =>
    val zipStr = Collections.count(originalStr)
    zipStr.flatMap({ case (c, n) => List.fill(n)(c) }).mkString == originalStr
  }

  property("findGaps") = forAll(Gen.nonEmptyListOf(chooseNum[Int](-100, 10000))) { originalSeq: List[Int] =>
    val preparedInts = originalSeq.distinct.sorted

    val gaps                 = Collections.findGaps(preparedInts)
    val gapsShouldBeNonEmpty = gaps.forall(_.forall { case (l, r) => l < (r - 1) })

    val missingInts     = gaps.toList.flatten.flatMap { case (l, r) => List.range(l + 1, r) }
    val intersectedInts = preparedInts.intersect(missingInts)

    gapsShouldBeNonEmpty &&
    intersectedInts.isEmpty &&
    (preparedInts.size + missingInts.size) == (preparedInts.max - preparedInts.min + 1)
  }

}
