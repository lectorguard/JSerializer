mkdir -p bin
cd bin
git clean -fdX
export CC=/usr/bin/clang-12
export CXX=/usr/bin/clang++-12
cmake -G "Unix Makefiles" ..
make
cd JSerializerDemo
./JSerializerDemo