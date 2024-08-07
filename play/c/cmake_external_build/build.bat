@echo off

setlocal ENABLEDELAYEDEXPANSION

:: handle argv
set count=0
for %%x in (%*) do (
    set /a count+=1
)

if %count% LSS 2 (
    echo "[ERROR] build.bat without build type"
	echo "[ERROR] Usage: build.bat <Debug|Release> <shared|static>"
    exit /b
)

set BUILD_TYPE=%~1
set LIB_TYPE=%~2

set "CMAKE_SHARED_LIBS=ON"
if "%LIB_TYPE%"=="shared" (
	set "CMAKE_SHARED_LIBS=ON"
) else (
	set "CMAKE_SHARED_LIBS=OFF"
)

echo build type: %BUILD_TYPE%
echo lib type: %LIB_TYPE%
echo CMAKE_SHARED_LIBS: %CMAKE_SHARED_LIBS%

set origin_dir=%~dp0
set build_dir=%origin_dir%build
set deps_dir=%origin_dir%dist

if exist %build_dir% (
	echo remove build dir
	rd /s /q %build_dir%
)
echo make build dir
md build

cmake ^
	-S %origin_dir% -B %build_dir% ^
	-G "Visual Studio 16 2019" -A x64 ^
	-DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
	-DBUILD_SHARED_LIBS=%CMAKE_SHARED_LIBS% ^
	-DCMAKE_PREFIX_PATH=%deps_dir%
