package com.insider.model

import spray.json._
import com.insider.services._
import reactivemongo.bson.{BSON, BSONArray, BSONDocument, BSONDocumentReader, BSONDocumentWriter, BSONLong, BSONNull, BSONNumberLike, BSONObjectID, BSONValue}
import java.text.SimpleDateFormat
import java.text.DateFormat
import java.util.TimeZone

object JsonProtocols extends DefaultJsonProtocol {
  val dfMDYHM = new SimpleDateFormat("MM-dd-yyyy HH:mm")
  dfMDYHM.setTimeZone(TimeZone.getTimeZone("UTC"))
  val dfMDY = new SimpleDateFormat("MM-dd-yyyy")
  dfMDY.setTimeZone(TimeZone.getTimeZone("UTC"))

  implicit object BSONObjectIDJsonFormat extends JsonFormat[BSONObjectID] {
    override def write(obj: BSONObjectID) = JsString(obj.stringify)
    def read(value: JsValue) = null
  }

  implicit val authenticateFormat        = jsonFormat2(Authenticate)
  implicit val signupFormat              = jsonFormat3(Signup)
  implicit val updateAppFormat           = jsonFormat1(UpdateApp)
  implicit val signupLogEntryFormat      = jsonFormat4(LogEntry)

  implicit object UserAppJsonFormat extends JsonFormat[UserApp] {
    override def write(obj: UserApp) = JsObject(
      "id" -> obj.id.toJson,
      "name" -> JsString(obj.name),
      "created_at" -> JsString(dfMDYHM.format(obj.created_at))
    )
    def read(value: JsValue) = null
  }

  implicit object LogJsonFormat extends JsonFormat[Log] {
    override def write(obj: Log) = JsObject(
      "id" -> obj.id.toJson,
      "message" -> JsString(obj.message),
      "summary" -> obj.summary.toJson,
      "log_type" -> JsString(obj.log_type),
      "created_at" -> JsString(dfMDYHM.format(obj.created_at))
    )
    def read(value: JsValue) = null
  }

  implicit object UserJsonFormat extends JsonFormat[User] {
    override def write(obj: User) = JsObject(
      "id" -> obj.id.toJson,
      "username" -> JsString(obj.username),
      "email" -> JsString(obj.email)
    )
    def read(value: JsValue) = null
  }

  implicit object AuthResponseJsonFormat extends JsonFormat[AuthResponse] {
    override def write(obj: AuthResponse) = JsObject(
      "user" -> obj.user.toJson,
      "token" -> JsString(obj.token)
    )
    def read(value: JsValue) = {
      value.asJsObject.getFields("token") match {
        case Seq(JsString(token)) =>
          new AuthResponse(token = token)
        case _ => throw new DeserializationException("Meh")
      }
    }
  }


}
