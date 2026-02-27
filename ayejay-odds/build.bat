@echo off
setlocal EnableExtensions

rem ============================================================
rem build.bat - Windows/MSVC configure/build/run (optional vcpkg toolchain)
rem - No line-continuation (^) pitfalls
rem ============================================================

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

set "BUILD_DIR=%ROOT%\build\msvc-x64"
set "CONFIG=%1"
if "%CONFIG%"=="" set "CONFIG=Debug"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%" >nul 2>&1

set "TOOLCHAIN_ARG="
if defined VCPKG_ROOT (
  set "TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
  if exist "%TOOLCHAIN%" (
    set "TOOLCHAIN_ARG=-DCMAKE_TOOLCHAIN_FILE:FILEPATH=%TOOLCHAIN%"
  ) else (
    echo WARNING: VCPKG_ROOT is set but toolchain not found: "%TOOLCHAIN%"
  )
)

echo Configuring...
cmake -S "%ROOT%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 %TOOLCHAIN_ARG%
if errorlevel 1 exit /b 1

echo Building (%CONFIG%)...
cmake --build "%BUILD_DIR%" --config "%CONFIG%"
if errorlevel 1 exit /b 1

set "EXE=%BUILD_DIR%\%CONFIG%\ayejay_odds_example.exe"
if not exist "%EXE%" (
  echo ERROR: exe not found: "%EXE%"
  exit /b 1
)

echo Running: "%EXE%"
"%EXE%"
exit /b 0
