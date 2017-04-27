organization  := "com.cardinal"

version       := "0.1"

scalaVersion  := "2.11.8"

scalacOptions := Seq("-unchecked", "-deprecation", "-encoding", "utf8")
fork in Test := true // allow to apply extra setting to Test
parallelExecution in Test := false

resolvers += "Typesafe repository releases" at "http://repo.typesafe.com/typesafe/releases/"
resolvers += "softprops-maven" at "http://dl.bintray.com/content/softprops/maven"
resolvers += Resolver.bintrayRepo("hmrc", "releases")
resolvers += "Redhat GA" at "https://maven.repository.redhat.com/ga/"
libraryDependencies ++= {
  val akkaV = "2.3.9"
  val sprayV = "1.3.3"
  Seq(
     "commons-codec" % "commons-codec" % "1.10.0.redhat-5",
    "uk.gov.hmrc"         %% "emailaddress" % "2.0.0",
    "io.spray"            %%  "spray-json"    % "1.3.2",
    "me.lessis"           %% "courier" % "0.1.3",
    "com.github.t3hnar"   %%  "scala-bcrypt"  % "3.0",
    "io.spray"            %%  "spray-can"     % sprayV,
    "io.spray"            %%  "spray-routing" % sprayV,
    "io.spray"            %%  "spray-testkit" % sprayV  % "test",
    "com.typesafe.akka"   %%  "akka-actor"    % akkaV,
    "com.typesafe.akka"   %%  "akka-testkit"  % akkaV   % "test",
    "org.specs2"          %%  "specs2-core"   % "2.3.11" % "test",
    "com.github.simplyscala" %% "scalatest-embedmongo" % "0.2.2" % "test",
    "io.spray"            %%  "spray-client"  % "1.3.2",
    "org.reactivemongo"   %%  "reactivemongo" % "0.11.14",
    "org.slf4j"           %   "slf4j-simple"   % "1.7.12",
    "org.slf4j"           %   "slf4j-api"   % "1.7.12",
    "com.github.philcali" %% "cronish" % "0.1.3"
  )
}
