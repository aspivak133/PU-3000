@echo off
setlocal enabledelayedexpansion

rem ===========================================================================
rem = ARINC-615A media load script.
rem =
rem = This script performs the following:
rem =    1) Configure the environment.
rem =    2) Discover the target hardware
rem =    3) Upload the ARINC-615A media onto the target hardware
rem =    4) Cleanup the environment.
rem ===========================================================================

rem ===========================================================================
rem =    1) Configure the environment.
rem ===========================================================================
call %~dp0configure_PU3000.bat
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

set PATH=%PSA_PATH%

set PSA_EXPORT_DIR=%~dp0..\integration\export\%PSA_HARDWARE_TYPE%
set PSA_MAINTENANCE_IP_ADDR=192.168.3.100
set PSA_MAINTENANCE_IP_MASK=255.255.255.0

rem ===========================================================================
rem =    2) Discover the target hardware
rem ===========================================================================
%PSA_TOOL_A615_DATALOADER% find %PSA_MAINTENANCE_IP_ADDR% %PSA_MAINTENANCE_IP_MASK%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rem ===========================================================================
rem =    3) Upload the ARINC-615A media onto the target hardware
rem ===========================================================================
%PSA_TOOL_A615_DATALOADER% upload %PSA_MAINTENANCE_IP_ADDR% %PSA_HARDWARE_TYPE%_0000 %PSA_EXPORT_DIR%\CMC-DEMN-0001\CMC4E-DEMN-0001.LUH
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

%PSA_TOOL_A615_DATALOADER% upload %PSA_MAINTENANCE_IP_ADDR% %PSA_HARDWARE_TYPE%_0000 %PSA_EXPORT_DIR%\CMC-DEMA-0001\CMC41-DEMA-0001.LUH
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rem ===========================================================================
rem =    4) Cleanup the environment.
rem ===========================================================================

goto EXIT_SUCCESS

:EXIT_ERROR
echo LOAD FAILED error=!ERRORLEVEL! >&2
set PATH=%PSA_PATH_ORIG%
exit /B 1

:EXIT_SUCCESS
echo LOAD SUCCESSFUL >&2
set PATH=%PSA_PATH_ORIG%
exit /B 0
