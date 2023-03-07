@echo off
setlocal enabledelayedexpansion

rem ===========================================================================
rem = Integration build script.
rem =
rem = This script performs the following:
rem =    1) Configure the environment, and delete generated/compilation artefacts 
rem =       from previous builds.
rem =    2) Generate the integration artefacts from the requirements.
rem =    3) Compile the Operational Flight Program (OFP) executable.
rem =    4) Generate the Parameter Data Item (PDI) binary files.
rem =    5) Package the OFP and PDIs into ARINC-615A medias.
rem =    6) Cleanup the environment.
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

set PSA_SRC_DIR=%~dp0..\integration\src
set PSA_GEN_SRC_DIR=%~dp0..\integration\gen_src\%PSA_HARDWARE_TYPE%\
set PSA_BUILD_DIR=%~dp0..\integration\build\%PSA_HARDWARE_TYPE%\
set PSA_EXPORT_DIR=%~dp0..\integration\export\%PSA_HARDWARE_TYPE%\

rem The integration-level NVMAT file contains the definitions for the dataloader 
rem application associated to the type of hardware:
rem
rem |---------------------------------------------------------------------------------| 
rem | Hardware  |       Dataloader application     |       Dataloader NVMAT           |
rem |           | CMC P/N        | ARINC-615A P/N  | CMC P/N        | ARINC-615A P/N  |
rem |-----------|----------------|-----------------|----------------|-----------------|
rem | MFD-3068  | 185-664707-001 | CMP47-PAAH-0001 | 185-664744-001 | CMP5C-PABP-0001 |
rem |---------------------------------------------------------------------------------| 
rem | VPU-3000  | 185-617059-001 | CMP41-PABM-0001 | 185-617134-001 | CMP42-PABN-0001 |
rem |  PU-3000  |                |                 |                |                 |
rem |---------------------------------------------------------------------------------| 
set PSA_NVMAT_FILE=%PSA_SRC_DIR%\NVMAT\NVMAT_%PSA_HARDWARE_TYPE%.xlsm

if %PSA_HARDWARE_TYPE% EQU SIMphony (
set PSA_CONFIG_TOOL_OPTIONS=-p --le
) else (
set PSA_CONFIG_TOOL_OPTIONS=-p
)

rmdir /S /Q %PSA_GEN_SRC_DIR%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rmdir /S /Q %PSA_BUILD_DIR%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rmdir /S /Q %PSA_EXPORT_DIR%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

mkdir %PSA_GEN_SRC_DIR%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

mkdir %PSA_BUILD_DIR%\OFP_temp
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

mkdir %PSA_BUILD_DIR%\OFP
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

mkdir %PSA_BUILD_DIR%\PDI
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

mkdir %PSA_BUILD_DIR%\media
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

mkdir %PSA_EXPORT_DIR%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

if %PSA_HARDWARE_TYPE% EQU SIMphony (
   mkdir %PSA_EXPORT_DIR%\filesystem\Platform
   if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem ===========================================================================
rem =    2) Generate the integration artefacts from the requirements.
rem ===========================================================================

rem Generate the schedule requirement file from the MS-Excel workbook
if %PSA_HARDWARE_TYPE% EQU SIMphony (
	python %PSA_TOOL_SCHEDULE_GENERATOR% -c %PSA_SRC_DIR%\schedule\schedule_SIMphony.xlsx -o %PSA_GEN_SRC_DIR%\schedule.reqs
) else (
	python %PSA_TOOL_SCHEDULE_GENERATOR% -c %PSA_SRC_DIR%\schedule\schedule.xlsx -o %PSA_GEN_SRC_DIR%\schedule.reqs
)

if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rem Generate the integration artefacts
if %PSA_VS_VERSION% EQU 2010 (
	java -jar %PSA_TOOL_INTEGRATOR% -integration %PSA_SRC_DIR%\integration_%PSA_HARDWARE_TYPE%.reqs
	if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
) else (
    if %PSA_HARDWARE_TYPE% EQU SIMphony (
	    java -jar %PSA_TOOL_INTEGRATOR% -integration %PSA_SRC_DIR%\integration_%PSA_HARDWARE_TYPE%_VS_2017.reqs
	    if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
	) else (
	    java -jar %PSA_TOOL_INTEGRATOR% -integration %PSA_SRC_DIR%\integration_%PSA_HARDWARE_TYPE%.reqs
	    if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
    ) 	
)


rem ===========================================================================
rem =    3) Compile the Operational Flight Program (OFP) executable.
rem ===========================================================================

