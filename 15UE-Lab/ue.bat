@echo off
setlocal
where node >nul 2>&1
if errorlevel 1 (
    echo Node.js was not found. Install Node.js or add the existing runtime to PATH.
    exit /b 1
)
node "%~dp0scripts\ue-lab.cjs" %*
exit /b %ERRORLEVEL%
