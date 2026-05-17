@echo off
rem build vs2026 x64
rem
rem vs2026
rem cmake 4.2.3

set TOOL="Visual Studio 18 2026"
rem set ARCH=Win32
set ARCH=x64


cd %~dp0
rem goto L1
find /C " TTX)" ..\TTXAdditional\CMakeLists.txt 2>NUL >NUL
if not @%ERRORLEVEL%@==@1@ goto L1
echo ==== update TTXAdditional\CMakeLists.txt ====
type plugins.cmake >> ..\TTXAdditional\CMakeLists.txt
echo.
:L1
cd ..

rem goto L2
if exist libs\build goto L2
pushd libs
echo ==== build libs ====
cmake -DCMAKE_GENERATOR=%TOOL% -DARCHITECTURE=%ARCH% -P buildall.cmake
echo.
popd
:L2

rem goto L3
if not exist build mkdir build
pushd build
echo ==== build configure ====
cmake.exe .. -G %TOOL% -A %ARCH%
echo.
popd
:L3

rem goto L4
if not exist build goto L4
pushd build
echo ==== build release ====
cmake.exe --build . --config release
echo.
popd
:L4

rem goto L5
if not exist build goto L5
pushd build
echo ==== install ====
cmake -P cmake_install.cmake
echo.
popd
:L5

:end
pause
