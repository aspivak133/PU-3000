@echo off

rem ===========================================================================
rem = Configuration script
rem ===========================================================================

rem ===========================================================================
rem = User-defined configurations.
rem =     These variables can be customized according to the instructions
rem =     below.
rem ===========================================================================

rem  --------------------------------------------------------------------------
rem  - Platform release version.
rem  -     possible values: Only RELEASE1 is supported.
rem  --------------------------------------------------------------------------
set PSA_PLATFORM_RELEASE=RELEASE1

rem  --------------------------------------------------------------------------
rem  - Hardware type.
rem  -     possible values: MFD-3068, PU-3000, VPU-3000, SIMphony
rem  --------------------------------------------------------------------------
set PSA_HARDWARE_TYPE=PU-3000

rem  --------------------------------------------------------------------------
rem  - IP address and subnet mask of the maintenance Ethernet interface.
rem  -     Not applicable for SIMphony hardware type.
rem  --------------------------------------------------------------------------
set PSA_MAINTENANCE_IP_ADDR=192.168.3.100
set PSA_MAINTENANCE_IP_MASK=255.255.255.0

rem  --------------------------------------------------------------------------
rem  - Green Hills INTEGRITY installation directory.
rem  --------------------------------------------------------------------------
set PSA_GHS_ROOT_DIR=C:\GHS

rem  --------------------------------------------------------------------------
rem  - Green Hills INTEGRITY license server(s)
rem  -     possible values: list of comma-separated server names
rem  --------------------------------------------------------------------------
set PSA_GHS_LICENSE_SERVER=172.31.88.219

rem  --------------------------------------------------------------------------
rem  - SIMphony installation directory.
rem  --------------------------------------------------------------------------
rem set PSA_SIMPHONY_ROOT_DIR=C:\Program Files (x86)\SIMphony\SIMphony_VS10\5.1.4.22254
set PSA_SIMPHONY_ROOT_DIR=C:\Soft\SIMphony 5.1.5.22300

rem  --------------------------------------------------------------------------
rem  - Java installation directory.
rem  --------------------------------------------------------------------------
set PSA_JAVA_ROOT_DIR=C:\Program Files (x86)\Java\jre1.8.0_361\bin

rem  --------------------------------------------------------------------------
rem  - Microsoft Visual Studio Version
rem  -     possible values: 2010, 2017
rem  --------------------------------------------------------------------------
set PSA_VS_VERSION=2010

rem  --------------------------------------------------------------------------
rem  - Microsoft Visual Studio installation directory.
rem  - 		required for VS 2010 only
rem  --------------------------------------------------------------------------
set PSA_VS_ROOT_DIR=C:\Program Files (x86)\Microsoft Visual Studio 10.0

rem  --------------------------------------------------------------------------
rem  - Python executable directory.
rem  --------------------------------------------------------------------------
set PSA_PYTHON_EXE_DIR=C:\Python27

rem ===========================================================================
rem = Automatic configuration.
rem =     DO NOT MODIFY
rem ===========================================================================
set GHS_ROOT=%PSA_GHS_ROOT_DIR%
set GHS_BIN=%GHS_ROOT%\comp_201516
set GHS_HOME=%GHS_ROOT%\adamulti_616e
set GHS_RTOS=%GHS_ROOT%\rtos-cert-noncert-install
set GHS_RTOS_178B=%GHS_RTOS%
set GHS_LMHOST=@%PSA_GHS_LICENSE_SERVER%

set IAP7K_SIMPHONY_HOME=%PSA_SIMPHONY_ROOT_DIR%
set IAP7K_SIMPHONY_DLL_HOME=%PSA_SIMPHONY_ROOT_DIR%\Library	

set VS_2010_PATH=%PSA_VS_ROOT_DIR%

if %PSA_VS_VERSION% EQU 2010 (
	set IAP7K_SIMPHONY_GPM_DLL=%~dp0..\dependencies\185-617644-001wr\Platform\GPM\DEBUG\openGL\libs\WIN32_VS10
) else (
	if %PSA_VS_VERSION% EQU 2017 (
		set IAP7K_SIMPHONY_GPM_DLL=%~dp0..\dependencies\185-617644-001wr\Platform\GPM\DEBUG\openGL\libs\WIN32_VS17
	) else (
		echo Visual Studio version not supported.  Ensure PSA_VS_VERSION is set correctly to 2010 or 2017
		pause
		exit
	)
)
set IAP7K_SIMPHONY_WORKING_DIRECTORY=%~dp0..\integration\export\%PSA_HARDWARE_TYPE%
set IAP7K_INTEGRATION_HOME=%~dp0..\integration\export\%PSA_HARDWARE_TYPE%

