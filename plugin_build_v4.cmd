@echo off
cd %~dp0
if not @%1@==@@ cd %1
mkdir build_win32_v4
pushd build_win32_v4
del /q /s *
cmake -A Win32 -DTT4=1 ..
cmake --build . --config Release --target package
popd
pause
