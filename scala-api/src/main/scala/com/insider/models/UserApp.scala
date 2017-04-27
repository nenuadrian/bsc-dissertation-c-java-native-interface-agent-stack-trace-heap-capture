package com.insider.model

import reactivemongo.bson.{BSON, BSONArray, BSONDocument, BSONInteger, BSONDocumentReader, BSONDocumentWriter, BSONLong, BSONNull, BSONNumberLike, BSONObjectID, BSONValue}
import java.util.Date


case class UpdateApp(name: String)

class UserApp(
  val id: BSONObjectID = BSONObjectID.generate,
  val user: BSONObjectID,
  var name: String = "Untitled",
  val created_at: Date = new Date()
)

trait UserAppBSON {
  implicit object UserAppBSONReader extends BSONDocumentReader[UserApp] {
    def read(doc: BSONDocument) = new UserApp(
        id = doc.getAs[BSONObjectID]("_id").get,
        user = doc.getAs[BSONObjectID]("user_id").get,
        name = doc.getAs[String]("name").get,
        created_at = doc.getAs[Date]("created_at").get
      )
  }

  implicit object UserAppBSONWriter extends BSONDocumentWriter[UserApp] {
    def write(obj: UserApp) = BSONDocument(
      "_id" -> obj.id,
      "user_id" -> obj.user,
      "name" -> obj.name,
      "created_at" -> obj.created_at
      )
  }
}


object UserApp extends UserAppBSON  {}
