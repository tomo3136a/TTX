@echo off
rem build vs2019 Win32
rem
rem vs2019
rem cmake 3.19.6

set TOOL="Visual Studio 16 2019"
set ARCH=Win32
rem set ARCH=x64


cd %~dp0
rem goto L1
find /C " TTX)" ..\TTXAdditional\CMakeLists.txt 2>NUL >NUL
if not @%ERRORLEVEL%@==@1@ goto L1
echo Å°Å°Å°Å° update TTXAdditional\CMakeLists.txt Å°Å°Å°Å°
type plugins.cmake >> ..\TTXAdditional\CMakeLists.txt
echo.
:L1
cd ..

rem goto L2
if exist libs\build goto L2
pushd libs
echo Å°Å°Å°Å° build libs Å°Å°Å°Å°
cmake -DCMAKE_GENERATOR=%TOOL% -DARCHITECTURE=%ARCH% -P buildall.cmake
echo.
popd
:L2

rem goto L3
if not exist build mkdir build
pushd build
echo Å°Å°Å°Å° build configure Å°Å°Å°Å°
cmake.exe .. -G %TOOL% -A %ARCH%
echo.
popd
:L3

rem goto L4
if not exist build goto L4
pushd build
echo Å°Å°Å°Å° build release Å°Å°Å°Å°
cmake.exe --build . --config release
echo.
popd
:L4

rem goto L5
if not exist build goto L5
pushd build
echo Å°Å°Å°Å° install Å°Å°Å°Å°
cmake -P cmake_install.cmake
echo.
popd
:L5

:end
pause
