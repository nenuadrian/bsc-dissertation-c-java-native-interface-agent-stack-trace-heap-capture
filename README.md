# BSc Dissertation Project - University of Manchester - Non-intrusive JVM agents for capturing the internal state of live production applications

[![CMake on a single platform](https://github.com/nenuadrian/c-java-native-interface-agent-stack-trace-heap-capture/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/nenuadrian/c-java-native-interface-agent-stack-trace-heap-capture/actions/workflows/cmake-single-platform.yml)
[![LaTeX Build](https://github.com/nenuadrian/bsc-dissertation-c-java-native-interface-agent-stack-trace-heap-capture/actions/workflows/main.yml/badge.svg)](https://github.com/nenuadrian/bsc-dissertation-c-java-native-interface-agent-stack-trace-heap-capture/actions/workflows/main.yml)

## Overview

This project implements a non-intrusive JVM agent capable of capturing the internal state of live production applications. The agent is written in C and utilizes the Java Native Interface (JNI) to interact with the Java Virtual Machine (JVM). It recursively captures the values of objects within methods on a stack trace, providing valuable debugging information without impacting application performance. [cite: 23, 24, 25, 55, 56, 57]

![design](DISSERTATION/end-design.png)

## Key Features

* **Non-intrusive:** The agent operates independently of the JVM, minimizing performance overhead and avoiding any interference with the application's normal execution. 
* **Cross-Platform Compatibility:** The agent is designed to be compatible with various operating systems, including macOS, Windows, and Linux.
* **Recursive Object Capture:** The agent can recursively capture the values of objects on the heap, providing a comprehensive snapshot of the application's state at the time of an exception.
* **Efficient Data Handling:** The agent employs data compression techniques to reduce the amount of data transmitted and stored, improving efficiency and minimizing resource usage.

## Technologies Used

* **JNI (Java Native Interface):** Enables the C agent to interact with the JVM and access its internal state.
* **JVMTI (JVM Tool Interface):** Provides additional capabilities for monitoring and debugging the JVM.
* **Scala:** Used to implement the API server, which handles data communication between the agent and the dashboard. 
* **Spray:** A Scala library used for building RESTful APIs, facilitating communication between the agent and the dashboard.
* **MongoDB:** A NoSQL database used for storing the captured data.
* **FuelPHP:** A PHP framework used to develop the dashboard for visualizing and exploring the captured data.
* **Miniz:** A data compression library used to optimize data transfer and storage.
* **OpenSSL:** A cryptography library used for data encryption and security. 

## Installation and Usage

Refer to the project's README for detailed instructions on building and deploying the agent, API server, and dashboard.

## Note

This implementation is a Minimum Viable Product (MVP) and is not intended for production use without further development and hardening.

## Disclaimer

This project was developed as part of academic research. No guarantees or maintenance are provided.

## Cite

```
@article{jniAgents,
	author = {Adrian Nenu and Caroline Jay},
	title = {{Non-intrusive JVM agents for capturing the internal state of live production applications}},
	url = {https://github.com/nenuadrian/bsc-dissertation-c-java-native-interface-agent-stack-trace-heap-capture},
	year = 2017
}
```
