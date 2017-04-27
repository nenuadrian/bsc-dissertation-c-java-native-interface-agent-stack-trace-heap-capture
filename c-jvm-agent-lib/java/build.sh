javac -g -d ./java_test_output/ ./java_test/org/codevolution/*.java

cp ./java_test/org/codevolution/*.java ./java_test_output/org/codevolution/

cd ./java_test_output/
jar cvfm ../output/app.jar ../MANIFEST.MF .

