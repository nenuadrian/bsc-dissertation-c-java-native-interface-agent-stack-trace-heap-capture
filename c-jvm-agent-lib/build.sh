echo "Go compile [C] ..."

cd ./c/agent/
sh build.sh
cd ../../

sleep 1

echo "Go compile [JAVA] ..."

cd ./java/
sh build.sh

cd ../
