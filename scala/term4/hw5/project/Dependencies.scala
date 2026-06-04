import sbt.*

object Dependencies {
  val kindProjector = ("org.typelevel" %% "kind-projector"     % "0.13.4").cross(CrossVersion.full)
  val bmFor         = "com.olegpy"     %% "better-monadic-for" % "0.3.1"

  val catsEffect  = "org.typelevel"     %% "cats-effect"                   % "3.7.0"
  val scalaTest   = "org.scalatest"     %% "scalatest"                     % "3.2.20"
  val scalaCheck  = "org.scalatestplus" %% "scalacheck-1-19"               % "3.2.20.0"
  val catsTest    = "org.typelevel"     %% "cats-effect-testing-scalatest" % "1.8.0"
  val catsTestkit = "org.typelevel"     %% "cats-effect-testkit"           % "3.7.0"
}