set JAVA_HOME=%PSA_JAVA_ROOT_DIR%

if %PSA_HARDWARE_TYPE% EQU SIMphony (
	if %PSA_VS_VERSION% EQU 2010 (
		set PSA_BUILD_TARGET=WIN32_VS10
	) else (
		set PSA_BUILD_TARGET=WIN32_VS17
	)
    set PSA_BUILD_CONFIG=DEBUG
    set PSA_CONSOLE_TX_IP_ADDR=127.0.0.1
    set PSA_CONSOLE_RX_IP_ADDR=127.0.0.1
) else (
    set PSA_BUILD_TARGET=P3041_GHS
    set PSA_BUILD_CONFIG=RELEASE_PERF
    set PSA_CONSOLE_TX_IP_ADDR=%PSA_MAINTENANCE_IP_ADDR%
    set PSA_CONSOLE_RX_IP_ADDR=0.0.0.0
)

set PSA_PLATFORM_ROOT_DIR=%~dp0..\dependencies\185-617644-001wr\Platform
set PSA_CPM_DIR=%PSA_PLATFORM_ROOT_DIR%\CPM_noncert\%PSA_BUILD_TARGET%\%PSA_BUILD_CONFIG%
set PSA_RD_DIR=%PSA_PLATFORM_ROOT_DIR%\ReferenceDesign_noncert\%PSA_BUILD_TARGET%\%PSA_BUILD_CONFIG%
set PSA_GPM_DIR=%PSA_PLATFORM_ROOT_DIR%\GPM\%PSA_BUILD_CONFIG%
set PSA_PLATFORM_DIR=%PSA_PLATFORM_ROOT_DIR%\CMA-70XX_noncert\%PSA_BUILD_TARGET%\%PSA_BUILD_CONFIG%

set PSA_TOOL_INTEGRATOR=%~dp0..\dependencies\185-617443-001wr\integrator.jar
set PSA_TOOL_SCHEDULE_GENERATOR=%~dp0..\dependencies\174-617441-001wr\ScheduleConverter.py
set PSA_TOOL_CONFIG_GENERATOR=%~dp0..\dependencies\185-617444-001wr\configTool.jar
set PSA_TOOL_GPM_CONFIG_GENERATOR=%~dp0..\dependencies\185-617445-001wr\configTool.jar
set PSA_TOOL_HOST_CONFIG_GENERATOR=%~dp0..\dependencies\174-617483-001wr\HostConfigurationPDIGeneratorTool.py
set PSA_TOOL_A615_DATALOADER=%~dp0..\dependencies\185-616679-001wr\Arinc615_DataLoader.exe
set PSA_TOOL_APPEND_CRC=%~dp0..\dependencies\185-617442-001wr\AppendCrc32.exe
set PSA_TOOL_CONFIG_TABLE_BUILDER=%~dp0..\dependencies\174-617446-001wr\ConfigTableBuilder.py
set PSA_TOOL_NVMAT_BUILDER=%~dp0\..\dependencies\174-617447-001wr\FATBuilder.py
if %PSA_VS_VERSION% EQU 2010 (
	set PSA_TOOL_BUILD_WIN32=%PSA_PLATFORM_ROOT_DIR%\CPM_noncert\WIN32\Tools\msbuild_vs10.bat
) else (
	set PSA_TOOL_BUILD_WIN32=%PSA_PLATFORM_ROOT_DIR%\CPM_noncert\WIN32\Tools\msbuild_vs17.bat	
)	

set ARINC_615_DL_TOOL=%PSA_TOOL_A615_DATALOADER%

set PSA_PATH_ORIG=%PATH%
set PSA_PATH=%GHS_BIN%;%GHS_HOME%;%JAVA_HOME%\bin;%PSA_PYTHON_EXE_DIR%;%IAP7K_SIMPHONY_DLL_HOME%;%IAP7K_SIMPHONY_GPM_DLL%;%PATH%

exit /B 0
