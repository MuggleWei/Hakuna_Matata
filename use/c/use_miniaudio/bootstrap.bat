@echo off

setlocal ENABLEDELAYEDEXPANSION

set BUILD_TYPE=debug

set origin_dir=%~dp0

set dep_src_dir=%origin_dir%deps
set dep_build_dir=%origin_dir%_deps\build
set build_dir=%origin_dir%build
set install_dir=%origin_dir%dist

md %dep_build_dir%
md %build_dir%
md %install_dir%

:: build deps
cmake ^
	-S %dep_src_dir% -B %dep_build_dir% ^
	-DCMAKE_INSTALL_PREFIX=%install_dir%
cmake --build %dep_build_dir% --config %BUILD_TYPE% --target INSTALL

:: build
cmake ^
	-S %origin_dir% -B %build_dir% ^
	-DCMAKE_PREFIX_PATH=%install_dir% ^
	-DCMAKE_INSTALL_PREFIX=%install_dir%