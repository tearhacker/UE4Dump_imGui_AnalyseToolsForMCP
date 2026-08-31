@echo off
setlocal enabledelayedexpansion
title UMT Android Build (arm64-v8a)
cd /d "%~dp0"

REM ============================================================
REM  UMT device-side build script (Android arm64-v8a)
REM  Usage:
REM    build_android.bat          incremental build (default, fast)
REM    build_android.bat clean    full rebuild from scratch
REM  Deps: NDK 27 + VS2026 bundled CMake/Ninja (no manual install)
REM ============================================================

REM ---------- tool paths (edit if moved) ----------
set "CMAKE=D:/ProgramerDevelop/VS2026/SDK/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
set "NINJA=D:/ProgramerDevelop/VS2026/SDK/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe"
set "NDK_PATH=D:/ProgramerDevelop/windowsNDK27"

REM ---------- dirs (strip trailing backslash to avoid "\" escaping quotes) ----------
set "SRC_DIR=%~dp0"
if "%SRC_DIR:~-1%"=="\" set "SRC_DIR=%SRC_DIR:~0,-1%"
set "BUILD_DIR=%SRC_DIR%\build"
set "OUT_DIR=%SRC_DIR%\outputs\arm64-v8a"

REM ---------- args ----------
set "CLEAN="
if /i "%~1"=="clean"   set "CLEAN=--clean-first"
if /i "%~1"=="-c"      set "CLEAN=--clean-first"
if /i "%~1"=="rebuild" set "CLEAN=--clean-first"

REM ---------- tool existence check ----------
if not exist "%CMAKE%" (
    echo [ERROR] cmake not found: %CMAKE%
    echo         please fix CMAKE var at top of this script
    goto :fail
)
if not exist "%NINJA%" (
    echo [ERROR] ninja not found: %NINJA%
    goto :fail
)
if not exist "%NDK_PATH%\build\cmake\android.toolchain.cmake" (
    echo [ERROR] NDK not found: %NDK_PATH%
    goto :fail
)

echo ============================================================
echo  UMT Android Build (arm64-v8a)
echo  Source : %SRC_DIR%
if defined CLEAN (
    echo  Mode   : FULL rebuild
) else (
    echo  Mode   : incremental
)
echo ============================================================
echo.

REM ---------- 1. CMake configure ----------
echo [1/2] CMake configure ...
"%CMAKE%" -S "%SRC_DIR%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_MAKE_PROGRAM="%NINJA%" -DNDK_PATH=%NDK_PATH%
if errorlevel 1 (
    echo [ERROR] CMake configure failed
    goto :fail
)

REM ---------- 2. build ----------
echo [2/2] Building ...
"%CMAKE%" --build "%BUILD_DIR%" %CLEAN%
if errorlevel 1 (
    echo [ERROR] build failed, check errors above
    goto :fail
)

echo.
echo ============================================================
echo  BUILD SUCCESS!
echo  Output: %OUT_DIR%\UnrealMemoryTools
echo  Before deploy: adb forward tcp:35515 tcp:35515
echo ============================================================
goto :end

:fail
echo.
echo [BUILD FAILED] see errors above.
exit /b 1

:end
echo.
pause
