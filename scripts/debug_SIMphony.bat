@echo off
setlocal enabledelayedexpansion

rem ===========================================================================
rem = Debug the SIMphony environment
rem =
rem ===========================================================================
call %~dp0configure.bat
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

set PATH=%PSA_PATH%

start /D %~dp0..\integration\export %~dp0..\integration\gen_src\%PSA_HARDWARE_TYPE%\Target\Integration_WIN32.sln
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

goto EXIT_SUCCESS

:EXIT_ERROR
echo EXECUTION FAILED error=!ERRORLEVEL! >&2
set PATH=%PSA_PATH_ORIG%
exit /B 1

:EXIT_SUCCESS
echo EXECUTION SUCCESSFUL
set PATH=%PSA_PATH_ORIG%
exit /B 0
