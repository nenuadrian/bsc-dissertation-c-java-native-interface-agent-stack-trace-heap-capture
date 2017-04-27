package com.insider.services

import java.net.URLDecoder
import java.util.UUID

import akka.actor.{Actor, ActorLogging, ActorRefFactory, ActorSystem}
import akka.io.IO
import akka.pattern.ask
import akka.util.Timeout
import reactivemongo.bson.BSONObjectID
import spray.can.Http
import spray.http.HttpMethods._
import spray.http.MediaTypes._
import spray.http._
import spray.routing._
import reactivemongo.bson.{BSON, BSONArray, BSONDocument, BSONDocumentReader, BSONDocumentWriter, BSONLong, BSONNull, BSONNumberLike, BSONObjectID, BSONValue}

import scala.concurrent.duration._
import scala.concurrent.{Await, Future}
import spray.httpx.unmarshalling._
import spray.httpx.marshalling._

import com.github.t3hnar.bcrypt._
import com.insider.model._
import com.insider._
import spray.http.HttpHeaders._
import uk.gov.hmrc.emailaddress._
import spray.json._

import com.insider.providers._
import spray.httpx.SprayJsonSupport._
import com.insider.model.JsonProtocols._

trait AppService extends ServiceUtils with Authenticator {
  val appRoutes = prepareResponse { path("log") {
    post { entity(as[JsObject]) { logData => {
      complete {
        val log = logData.convertTo[LogEntry]

        var newLog = Log(app = BSONObjectID.parse(log.app_hash).get, message = log.message.get, log_type = log.log_type)
        val output = log.log_type match {
          case "app-start" => {}
          case "app-stop" => {}
          case "app-ping" => {}
          case "exception" if log.message.isDefined => {
            val message = scala.xml.XML.loadString(log.message.get)
            newLog = newLog.copy(summary = Some((message \ "ex" \ "getMessage")(0).text))
          }
        }

        logsProvider.insert(newLog)

        """<?xml version="1.0"?><root></root>"""
      }
    }}}
  } ~ pathPrefix("in") {
    auth { user =>
      user match {
        case None => complete { APIResponse(done = false, message = "invalid_session") render }
        case Some(user) => {
          pathPrefix("apps") {
            pathPrefix(Segment) { id => {
              Await.result(appProvider.findById(BSONObjectID.parse(id).get), 5.seconds) match {
                case Some(app) if app.user == user.id => {
                  pathPrefix("logs") {
                    path(Segment) { id => {
                      Await.result(logsProvider.findById(BSONObjectID.parse(id).get), 5.seconds) match {
                        case Some(log) => { get { complete {
                          APIResponse(data = log.toJson) render
                        }}}
                        case _ => complete(APIResponse(done = false) render)
                      }
                    }} ~ pathEnd { get { complete {
                      APIResponse(data = Await.result(logsProvider.findByApp(app.id), 5.seconds).toJson) render
                    }}}
                  } ~ pathEnd {
                    post { entity(as[JsObject]) { appData =>
                      complete {
                        val updateApp = appData.convertTo[UpdateApp]
                        app.name = updateApp.name
                        Await.result(appProvider.save(app), 5.seconds)
                        APIResponse() render
                    }}} ~ get { complete {
                      APIResponse(data = app.toJson) render
                    }}
                  }
                }
                case _ => complete(APIResponse(done = false) render)
              }
            }} ~ pathEnd {
              post { complete {
                Await.result(appProvider.insert(new UserApp(user = user.id)), 5.seconds)
                APIResponse() render
              }} ~ get {
                complete {
                  APIResponse(data = Await.result(appProvider.findByUser(user.id), 5.seconds).toArray.toJson) render
                }
              }
            }
          } ~ pathEnd {
            get {
              complete {
                APIResponse(data = user.toJson) render
              }
            }
          }
        }
      }
    }
  }}
}
