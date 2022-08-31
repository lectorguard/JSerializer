mkdir bin
cd bin
git clean -f -d -x
cmake -G "Visual Studio 16 2019" ..
