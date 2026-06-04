import sbt.*

object Dependencies {
  val kindProjector = ("org.typelevel" %% "kind-projector"     % "0.13.4").cross(CrossVersion.full)
  val bmFor         = "com.olegpy"     %% "better-monadic-for" % "0.3.1"

  val scalaTest  = "org.scalatest"     %% "scalatest"       % "3.2.19"
  val scalaCheck = "org.scalatestplus" %% "scalacheck-1-18" % "3.2.19.0"
}
