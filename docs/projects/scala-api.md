# Scala API

Path: `scala-api/`

## Purpose

Backend service that ingests/serves captured JVM diagnostic data and provides APIs consumed by the dashboard.

## Stack

- Scala `2.11.8`
- Akka actor system
- Spray (`spray-can`, `spray-routing`, `spray-json`)
- ReactiveMongo

## Key Files

- `build.sbt`: Build definition and dependencies.
- `start.sh`: Starts the API using `sbt run`.
- `src/main/resources/application.conf.example`: Example runtime configuration.

## Typical Commands

```bash
cd scala-api
sbt run
```

## Configuration Notes

The example config includes:

- MongoDB host/database settings
- Spray server limits/timeouts
- SMTP credentials for email-related flows
