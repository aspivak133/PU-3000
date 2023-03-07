@echo off
setlocal enabledelayedexpansion

rem ===========================================================================
rem = Start the SIMphony environment
rem =
rem ===========================================================================
call %~dp0configure_SIMPhony.bat
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

set PATH=%PSA_PATH%

start "SIMphony" /D %~dp0..\integration\export\%PSA_HARDWARE_TYPE% "%IAP7K_SIMPHONY_HOME%\exe\Starter.exe"
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
