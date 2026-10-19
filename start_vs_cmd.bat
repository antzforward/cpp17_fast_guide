@echo off
REM File encoding: GBK. Do not save as UTF-8.
REM call start_vs_cmd.bat
REM double-click start_vs_window.bat
REM Console stays 65001 so UTF-8 C++ output shows Chinese.

if "%VS_FASTGUIDE_READY%"=="1" (
    chcp 65001 >nul
    echo env already set, code page 65001
    goto :eof
)

set VCToolsVersion=
set VCToolsInstallDir=
set VSCMD_VER=
set VSINSTALLDIR=
set VCINSTALLDIR=

call "D:\Program Files\VisualStudioCommunity2026\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 (
    echo vcvarsall.bat failed.
    goto :eof
)

REM Do not append INCLUDE. Put extra -I on CL.
set CL=/std:c++23preview /utf-8 /IG:\cpp17_fast_guide\third_party\range-v3\include /IG:\cpp17_fast_guide\include
set VS_FASTGUIDE_READY=1

REM vcvars may change the console CP; restore UTF-8.
chcp 65001 >nul

echo.
chcp
where cl
echo CL=%CL%
echo.