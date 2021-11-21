@echo off
cd %~dp0
if not @%1@==@@ cd %1
mkdir build
cd build
del /q /s *
cmake -A Win32  ..
cmake --build . --config Release
pause
