@echo off

setlocal ENABLEDELAYEDEXPANSION

set BUILD_TYPE=Release

set origin_dir=%~dp0

: clean
cd %origin_dir%
call clean.bat

: build foo 1.1
set foo_1_1_dir=%origin_dir%foo_1.1
cd %foo_1_1_dir%

md build
cmake ^
	-S %foo_1_1_dir% -B %foo_1_1_dir%\build ^
	-DBUILD_SHARED_LIBS=ON ^
	-DCMAKE_INSTALL_PREFIX=%foo_1_1_dir%\dist
cmake --build %foo_1_1_dir%\build --config %BUILD_TYPE% --target install

: build foo 1.0
set foo_1_0_dir=%origin_dir%foo_1.0
cd %foo_1_0_dir%

md build
cmake ^
	-S %foo_1_0_dir% -B %foo_1_0_dir%\build ^
	-DBUILD_SHARED_LIBS=OFF ^
	-DCMAKE_INSTALL_PREFIX=%foo_1_0_dir%\dist ^
	-DFOO_VISIBILITY_HIDDEN=ON
cmake --build %foo_1_0_dir%\build --config %BUILD_TYPE% --target install

: build bar 1.0
set bar_1_0_dir=%origin_dir%bar_1.0
cd %bar_1_0_dir%

md build
cmake ^
	-S %bar_1_0_dir% -B %bar_1_0_dir%\build ^
	-DBUILD_SHARED_LIBS=ON ^
	-DCMAKE_PREFIX_PATH=%foo_1_0_dir%\dist ^
	-DCMAKE_INSTALL_PREFIX=%bar_1_0_dir%\dist
cmake --build %bar_1_0_dir%\build --config %BUILD_TYPE% --target install

: build foo 1.2
set foo_1_2_dir=%origin_dir%foo_1.2
cd %foo_1_2_dir%

md build
cmake ^
	-S %foo_1_2_dir% -B %foo_1_2_dir%\build ^
	-DBUILD_SHARED_LIBS=OFF ^
	-DCMAKE_INSTALL_PREFIX=%foo_1_2_dir%\dist
cmake --build %foo_1_2_dir%\build --config %BUILD_TYPE% --target install

: build bob 1.0
set bob_1_0_dir=%origin_dir%bob_1.0
cd %bob_1_0_dir%

md build
cmake ^
	-S %bob_1_0_dir% -B %bob_1_0_dir%\build ^
	-DBUILD_SHARED_LIBS=ON ^
	-DCMAKE_PREFIX_PATH=%foo_1_2_dir%\dist ^
	-DCMAKE_INSTALL_PREFIX=%bob_1_0_dir%\dist
cmake --build %bob_1_0_dir%\build --config %BUILD_TYPE% --target install

: build baz 1.0
set baz_1_0_dir=%origin_dir%baz_1.0
cd %baz_1_0_dir%

md build
cmake ^
	-S %baz_1_0_dir% -B %baz_1_0_dir%\build ^
	-DCMAKE_PREFIX_PATH="%foo_1_1_dir%\dist;%bar_1_0_dir%\dist;%bob_1_0_dir%\dist" ^
	-DCMAKE_INSTALL_PREFIX=%baz_1_0_dir%\dist
cmake --build %baz_1_0_dir%\build --config %BUILD_TYPE% --target install

: show symbols
cd %origin_dir%

echo.
dumpbin /EXPORTS foo_1.1\dist\bin\foo.dll

echo.
dumpbin /EXPORTS bar_1.0\dist\bin\bar.dll

echo.
dumpbin /EXPORTS bob_1.0\dist\bin\bob.dll

echo.
baz_1.0\dist\bin\baz.exe