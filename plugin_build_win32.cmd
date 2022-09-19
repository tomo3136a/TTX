@echo off
cd %~dp0
if not @%1@==@@ cd %1
mkdir build_win32
pushd build_win32
del /q /s *
cmake -A Win32 ..
cmake --build . --config Release --target package
popd
pause
