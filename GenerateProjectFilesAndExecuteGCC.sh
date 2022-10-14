mkdir -p bin
cd bin
git clean -fdX
cmake -G "Unix Makefiles" ..
make
cd JSerializerDemo
./JSerializerDemo