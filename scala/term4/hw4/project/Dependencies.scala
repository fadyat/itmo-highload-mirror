import sbt.*

object Dependencies {
  val kindProjector = ("org.typelevel" %% "kind-projector"     % "0.13.4").cross(CrossVersion.full)
  val bmFor         = "com.olegpy"     %% "better-monadic-for" % "0.3.1"

  val catsCore       = "org.typelevel"     %% "cats-core"            % "2.13.0"
  val catsLaws       = "org.typelevel"     %% "cats-laws"            % "2.13.0"
  val disciplineTest = "org.typelevel"     %% "discipline-scalatest" % "2.3.0"
  val scalaTest      = "org.scalatest"     %% "scalatest"            % "3.2.20"
  val scalaCheck     = "org.scalatestplus" %% "scalacheck-1-19"      % "3.2.20.0"
}
