@echo off
rem build vs2019 x64
rem
rem vs2019
rem cmake 3.19.6

set TOOL="Visual Studio 16 2019"
rem set ARCH=Win32
set ARCH=x64

cd %~dp0
find /C " TTX)" ..\TTXAdditional\CMakeLists.txt 2>NUL >NUL
if @%ERRORLEVEL%@==@1@ type plugins.txt >> ..\TTXAdditional\CMakeLists.txt

cd ..
pushd libs
if not exist build cmake -DCMAKE_GENERATOR=%TOOL% -DARCHITECTURE=%ARCH% -P buildall.cmake
popd

mkdir build_%ARCH%
pushd build_%ARCH%
cmake.exe .. -G %TOOL% -A %ARCH%
cmake.exe --build . --config release

cmake -P cmake_install.cmake
popd
pause
