rem build vs2019 Win32
rem
rem vs2019
rem cmake 3.19.6

cd %~dp0
find /C " TTX)" ..\TTXAdditional\CMakeLists.txt 2>NUL >NUL
if @%ERRORLEVEL%@==@1@ type plugins.txt >> ..\TTXAdditional\CMakeLists.txt

cd ..
pushd libs
if not exist build cmake -DCMAKE_GENERATOR="Visual Studio 16 2019" -DARCHITECTURE=Win32 -P buildall.cmake
popd

mkdir build_vs2019_win32
pushd build_vs2019_win32
cmake.exe .. -G "Visual Studio 16 2019" -A Win32
cmake.exe --build . --config release

cmake -P cmake_install.cmake
popd
pause
