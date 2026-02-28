# C JVM Agent Library

Path: `c-jvm-agent-lib/`

## Purpose

Native JVM agent implementation that captures stack-trace context and recursively inspects object state from live JVM processes.

## Main Parts

- `c/agent/`: C/C++ agent implementation, CMake build files, and native dependencies.
- `java/`: Java test application and build scripts for agent integration testing.
- `build.sh`: Helper script that builds native code and Java parts in sequence.

## Typical Commands

Build both native agent and Java app:

```bash
cd c-jvm-agent-lib
sh build.sh
```

Run Java app with native agent:

```bash
java -agentpath:CodEvoAgent -jar app.jar
```

Verbose mode with source path:

```bash
java -agentpath:CodEvoAgent -DcodevoSources=../java/java_test/ -DcodevoVerbose=3 -jar app.jar
```
