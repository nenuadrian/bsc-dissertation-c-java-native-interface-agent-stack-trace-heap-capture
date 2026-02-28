# Scala API DB

Path: `scala-api-db/`

## Purpose

MongoDB export files used by the API/dashboard data layer.

## Contents

- `apps.bson` and `apps.metadata.json`
- `logs.bson` and `logs.metadata.json`
- `users.bson` and `users.metadata.json`

## Usage

Import data into MongoDB with `mongorestore` as needed for local development or reproducibility testing.

Example:

```bash
mongorestore --db scala-api-db --collection apps scala-api-db/apps.bson
```
