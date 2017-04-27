
echo "Running"

#java -jar ./java/output/app.jar
#java -agentpath:CodEvoAgent -jar ./java/output/app.jar

java -agentpath:"./release/CodEvoAgent.dll=api_key>58a06cfd3a602d5d004c7223,app_hash>58a06cfd3a602d5d004c7223" -Dcodevo.Verbose=3 -jar ./release/app.jar
#java -agentpath:./c/agent/output/agent.dll -Dcodevo.Sources=./java/java_test/ -DcodevoVerbose=3 -jar ./java/output/app.jar
