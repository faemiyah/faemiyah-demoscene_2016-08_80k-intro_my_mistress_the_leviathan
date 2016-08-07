@echo off

set GLEW_INCLUDE_PATH=%~dp0..\env\glew-1.13.0\include
set BOOST_INCLUDE_PATH=%~dp0..\env\boost_1_60_0

set DNLOAD=%~dp0..\dnload\dnload.py
if not exist "%DNLOAD%" (
  echo Could not find dnload.py
  goto :error
)

if not exist "%~dp0src\dnload.h" type nul >%~dp0src/dnload.h

python "%DNLOAD%" -I"%GLEW_INCLUDE_PATH%" -I"%BOOST_INCLUDE_PATH%" "%~dp0src\intro.cpp" -v

if errorlevel 1 (
  echo Regenerating symbols failed
  goto :error
)

exit /b %errorlevel%

:error
pause
exit /b 1
