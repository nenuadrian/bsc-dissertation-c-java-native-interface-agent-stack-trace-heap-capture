# 3rd year Dissertation Project - University of Manchester

The commencement of a C JNI Agent, designed to recursively capture the values of objects within methods on a stack trace.

To use, it is necessary to install the libxml2 library, as well as Jansson, crypto, and openssl.

The resulting library files can be incorporated using the -agentpath flag in the java command.

This agent interacts with a basic Scala Spray server, which records the captured values in MongoDB. To configure the server, modify the src/main/resources/application.conf file accordingly, then build and execute it using sbt run.

Additionally, a FuelPHP dashboard is utilized, which communicates with the Scala Spray server to present the analysis results. Ensure you perform a composer install to set it up.

It is important to note that this implementation is not stable, secure, or optimized, as it serves as a minimum viable product (MVP). However, you can extract relevant code snippets from it, particularly those demonstrating communication with the JNE.

Please bear in mind that this project was an experimental endeavor conducted at the university level.