if %PSA_HARDWARE_TYPE% EQU SIMphony (
  rem Compile and link the partitions
  if %PSA_VS_VERSION% EQU 2010 (
	call %PSA_TOOL_BUILD_WIN32% /t:Rebuild /p:configuration=Debug_VS10 /p:Platform=Win32 %PSA_GEN_SRC_DIR%\Target\Integration_WIN32.sln
	if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
  ) else (
	call %PSA_TOOL_BUILD_WIN32% /t:Rebuild /p:configuration=Debug_VS17 /p:Platform=Win32 %PSA_GEN_SRC_DIR%\Target\Integration_WIN32.sln
	if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR	
  )
) else (
  rem Compile the object tables and link them into the kernel partition
  gbuild -all -top %PSA_GEN_SRC_DIR%\Partitions\PART_KERNEL\PART_KERNEL_Part_GHS.gpj
  if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

  rem Integrate the partitions into the OFP
  gbuild -all -top %PSA_GEN_SRC_DIR%\Target\Integration_Link_GHS.gpj
  if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

  rem Validate the OFP
  java -jar %PSA_TOOL_INTEGRATOR% -validate_ofp %PSA_SRC_DIR%\integration_%PSA_HARDWARE_TYPE%.reqs %PSA_BUILD_DIR%\OFP\CMD_APP.ofp
  if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem ===========================================================================
rem =    4) Generate the Parameter Data Item (PDI) binary files.
rem ===========================================================================

rem      4.1) Generate the PDIs that are referenced from the Configurtion
rem           Table PDI
rem ---------------------------------------------------------------------------

rem Cmd application PDI (included as-is)
copy /Y %PSA_SRC_DIR%\PDI\Cmd_application_PDI.txt %PSA_BUILD_DIR%\PDI
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

if %PSA_HARDWARE_TYPE% NEQ SIMphony (
rem Platform's Event Log PDI
java -jar %PSA_TOOL_CONFIG_GENERATOR% %PSA_CONFIG_TOOL_OPTIONS% -c %PSA_GEN_SRC_DIR%\CXML\Platform_EVLOG_Config.cxml -o %PSA_BUILD_DIR%\PDI
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem Platform's Graphic PDI
if %PSA_HARDWARE_TYPE% NEQ PU-3000 (
java -jar %PSA_TOOL_GPM_CONFIG_GENERATOR% %PSA_CONFIG_TOOL_OPTIONS% -c %PSA_GEN_SRC_DIR%\CXML\Platform_Graphic_Config.cxml -o %PSA_BUILD_DIR%\PDI
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem Platform's Windowing PDI
if %PSA_HARDWARE_TYPE% NEQ PU-3000 (
java -jar %PSA_TOOL_GPM_CONFIG_GENERATOR% %PSA_CONFIG_TOOL_OPTIONS% -c %PSA_GEN_SRC_DIR%\CXML\Platform_Windowing_Config.cxml -o %PSA_BUILD_DIR%\PDI
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem Platform's I/O PDI
java -jar %PSA_TOOL_CONFIG_GENERATOR% %PSA_CONFIG_TOOL_OPTIONS% -c %PSA_GEN_SRC_DIR%\CXML\Platform_IO_Config.cxml -o %PSA_BUILD_DIR%\PDI
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rem Platform's IP PDI
java -jar %PSA_TOOL_CONFIG_GENERATOR% %PSA_CONFIG_TOOL_OPTIONS% -c %PSA_GEN_SRC_DIR%\CXML\Platform_IP_Config.cxml -o %PSA_BUILD_DIR%\PDI
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rem Platform's Logbook PDI
java -jar %PSA_TOOL_CONFIG_GENERATOR% %PSA_CONFIG_TOOL_OPTIONS% -c %PSA_GEN_SRC_DIR%\CXML\Platform_Logbook.cxml -o %PSA_BUILD_DIR%\PDI
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

rem      4.2) Generate the Configuration Table PDI. 
rem           (not required on SIMphony)
rem           All files in directory %PSA_BUILD_DIR%\PDI will be
rem           referenced from that PDI, hence the importance that no file
rem           other than the PDIs generated at step 4.1 are in this directory.
rem ---------------------------------------------------------------------------
if %PSA_HARDWARE_TYPE% NEQ SIMphony (
python %PSA_TOOL_CONFIG_TABLE_BUILDER% -i %PSA_BUILD_DIR%\PDI -f %PSA_NVMAT_FILE% -o %PSA_BUILD_DIR%\PDI\CONFIGTABLE.cfg -d
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)
    
rem      4.3) Generate the Host Configuration PDI
rem ---------------------------------------------------------------------------
echo %PSA_HARDWARE_TYPE%
if %PSA_HARDWARE_TYPE% EQU MFD-3068 (
python %PSA_TOOL_HOST_CONFIG_GENERATOR% -o %PSA_BUILD_DIR%\PDI\HOST_CONFIG_SLOT_01.bin -g -d
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)
if %PSA_HARDWARE_TYPE% EQU PU-3000 (
python %PSA_TOOL_HOST_CONFIG_GENERATOR% -o %PSA_BUILD_DIR%\PDI\HOST_CONFIG_SLOT_01.bin
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)
if %PSA_HARDWARE_TYPE% EQU VPU-3000 (
python %PSA_TOOL_HOST_CONFIG_GENERATOR% -o %PSA_BUILD_DIR%\PDI\HOST_CONFIG_SLOT_01.bin -g
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)
if %PSA_HARDWARE_TYPE% EQU SIMphony (
python %PSA_TOOL_HOST_CONFIG_GENERATOR% -o %PSA_BUILD_DIR%\PDI\HOST_CONFIG.bin -g -d
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)


rem      4.5) Generate the NVMAT PDI
rem           (not required on SIMphony)
rem ---------------------------------------------------------------------------
if %PSA_HARDWARE_TYPE% NEQ SIMphony (
python %PSA_TOOL_NVMAT_BUILDER% -i %PSA_NVMAT_FILE% -o %PSA_BUILD_DIR%\PDI\FLASH_ALLOC.FAT
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem ===========================================================================
rem =    5) Package the OFP and PDIs into ARINC-615A medias.
rem         On SIMphony, ARINC-615A medias are not used. A directory structure
rem         containing the PDIs and executable is used instead.
rem ===========================================================================

