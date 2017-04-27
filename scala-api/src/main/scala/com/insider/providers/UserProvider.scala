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

import scala.concurrent.duration._
import scala.concurrent.{Await, Future}
import spray.httpx.unmarshalling._
import spray.httpx.marshalling._
import com.typesafe.config.Config

import com.github.t3hnar.bcrypt._
import com.insider.model._
import spray.http.HttpHeaders._
import com.insider._
import com.insider.providers._
import reactivemongo.bson.{BSON, BSONArray, BSONDocument, BSONDocumentReader, BSONDocumentWriter, BSONLong, BSONNull, BSONNumberLike, BSONObjectID, BSONValue}
import scala.concurrent._
import ExecutionContext.Implicits.global
import reactivemongo.api.collections.bson.BSONCollection

trait UserDAO extends MongoDAO {
  implicit val collection = getCollection("users")

  def findById(id: BSONObjectID) = collection.find(BSONDocument("_id" -> id)).one[User]
  def findJoinedByIds(ids: Set[BSONObjectID]) = collection.find(BSONDocument("_id" -> BSONDocument("$in" -> ids))).cursor[User]().collect[List]()

  def findByIds(ids: Set[BSONObjectID]) = collection.find(BSONDocument("_id" -> BSONDocument("$in" -> ids))).cursor[User]().collect[List]()

  def findByToken(token: String) = collection.find(BSONDocument("token" -> token)).one[User]
  def findByUsername(username: String) = collection.find(BSONDocument("username" -> username)).one[User]
  def findByEmail(email: String) = collection.find(BSONDocument("email" -> email)).one[User]
  def setToken(id: BSONObjectID, token: String) = collection.update(BSONDocument("_id" -> id), BSONDocument("$set" -> BSONDocument("token" -> token)))

  def insert(entity: User) = collection.insert(entity)
  def save(entity: User) = collection.update(BSONDocument("_id" -> entity.id), BSONDocument("$set" -> entity))
  def update(id: BSONObjectID, data: BSONDocument) = collection.update(BSONDocument("_id" -> id), BSONDocument("$set" -> data))

}

class UserProvider(val config: Config) extends UserDAO {


}
