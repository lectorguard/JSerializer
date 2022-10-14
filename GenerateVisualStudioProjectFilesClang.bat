mkdir bin
cd bin
git clean -fdX
cmake -G "Visual Studio 17 2022"  -T ClangCL -A x64 ..

