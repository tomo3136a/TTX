rem build vs2019 x64
rem
rem vs2019
rem cmake 3.19.6

cd %~dp0
set ptn=add_subdirectory(../TTX TTX)
set dst=..\TTXAdditional\CMakeLists.txt
find /C "%ptn%" %dst% 2>NUL >NUL
if @%ERRORLEVEL%@==@1@ echo %ptn% >> %dst%

cd ..
pushd libs
if not exist build cmake -DCMAKE_GENERATOR="Visual Studio 16 2019" -DARCHITECTURE=x64 -P buildall.cmake
popd

mkdir build_vs2019_x64
pushd build_vs2019_x64
cmake.exe .. -G "Visual Studio 16 2019"
cmake.exe --build . --config release

cmake -P cmake_install.cmake
popd
pause
