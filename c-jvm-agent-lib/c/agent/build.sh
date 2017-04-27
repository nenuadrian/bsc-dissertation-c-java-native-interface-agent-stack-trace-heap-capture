cd ./output/

#win32 vs darwin
g++ -c -I "/usr/local/opt/libxml2/include/libxml2/" -I "/usr/local/opt/openssl/include" -I "."  -I "../jdk1.8.0_91.jdk/Contents/Home/include" -I "../jdk1.8.0_91.jdk/Contents/Home/include/darwin" ../code/*.cpp

g++ -lssl -lxml2 -lcrypto -lcurl -ljansson -o agent.dll *.o
g++ -lssl -lxml2 -lcrypto -dynamiclib -ljansson -lcurl -o agent.dylib  *.o

rm *.o
