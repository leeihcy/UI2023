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

set PATH=%DEPOT_TOOLS_DIR%;%GN_NINJA_DIR%;%VULKAN_SDK_BIN_DIR%;%PATH%
