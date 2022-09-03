mkdir bin
git clean -f -d -x bin/
cd bin
cmake -G "Visual Studio 16 2019" ..
