rem build vs2019
cd %~dp0

cd ..
pushd build_vs2019_*
cmake.exe --build . --config release

cmake -P cmake_install.cmake
popd
pause
