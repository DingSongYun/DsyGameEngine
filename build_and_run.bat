@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: Set console colors
color 0A

:: Parse command line arguments
set "ACTION=%1"
set "CONFIG=%2"

:: Default values
if "%ACTION%"=="" set "ACTION=build"
if "%CONFIG%"=="" set "CONFIG=Debug"

echo ========================================
echo DsyGameEngine Build Script
echo ========================================
echo Action: %ACTION%
echo Config: %CONFIG%
echo ========================================

:: Change to Source directory
cd /d "%~dp0Source"
if errorlevel 1 (
    echo Error: Cannot change to Source directory
    pause
    exit /b 1
)

:: Execute based on action
if /i "%ACTION%"=="clean" goto :clean
if /i "%ACTION%"=="build" goto :build
if /i "%ACTION%"=="run" goto :run
if /i "%ACTION%"=="rebuild" goto :rebuild
if /i "%ACTION%"=="help" goto :help

echo Unknown action: %ACTION%
goto :help

:clean
echo.
echo [CLEAN] Removing build directory...
if exist "build" (
    rmdir /s /q "build"
    if errorlevel 1 (
        echo Error: Failed to delete build directory!
        pause
        exit /b 1
    )
    echo Build directory cleaned successfully!
) else (
    echo Build directory does not exist.
)
goto :end

:build
echo.
echo [1/2] Generating Visual Studio project files...
cmake -B build -S . -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo.
    echo Error: CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo [2/2] Building project (%CONFIG%)...
cmake --build build --config %CONFIG%
if errorlevel 1 (
    echo.
    echo Error: Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
goto :end

:run
:: First check if executable exists
set "EXE_PATH=build\Runtime\%CONFIG%\DsyGameEngine.exe"
if not exist "%EXE_PATH%" (
    echo Executable not found: %EXE_PATH%
    echo Building first...
    goto :build_and_run
)

echo.
echo [RUN] Starting DsyGameEngine (%CONFIG%)...
echo ========================================
echo.
"%EXE_PATH%"
echo.
echo ========================================
echo Program finished with exit code: %errorlevel%
goto :end

:build_and_run
call :build
if errorlevel 1 exit /b 1
call :run
goto :end

:rebuild
call :clean
call :build
goto :end

:help
echo.
echo Usage: build_and_run.bat [ACTION] [CONFIG]
echo.
echo Actions:
echo   build    - Build the project (default)
echo   run      - Run the executable (build if needed)
echo   clean    - Clean build directory
echo   rebuild  - Clean and build
echo   help     - Show this help
echo.
echo Configs:
echo   Debug    - Debug build (default)
echo   Release  - Release build
echo.
echo Examples:
echo   build_and_run.bat
echo   build_and_run.bat build Release
echo   build_and_run.bat run
echo   build_and_run.bat clean
echo   build_and_run.bat rebuild Release
echo.
goto :end

:end
if "%ACTION%"=="help" (
    pause
) else (
    echo.
    set /p choice="Press Enter to close, or type 'stay' to keep window open: "
    if /i "!choice!"=="stay" (
        echo Window will stay open...
        pause
    )
)