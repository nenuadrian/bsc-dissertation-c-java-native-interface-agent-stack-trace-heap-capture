package com.insider.services

import java.net.URLDecoder
import java.util.UUID
import spray.json._

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

import scala.concurrent.duration._
import scala.concurrent.{Await, Future}
import spray.httpx.unmarshalling._
import spray.httpx.marshalling._

import com.github.t3hnar.bcrypt._
import com.insider.model._
import com.insider._
import spray.http.HttpHeaders._
import uk.gov.hmrc.emailaddress._

import com.insider.providers._

import spray.httpx.SprayJsonSupport._
import com.insider.model.JsonProtocols._
import reactivemongo.bson.{BSON, BSONArray, BSONDocument, BSONDocumentReader, BSONDocumentWriter, BSONLong, BSONNull, BSONNumberLike, BSONObjectID, BSONValue}

trait Authenticator extends ServiceUtils {
  def auth: Directive1[Option[User]] = {
    extract(_.request.headers.find(h => h.name == "auth-token" && h.value.length > 5) match {
        case Some(token: HttpHeader) => Await.result(userProvider.findByToken(token.value), 5.seconds)
        case None => None
    })
  }
}

trait AppAuthenticationService extends ServiceUtils {
  val appAuthRoutes = prepareResponse { pathPrefix("auth") {
    path("authenticate") {
      post {
        entity(as[JsObject]) { loginData =>
          complete {
            val authenticate = loginData.convertTo[Authenticate]
            (Await.result(userProvider.findByUsername(authenticate.username), 5.seconds) match {
              case Some(user) => {
                if (authenticate.password.isBcrypted(user.password)) {
                  val token = generateToken
                  Await.result(userProvider.setToken(user.id, token), 5.seconds)
                  APIResponse(data = AuthResponse(user, token).toJson)
                } else APIResponse(done = false)
              }
              case _ => APIResponse(done = false)
            }) render
          }
        }
      }
    } ~ path("signup") {
      post {
        entity(as[JsObject]) { signupData =>
          complete {
            val signup = signupData.convertTo[Signup]
            (if (!EmailAddress.isValid(signup.email)) {
              APIResponse(done = false, data = JsNumber(3))
            } else if (Await.result(userProvider.findByEmail(signup.email), 5.seconds).isDefined) {
              APIResponse(done = false, data = JsNumber(2))
            } else if (Await.result(userProvider.findByUsername(signup.username), 5.seconds).isDefined) {
              APIResponse(done = false, data = JsNumber(1))
            } else {
              val user = new User(
                username = signup.username,
                email = signup.email,
                password = signup.password.bcrypt,
                token = generateToken)
              Await.result(userProvider.insert(user), 5.seconds)
              APIResponse(data = AuthResponse(user, user.token).toJson)
            }) render
          }
        }
      }
    }
  }}

  private def generateToken = UUID.randomUUID.toString.bcrypt
}
