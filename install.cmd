@echo off
pushd %~dp0
setlocal ENABLEDELAYEDEXPANSION

if not @%1@==@@ cd %1
if not exist build goto :end
cd build

set app=ttermpro.exe
set src=%ProgramFiles(x86)%\teraterm\
if not exist "!src!%app%" set src=%ProgramFiles%\teraterm\
if not exist "!src!%app%" (
  echo Teraterm ��������܂���ł����B
  exit /b
)

echo �t�@�C���R�s�[��...
if exist %cd%\Release\ (
set file=%cd%\Release\TTX*.dll
powershell -Command "Start-Process -FilePath xcopy.exe -ArgumentList '/Y %file% ""!src!""' -Verb RunAs"
)

echo �t�@�C���R�s�[��...
if exist %cd%\data\ (
set file=%cd%\data\*.*l
powershell -Command "Start-Process -FilePath xcopy.exe -ArgumentList '/Y %file% ""!src!""' -Verb RunAs"
)

:end
rem pause
endlocal
popd
