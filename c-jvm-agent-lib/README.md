# Project Agent

## Run release

```
java -jar app.jar

java -agentpath:CodEvoAgent -jar app.jar

java -agentpath:CodEvoAgent -DcodevoSources=../java/java_test/ -DcodevoVerbose=3 -jar app.jar
```

## Building setup

Useful setup commands to be able to build

```
curl
brew link curl --force


sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils


http://mac-dev-env.patrickbougie.com/openssl/ -lssl

https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption

http://www.xmlsoft.org/downloads.html  xml2-config --libs

brew install libxml2
brew install gpg
```\