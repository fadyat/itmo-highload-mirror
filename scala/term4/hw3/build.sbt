import org.typelevel.scalacoptions.{ScalaVersion, ScalacOption, ScalacOptions}

ThisBuild / version                                       := "0.1.0-SNAPSHOT"
ThisBuild / scalaVersion                                  := "2.13.18"
ThisBuild / scalafixDependencies += "org.typelevel"       %% "typelevel-scalafix" % "0.5.0"
ThisBuild / scalafixDependencies += "com.github.vovapolu" %% "scaluzzi"           % "0.1.23"
ThisBuild / semanticdbEnabled                             := true
// explicitly set the version of semanticdb, because by default it's `4.9.9` which is not supports scala `2.13.18`
ThisBuild / semanticdbVersion := scalafixSemanticdb("4.15.2").revision

lazy val root: Project =
  project
    .settings(name := "hw3-typeclass")
    .aggregate(
      `library-source`,
      `hw-source`,
    )

lazy val `library-source`: Project = (project in file("./library-source"))
  .disablePlugins(ScalafixPlugin)
  .settings(
    name := "library-source",
    libraryDependencies ++= List(
      Dependencies.scalaTest  % Test,
      Dependencies.scalaCheck % Test,
    ),
    tpolecatExcludeOptions += ScalacOptions.warnNonUnitStatement,
    tpolecatScalacOptions += ScalacOption(
      "-Wconf:cat=lint-infer-any&msg=kind-polymorphic:s",
      _.isBetween(ScalaVersion.V2_13_0, ScalaVersion.V3_0_0),
    ),
    addCompilerPlugin(Dependencies.kindProjector),
    addCompilerPlugin(Dependencies.bmFor),
  )

lazy val `hw-source`: Project = (project in file("./hw-source"))
  .settings(
    name := "hw-source",
    libraryDependencies ++= List(
      Dependencies.scalaTest  % Test,
      Dependencies.scalaCheck % Test,
    ),
    tpolecatExcludeOptions += ScalacOptions.warnNonUnitStatement,
    tpolecatScalacOptions += ScalacOption(
      "-Wconf:cat=lint-infer-any&msg=kind-polymorphic:s",
      _.isBetween(ScalaVersion.V2_13_0, ScalaVersion.V3_0_0),
    ),
    addCompilerPlugin(Dependencies.kindProjector),
    addCompilerPlugin(Dependencies.bmFor),
  )
  .dependsOn(`library-source`)
