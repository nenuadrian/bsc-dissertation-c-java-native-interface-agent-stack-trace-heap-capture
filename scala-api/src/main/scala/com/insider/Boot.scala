package com.insider

import akka.actor.{ActorSystem, Props}
import akka.io.IO
import spray.can.Http
import akka.pattern.ask
import akka.util.Timeout
import com.typesafe.config.ConfigFactory

import scala.concurrent.duration._
import com.insider.services._


import akka.actor.OneForOneStrategy
import akka.actor.SupervisorStrategy._
import scala.concurrent.duration._
import akka.actor.ActorInitializationException

object Main extends App {
  implicit val system = ActorSystem("on-spray-can")

  val service = system.actorOf(Props[NGSServiceActor], "api-scala")

  implicit val timeout = Timeout(5.seconds)

  IO(Http) ? Http.Bind(service, interface = "0.0.0.0", port = 8081)
}
