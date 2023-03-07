@echo off
setlocal enabledelayedexpansion

rem ===========================================================================
rem = Application build script.
rem =
rem = This script performs the following:
rem =    1) Configure the environment, and delete generated/compilation artefacts 
rem =       from previous builds.
rem =    2) Generate the application's integration artefacts from its requirements
rem =       and ARINC-653 blueprint.
rem =    3) Compile the application partitions.
rem =    4) Package the application partition as required for the integration phase.
rem =    5) Cleanup the environment.
rem ===========================================================================

rem ===========================================================================
rem =    1) Configure the environment, and delete generated/compilation artefacts 
rem =       from previous builds.
rem ===========================================================================
set /p ask="Select: 1. PU3000 or 2. SIMPhony: "
echo. 
if %ask%==1 (call %~dp0configure_PU3000.bat) else (call %~dp0configure_SIMPhony.bat)
echo.

if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

set PATH=%PSA_PATH%

set PSA_SRC_DIR=%~dp0..\application\src
set PSA_GEN_SRC_DIR=%~dp0..\application\gen_src\%PSA_HARDWARE_TYPE%
set PSA_BUILD_DIR=%~dp0..\application\build\%PSA_HARDWARE_TYPE%\
set PSA_EXPORT_DIR=%~dp0..\application\export\%PSA_HARDWARE_TYPE%\

rmdir /S /Q %PSA_GEN_SRC_DIR%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rmdir /S /Q %PSA_BUILD_DIR%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rmdir /S /Q %PSA_EXPORT_DIR%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

mkdir %PSA_GEN_SRC_DIR%\PART_CMD
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

mkdir %PSA_BUILD_DIR%\PART_CMD
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

mkdir %PSA_EXPORT_DIR%\PART_CMD
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR


rem ===========================================================================
rem =    2) Generate the application's integration artefacts from its requirements
rem =       and ARINC-653 blueprint.
rem ===========================================================================
java -jar %PSA_TOOL_INTEGRATOR% -app_partition_files %PSA_SRC_DIR%\PART_CMD\partition_%PSA_HARDWARE_TYPE%.reqs %PSA_PLATFORM_DIR% %PSA_GEN_SRC_DIR%\PART_CMD
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rem ===========================================================================
rem =    3) Compile the application partitions.
rem ===========================================================================
if %PSA_HARDWARE_TYPE% EQU SIMphony (
	if %PSA_VS_VERSION% EQU 2010 (
		call %PSA_TOOL_BUILD_WIN32% /t:Rebuild /p:configuration=Debug_VS10 /p:Platform=Win32 %PSA_SRC_DIR%\PART_CMD\PART_CMD.sln
	) else (
		call %PSA_TOOL_BUILD_WIN32% /t:Rebuild /p:configuration=Debug_VS17 /p:Platform=Win32 %PSA_SRC_DIR%\PART_CMD\PART_CMD_VS_2017.sln
		call %PSA_TOOL_BUILD_WIN32% /t:Rebuild /p:configuration=Debug_VS17 /p:Platform=Win32 %PSA_SRC_DIR%\PART_LIGHT\PART_LIGHT_VS_2017.sln	
	)
    if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
) else (
    gbuild -all -top %PSA_SRC_DIR%\PART_CMD\PART_CMD.gpj 
    if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem ===========================================================================
rem =    4) Package the application partition as required for the integration phase.
rem ===========================================================================
copy /Y %PSA_SRC_DIR%\PART_CMD\PART_CMD_A653Config_Health.xml /B %PSA_EXPORT_DIR%\PART_CMD\
copy /Y %PSA_SRC_DIR%\PART_CMD\PART_CMD_A653Config_Memory.xml /B %PSA_EXPORT_DIR%\PART_CMD\
copy /Y %PSA_SRC_DIR%\PART_CMD\PART_CMD_A653Config_Partition.xml /B %PSA_EXPORT_DIR%\PART_CMD\
copy /Y %PSA_SRC_DIR%\PART_CMD\partition_%PSA_HARDWARE_TYPE%.reqs /B %PSA_EXPORT_DIR%\PART_CMD\

rem ===========================================================================
rem =    5) Cleanup the environment.
rem ===========================================================================

goto EXIT_SUCCESS

:EXIT_ERROR
echo BUILD FAILED error=!ERRORLEVEL! >&2
set PATH=%PSA_PATH_ORIG%
pause
exit /B 1

:EXIT_SUCCESS
echo BUILD SUCCESSFUL >&2
set PATH=%PSA_PATH_ORIG%
pause
exit /B 0
