rem build vs2019 x64
rem
rem vs2019
rem cmake 3.19.6

cd ..
pushd libs
cmake -DCMAKE_GENERATOR="Visual Studio 16 2019" -DARCHITECTURE=x64 -P buildall.cmake
popd

mkdir build_vs2019_x64
pushd build_vs2019_x64
cmake.exe .. -G "Visual Studio 16 2019"
cmake.exe --build . --config release

cmake -P cmake_install.cmake
popd
pause
