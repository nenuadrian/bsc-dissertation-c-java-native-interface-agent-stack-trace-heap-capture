package com.insider.model

import reactivemongo.api.collections.bson.BSONCollection
import reactivemongo.bson.{BSON, BSONArray, BSONDocument, BSONInteger, BSONDocumentReader, BSONDocumentWriter, BSONLong, BSONNull, BSONNumberLike, BSONObjectID, BSONValue}
import spray.json._
import com.insider._
import scala.concurrent._
import ExecutionContext.Implicits.global


class User(
  val id: BSONObjectID = BSONObjectID.generate,
  val username: String,
  var password: String = null,
  var email: String,
  var token: String = ""
)

case class Authenticate(username: String, password: String)
case class AuthResponse(user: User = null, token: String)
case class Signup(email: String, username: String, password: String)


trait UserBSON {
  implicit object UserBSONReader extends BSONDocumentReader[User] {
    def read(doc: BSONDocument) = {
      new User(
        id = doc.getAs[BSONObjectID]("_id").get,
        username = doc.getAs[String]("username").get,
        email = doc.getAs[String]("email").get,
        password = doc.getAs[String]("password").get,
        token = doc.getAs[String]("token").get
      )
    }
  }

  implicit object UserBSONWriter extends BSONDocumentWriter[User] {
    def write(obj: User) =
      BSONDocument(
        "_id" -> obj.id,
        "username" -> obj.username,
        "password" -> obj.password,
        "email" -> obj.email,
        "token" -> obj.token
      )
  }
}


object User extends UserBSON {}
