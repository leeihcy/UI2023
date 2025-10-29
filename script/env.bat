@echo off
REM update cmd environment variables for build
REM call script/env.bat
REM 

REM get script directory
set SCRIPT_DIR=%~dp0
set ROOT_DIR=%SCRIPT_DIR:~0,-7%

call "%ROOT_DIR%script/local/env.win.bat"

set DEPOT_TOOLS_DIR=%ROOT_DIR%third_party\depot_tools
set GN_NINJA_DIR=%ROOT_DIR%build_tools\windows
set VULKAN_SDK_BIN_DIR=%VULKAN_SDK_DIR%/bin

echo PYTHON_INC_DIR = %PYTHON_INC_DIR%
echo PYTHON_LIB_DIR = %PYTHON_LIB_DIR%
echo VULKAN_SDK_DIR = %VULKAN_SDK_DIR%

set PATH=%DEPOT_TOOLS_DIR%;%GN_NINJA_DIR%;%VULKAN_SDK_BIN_DIR%;%PATH%

echo PATH updated.
echo Environment initialized for Windows