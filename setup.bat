@ECHO OFF

:: Store the original directory before requesting admin
SET "SCRIPT_DIR=%~dp0"

:: Request administrative privileges
NET SESSION >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    powershell -Command "Start-Process -Verb RunAs -FilePath '%0' -ArgumentList 'am_admin'"
    exit /b
)

:: Check if script is running with admin rights
IF "%1"=="am_admin" (
    :: Use the stored directory path
    cd /d "%SCRIPT_DIR%\vcpkg"
    call bootstrap-vcpkg.bat
    call vcpkg.exe install sdl2
    call vcpkg.exe integrate install
    cd /d "%SCRIPT_DIR%"
    timeout /T 3
    exit /b
)