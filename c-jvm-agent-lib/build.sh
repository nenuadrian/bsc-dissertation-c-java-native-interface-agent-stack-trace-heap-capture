echo "Go compile [C] ..."

rm hs_err_*.log || true

cd ./c/agent/
sh build.sh
cd ../../

sleep 1

echo "Go compile [JAVA] ..."

cd ./java/
sh build.sh

cd ../

echo "Compiled successful ..."
sleep 2

echo "Releasing.."
rm ./release/*.jar
rm ./release/*.dll
rm ./release/*.log
rm ./release/*.dylib
cp ./java/output/app.jar ./release/app.jar
cp ./c/agent/build/libagent.dylib.dll ./release/CodEvoAgent.dll
cp ./c/agent/build/libagent.dylib.dylib ./release/libCodEvoAgent.dylib

echo "Released!"
sleep 2
