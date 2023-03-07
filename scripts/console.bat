@echo off
setlocal enabledelayedexpansion

rem ===========================================================================
rem = Start the console, used to interact with the hardware.
rem =
rem ===========================================================================
set /p ask="Select: 1. PU3000 or 2. SIMPhony: "
echo. 
if %ask%==1 (call %~dp0configure_PU3000.bat) else (call %~dp0configure_SIMPhony.bat)
echo.
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

set PATH=%PSA_PATH%

python %~dp0\Console.py
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

goto EXIT_SUCCESS

:EXIT_ERROR
echo OPERATION FAILED error=!ERRORLEVEL! >&2
set PATH=%PSA_PATH_ORIG%
exit /B 1

:EXIT_SUCCESS
set PATH=%PSA_PATH_ORIG%
exit /B 0
