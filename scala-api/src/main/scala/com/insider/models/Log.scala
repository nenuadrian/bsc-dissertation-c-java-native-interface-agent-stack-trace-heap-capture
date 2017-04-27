package com.insider.model

import reactivemongo.bson.{BSON, BSONArray, BSONDocument, BSONInteger, BSONDocumentReader, BSONDocumentWriter, BSONLong, BSONNull, BSONNumberLike, BSONObjectID, BSONValue}
import java.util.Date

case class LogEntry(
  api_key: String,
  log_type: String,
  message: Option[String] = None,
  app_hash: String
)

case class Log(
  id: BSONObjectID = BSONObjectID.generate,
  log_type: String,
  message: String,
  app: BSONObjectID,
  created_at: Date = new Date(),
  summary: Option[String] = None
)

trait LogBSON {
  implicit object LogBSONReader extends BSONDocumentReader[Log] {
    def read(doc: BSONDocument) = new Log(
        id = doc.getAs[BSONObjectID]("_id").get,
        log_type = doc.getAs[String]("log_type").get,
        message = doc.getAs[String]("message").get,
        summary = doc.getAs[String]("summary"),
        app = doc.getAs[BSONObjectID]("app_id").get,
        created_at = doc.getAs[Date]("created_at").get
      )
  }

  implicit object LogBSONWriter extends BSONDocumentWriter[Log] {
    def write(obj: Log) = BSONDocument(
      "_id" -> obj.id,
      "log_type" -> obj.log_type,
      "app_id" -> obj.app,
      "message" -> obj.message,
      "summary" -> obj.summary,
      "created_at" -> obj.created_at
      )
  }
}


object Log extends LogBSON  {}
