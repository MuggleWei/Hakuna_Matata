@echo off

::setlocal ENABLEDELAYEDEXPANSION

:: handle argv
set count=0
for %%x in (%*) do (
    set /a count+=1
)

if %count% LSS 2 (
    echo "[ERROR] build.sh without build type"
        echo "[ERROR] Usage: build.sh <Debug|Release> <shared|static>"
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

set origin_dir=%~dp0
set build_dir=%origin_dir%build
set dist_dir=%origin_dir%dist

if exist %build_dir% (
        rmdir /s /q %build_dir%
)
if exist %dist_dir% (
        rmdir /s /q %dist_dir%
)
md %build_dir%

cmake ^
        -S %origin_dir% -B %build_dir% ^
        -G "Visual Studio 16 2019" -A x64 ^
        -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
        -DBUILD_SHARED_LIBS=%CMAKE_SHARED_LIBS% ^
        -DCMAKE_INSTALL_PREFIX=%dist_dir%
cmake --build %build_dir% --config %BUILD_TYPE%
cmake --build %build_dir% --config %BUILD_TYPE% --target install
