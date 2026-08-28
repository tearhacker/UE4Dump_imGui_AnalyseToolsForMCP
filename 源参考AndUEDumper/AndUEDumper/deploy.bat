@ECHO OFF
SETLOCAL ENABLEDELAYEDEXPANSION

:: ===============================
:: Config
:: ===============================

:: Default build directories
SET "CMAKE_BUILD_DIR=cmake_build"
SET "NDK_BUILD_DIR=ndk_build"

SET "PUSH_PATH=/data/local/tmp"

SET "EXE_NAME=UEDump3r"

:: Executable arguments
SET "EXE_ARGS=-o /sdcard -d"

:: ===============================
:: Menu: choose build system
:: ===============================
ECHO.
ECHO Select build system for deployment:
ECHO   [1] CMake
ECHO   [2] ndk-build
ECHO.

SET /P "CHOICE=Enter choice (1 or 2): "

IF "%CHOICE%"=="1" (
    SET "BUILD_SYSTEM=cmake"
    SET "BIN_PATH=%CMAKE_BUILD_DIR%"
) ELSE IF "%CHOICE%"=="2" (
    SET "BUILD_SYSTEM=ndk"
    SET "BIN_PATH=%NDK_BUILD_DIR%/libs"
) ELSE (
    ECHO Invalid choice
	PAUSE
    EXIT /B 1
)

IF NOT EXIST "%BIN_PATH%" (
    ECHO ERROR: Binaries folder not found: "%BIN_PATH%"
	PAUSE
    EXIT /B 1
)

:: ===============================
:: Menu: choose ABI
:: ===============================
ECHO.
ECHO Select ABI for deployment:
ECHO   [1] arm
ECHO   [2] arm64
ECHO   [3] x86
ECHO   [4] x86_64
ECHO.
SET /P "CHOICE=Enter choice (1, 2, 3, 4): "

IF "%CHOICE%"=="1" (
    SET "EXE_ABI=armeabi-v7a"
) ELSE IF "%CHOICE%"=="2" (
    SET "EXE_ABI=arm64-v8a"
) ELSE IF "%CHOICE%"=="3" (
    SET "EXE_ABI=x86"
) ELSE IF "%CHOICE%"=="4" (
    SET "EXE_ABI=x86_64"
) ELSE (
    ECHO Invalid choice
	PAUSE
    EXIT /B 1
)

SET "EXE_PATH=%BIN_PATH%/%EXE_ABI%/%EXE_NAME%"
SET "DEVICE_EXE_PATH=%PUSH_PATH%/%EXE_NAME%"

ECHO Deploying from %BUILD_SYSTEM% build...
ECHO Executable: "%EXE_PATH%"

IF NOT EXIST "%EXE_PATH%" (
    ECHO ERROR: Executable not found: "%EXE_PATH%"
	PAUSE
    EXIT /B 1
)

:: ===============================
:: Check ADB connection
:: ===============================
ECHO Checking connected devices...

for /f "delims=" %%i in ('adb install app.apk 2^>^&1') do (
    echo %%i | findstr /C:"more than one device/emulator" >nul && (
        ECHO "Detected more than one connected device/emulator."
        ECHO "Restaring adb..."
        adb kill-server
        timeout /t 1 >nul
        adb start-server
    )
)

SET "DEVICE_ID="
FOR /F "skip=1 tokens=1,2" %%a IN ('adb devices') DO (
    IF "%%b"=="device" SET "DEVICE_ID=%%a"
)

IF "%DEVICE_ID%"=="" (
    ECHO ERROR: No Android device detected.
	PAUSE
    EXIT /B 1
)

ECHO Device connected: "%DEVICE_ID%"

:: ===============================
:: Kill running executable
:: ===============================
ECHO Killing any running instances of "%EXE_NAME%"...
adb shell "su -c 'pkill -f %EXE_NAME% 2>/dev/null'" >NUL 2>&1
adb shell "su -c 'kill $(pidof %EXE_NAME%) 2>/dev/null'" >NUL 2>&1

:: ===============================
:: Push executable
:: ===============================
ECHO Pushing executable...
adb push "%EXE_PATH%" "%PUSH_PATH%/"

:: ===============================
:: Set executable permissions
:: ===============================
ECHO Setting executable permissions...
adb shell "su -c 'chmod 755 %DEVICE_EXE_PATH%'"

:: ===============================
:: Run executable
:: ===============================
ECHO Running "%EXE_NAME%" on device...
adb shell "su -c '%DEVICE_EXE_PATH% %EXE_ARGS%'"

PAUSE
