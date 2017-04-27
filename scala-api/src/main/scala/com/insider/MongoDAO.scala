package com.insider

import com.typesafe.config.Config
import reactivemongo.api.{DefaultDB, MongoDriver}
import reactivemongo.api.collections.bson.BSONCollection
import reactivemongo.bson.{BSON, BSONArray, BSONDocument, BSONDocumentReader, BSONDocumentWriter, BSONLong, BSONNull, BSONNumberLike, BSONObjectID, BSONValue}

import scala.collection.JavaConverters._
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.{Await, Future}
import scala.concurrent.duration._

import com.insider.model._
import reactivemongo.api.gridfs.Implicits._
import com.github.t3hnar.bcrypt._

trait MongoDAO {
  implicit val config: Config

  val servers = config.getString("mongodb.server").split(",")
  val driver = new MongoDriver
  val connection = driver.connection(servers)

  val db = Await.result(connection.database(config.getString("mongodb.database")), 5.seconds)
  implicit val collection: BSONCollection

  def getCollection(col: String) = db[BSONCollection](col)
}