rem      5.1) Append CRCs to all files that are to be packaged in ARINC-615A 
rem           medias
rem           (not required on SIMphony)
rem ---------------------------------------------------------------------------
if %PSA_HARDWARE_TYPE% NEQ SIMphony (
copy %PSA_BUILD_DIR%\PDI\* %PSA_BUILD_DIR%\media
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

copy %PSA_BUILD_DIR%\OFP\CMD_APP.ofp %PSA_BUILD_DIR%\media
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR

for /F %%F in ('dir /B %PSA_BUILD_DIR%\media\*') do (
%PSA_TOOL_APPEND_CRC% %PSA_BUILD_DIR%\media\%%F
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)
)

rem      5.2) Create the NVMAT media.
rem           (not required on SIMphony)
rem ---------------------------------------------------------------------------
if %PSA_HARDWARE_TYPE% NEQ SIMphony (
%PSA_TOOL_A615_DATALOADER% partmaker %PSA_EXPORT_DIR% CMC DEMN-0001 --thwid %PSA_HARDWARE_TYPE% --datafile %PSA_BUILD_DIR%\media\FLASH_ALLOC.FAT --userdata %PSA_SRC_DIR%\media\NVMAT_packageName.txt
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem      5.3) Create the OFP media.
rem           (not required on SIMphony)
rem ---------------------------------------------------------------------------
if %PSA_HARDWARE_TYPE% EQU PU-3000 (
%PSA_TOOL_A615_DATALOADER% partmaker %PSA_EXPORT_DIR% CMC DEMA-0001 --thwid %PSA_HARDWARE_TYPE% --datafile %PSA_BUILD_DIR%\media\HOST_CONFIG_SLOT_01.bin --datafile %PSA_BUILD_DIR%\media\CONFIGTABLE.cfg --datafile %PSA_BUILD_DIR%\media\Cmd_application_PDI.txt --datafile %PSA_BUILD_DIR%\media\EVLOG_CFG.dat --datafile %PSA_BUILD_DIR%\media\LOGBOOKCFG.dat --datafile %PSA_BUILD_DIR%\media\PLATIOCFG.dat --datafile %PSA_BUILD_DIR%\media\PLAT_IP_CFG.dat --datafile %PSA_BUILD_DIR%\media\CMD_APP.ofp  --userdata %PSA_SRC_DIR%\media\OFP_packageName.txt
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem      5.4) Create the directory structure simulating ARINC-615A medias
rem           (only required on SIMphony)
rem ---------------------------------------------------------------------------
if %PSA_HARDWARE_TYPE% EQU SIMphony (
rem Copy the PDIs and configuration files into the SIMphony runtime directories
copy /Y %PSA_SRC_DIR%\PDI\PlatformParameterEmulator.txt %PSA_EXPORT_DIR%\filesystem
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
copy /Y %PSA_SRC_DIR%\PDI\ConfigurationFolderUnits.txt %PSA_EXPORT_DIR%
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
copy /Y %PSA_SRC_DIR%\PDI\win7kDisplayConfig.txt %PSA_EXPORT_DIR%\Configuration
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
copy %PSA_BUILD_DIR%\PDI\* %PSA_EXPORT_DIR%\filesystem\Platform
if !ERRORLEVEL! NEQ 0 goto EXIT_ERROR
)

rem ===========================================================================
rem =    6) Cleanup the environment.
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
