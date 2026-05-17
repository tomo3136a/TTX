@echo off
cd %~dp0
if not @%1@==@@ cd %1
mkdir build_x64
pushd build_x64
del /q /s *
cmake -A x64 ..
cmake --build . --config Release --target package
popd
pause
