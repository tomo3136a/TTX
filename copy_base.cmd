@echo off
rem copy base

cd %~dp0
pushd ..

set DST=buildtools\download\perl
if not exist %DST% mkdir %DST%
copy ..\base\%DST%\* %DST%

set DST=libs\download\argon2
if not exist %DST% mkdir %DST%
copy ..\base\%DST%\* %DST%
set DST=libs\download\cJSON
if not exist %DST% mkdir %DST%
copy ..\base\%DST%\* %DST%
set DST=libs\download\libressl
if not exist %DST% mkdir %DST%
copy ..\base\%DST%\* %DST%
set DST=libs\download\oniguruma
if not exist %DST% mkdir %DST%
copy ..\base\%DST%\* %DST%
set DST=libs\download\putty
if not exist %DST% mkdir %DST%
copy ..\base\%DST%\* %DST%
set DST=libs\download\SFMT
if not exist %DST% mkdir %DST%
copy ..\base\%DST%\* %DST%
set DST=libs\download\zlib
if not exist %DST% mkdir %DST%
copy ..\base\%DST%\* %DST%

:end
popd
pause
