@echo off
rem make base

cd %~dp0
pushd ..
if not exist ..\base mkdir ..\base

set DST=buildtools\download\perl
if not exist ..\base\%DST% mkdir ..\base\%DST%
if exist %DST% copy %DST%\* ..\base\%DST%

set DST=libs\download\argon2
if not exist ..\base\%DST% mkdir ..\base\%DST%
if exist %DST% copy %DST%\* ..\base\%DST%
set DST=libs\download\cJSON
if not exist ..\base\%DST% mkdir ..\base\%DST%
if exist %DST% copy %DST%\* ..\base\%DST%
set DST=libs\download\libressl
if not exist ..\base\%DST% mkdir ..\base\%DST%
if exist %DST% copy %DST%\* ..\base\%DST%
set DST=libs\download\oniguruma
if not exist ..\base\%DST% mkdir ..\base\%DST%
if exist %DST% copy %DST%\* ..\base\%DST%
set DST=libs\download\putty
if not exist ..\base\%DST% mkdir ..\base\%DST%
if exist %DST% copy %DST%\* ..\base\%DST%
set DST=libs\download\SFMT
if not exist ..\base\%DST% mkdir ..\base\%DST%
if exist %DST% copy %DST%\* ..\base\%DST%
set DST=libs\download\zlib
if not exist ..\base\%DST% mkdir ..\base\%DST%
if exist %DST% copy %DST%\* ..\base\%DST%

:end
popd
pause
