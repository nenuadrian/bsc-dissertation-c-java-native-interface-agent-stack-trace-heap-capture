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


trait AppDAO extends MongoDAO {
  val collection = db[BSONCollection]("apps")

  def insert(entity: UserApp) = collection.insert(entity)
  def findById(id: BSONObjectID) = collection.find(BSONDocument("_id" -> id)).one[UserApp]
  def findByUser(id: BSONObjectID) = collection.find(BSONDocument("user_id" -> id)).cursor[UserApp]().collect[List]()

  def save(entity: UserApp) = collection.update(BSONDocument("_id" -> entity.id), BSONDocument("$set" -> entity))
}

class AppProvider(val config: Config) extends AppDAO {}
