package com.insider.providers

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
import spray.json._
import spray.routing._
import com.typesafe.config.Config
import spray.json._

import scala.concurrent.duration._
import scala.concurrent.{Await, Future}
import spray.httpx.unmarshalling._
import spray.httpx.marshalling._

import com.github.t3hnar.bcrypt._
import com.insider.model._
import spray.http.HttpHeaders._
import com.insider._
import com.insider.providers._
import reactivemongo.api.collections.bson.BSONCollection
import reactivemongo.bson.{BSON, BSONArray, BSONDocument, BSONDocumentReader, BSONDocumentWriter, BSONLong, BSONNull, BSONNumberLike, BSONObjectID, BSONValue}
import scala.concurrent._
import ExecutionContext.Implicits.global


trait LogsDAO extends MongoDAO {
  val collection = db[BSONCollection]("logs")

  def findById(id: BSONObjectID) = collection.find(BSONDocument("_id" -> id)).one[Log]
  def findByApp(id: BSONObjectID) = collection.find(BSONDocument("app_id" -> id)).cursor[Log]().collect[List]()
  def insert(entity: Log) = collection.insert(entity)
}

class LogsProvider(val config: Config) extends LogsDAO {}
