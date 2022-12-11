# 3rd year Dissertation Project - University of Manchester

The beginning of a C JNI Agent which can capture recursively the values of objects inside the methods on a stack-trace

To build the C part you need to install the libxml2 library and Jansson and crypto and openssl.

The resulting library files can be attached using the -agentpath in the java command.

Communicates with an elementary Scala Spray server recording values in MongoDB (configure src/main/resources/application.conf from sample, build and run with sbt run)

Uses a FuelPHP dashboard which communicated with the Scala Spray Server to show results of analysis (do a composer install)

No stable, not secure or optimised.

This is an MVP from which you could use code-bits for now, especially on how to communicate with the JNE.

This was an university experimental project.
