package com.insider

import java.net.URLDecoder
import java.util.UUID
import java.text.SimpleDateFormat
import java.text.DateFormat
import java.util.TimeZone

import akka.actor._
import akka.io.IO
import akka.pattern.ask
import akka.util.Timeout
import reactivemongo.bson.BSONObjectID
import spray.can.Http
import spray.http.HttpMethods._
import spray.http.MediaTypes._
import spray.http._
import spray.json._
import spray.routing._

import scala.concurrent.duration._
import scala.concurrent.{Await, Future}
import spray.httpx.unmarshalling._
import spray.httpx.marshalling._

import com.github.t3hnar.bcrypt._
import com.insider.model._
import spray.http.HttpHeaders._
import com.insider._
import com.insider.providers._
import spray.util.LoggingContext
import com.insider.services._
import java.io.StringWriter
import java.io.PrintWriter
import com.typesafe.config.Config
import com.typesafe.config.ConfigFactory

import akka.actor.OneForOneStrategy
import akka.actor.SupervisorStrategy._
import cronish._
import cronish.dsl._

case class APIResponse(
  done:    Boolean = true,
  data:    JsValue = JsNull,
  message: String = ""
) {
  def render(): String = {
    JsObject("done" -> JsBoolean(done), "data" -> data, "message" -> JsString(message)).prettyPrint
  }
}

class NGSServiceActor extends Actor with NGSService {
  def actorRefFactory = context
  def receive = runRoute(routes)
}

trait NGSService extends ServiceUtils
  with AppAuthenticationService with AppService {
  implicit def executionContext = actorRefFactory.dispatcher

  val routes = {
    appAuthRoutes ~ appRoutes ~ path("") {
      complete ("It works! But you shouldn' be here!")
    }
  }
}

trait ServiceUtils extends HttpService with CORSSupport {
  val config                = ConfigFactory.load()
  val userProvider          = new UserProvider(config = config)
  val logsProvider          = new LogsProvider(config = config)
  val emailProvider         = new EmailProvider(config = config)
  val appProvider           = new AppProvider(config = config)

  val dfMDYHM = new SimpleDateFormat("MM-dd-yyyy HH:mm")
  val dfMDY = new SimpleDateFormat("MM-dd-yyyy")

  dfMDYHM.setTimeZone(TimeZone.getTimeZone("UTC"))
  dfMDY.setTimeZone(TimeZone.getTimeZone("UTC"))

  val myRejectionHandler = RejectionHandler {
    case _ => complete(500, APIResponse(done = false, message = "Request rejected by server").render)
  }

  def getStackTraceAsString(t: Throwable) = {
      val sw = new StringWriter
      t.printStackTrace(new PrintWriter(sw))
      sw.toString
  }

  def myExceptionHandler(implicit log: LoggingContext) = ExceptionHandler.apply {
    case e: Exception => ctx => {
      println(getStackTraceAsString(e))
      ctx.complete(500, APIResponse(done = false, message = e.getMessage).render)
    }
  }

  def prepareResponse(routes: => Route): Route = {
    return handleExceptions(myExceptionHandler) { cors { respondWithMediaType(`application/json`) { routes } } }
  }
}

trait CORSSupport {
  this: HttpService =>

  private val allowOriginHeader = `Access-Control-Allow-Origin`(AllOrigins)
  private val optionsCorsHeaders = List(
    `Access-Control-Allow-Headers`("Auth-Token, Origin, X-Requested-With, Content-Type, Accept, Accept-Encoding, Accept-Language, Host, Referer, User-Agent, Access-Control-Request-Headers"),
    `Access-Control-Request-Headers`("Auth-Token, Origin, X-Requested-With, Content-Type, Accept, Accept-Encoding, Accept-Language, Host, Referer, User-Agent"),
    `Access-Control-Max-Age`(1728000),
    `Access-Control-Allow-Methods`(HttpMethods.GET, HttpMethods.POST, HttpMethods.OPTIONS, HttpMethods.DELETE, HttpMethods.PUT))

  def cors[T]: Directive0 = mapRequestContext { ctx => ctx.withRouteResponseHandling({
    //It is an option requeset for a resource that responds to some other method
    case Rejected(x) if (ctx.request.method.equals(HttpMethods.OPTIONS) && !x.filter(_.isInstanceOf[MethodRejection]).isEmpty) => {
      val allowedMethods: List[HttpMethod] = x.filter(_.isInstanceOf[MethodRejection]).map(rejection=> {
        rejection.asInstanceOf[MethodRejection].supported
      })
      ctx.complete(HttpResponse().withHeaders(
        `Access-Control-Allow-Methods`(OPTIONS, allowedMethods :_*) ::  allowOriginHeader ::
         optionsCorsHeaders
      ))
    }
  }).withHttpResponseHeadersMapped { headers =>
    (allowOriginHeader :: headers) ::: optionsCorsHeaders
  }}
}
