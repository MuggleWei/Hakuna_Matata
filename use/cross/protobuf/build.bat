@echo off

setlocal ENABLEDELAYEDEXPANSION

:: handle argv
set argc=0
for %%x in (%*) do set /A argc+=1
echo argc=%argc%

if %argc% LSS 1 (
	echo "[WARNING] Usage: build.bat <Debug|Release|RelWithDebInfo|MinSizeRel>"
	echo "[INFO] input without build type, use Release by default"
	set BUILD_TYPE=Release
) else (
	set BUILD_TYPE=%1
)

echo Build Type: %BUILD_TYPE%

set build_protobuf=1
set build_source=1
set build_cpp=1
set build_java=1

:: script directory
set origin_dir=%~dp0

set pkg_dir=%origin_dir%build\pkg
set protobuf_dir=%origin_dir%protobuf
set protoc=%pkg_dir%\bin\protoc.exe

set res_dir=%origin_dir%res\proto

set vs_gen="Visual Studio 16 2019"
set vs_arch="x64"

:: create package directory
if not exist %pkg_dir% (
	md %pkg_dir%
)

:: build protobuf
echo "------------------------"
if %build_protobuf% EQU 1 (
	echo "build protobuf"
	md %protobuf_dir%\build
	cd %protobuf_dir%\build
	cmake .. ^
		-G %vs_gen% -A %vs_arch%^
		-DBUILD_SHARED_LIBS=ON ^
		-Dprotobuf_BUILD_SHARED_LIBS=ON ^
		-Dprotobuf_BUILD_TESTS=OFF ^
		-DCMAKE_INSTALL_PREFIX=%pkg_dir%

	cmake --build . --config %BUILD_TYPE%
	cmake --build . --target INSTALL
) else (
	echo "build protobuf off, ignore protobuf"
)

:: build cpp
echo "------------------------"

cd %origin_dir%

set cpp_dir=%origin_dir%cpp
set cpp_build_dir=%cpp_dir%\build
set cpp_proto_dir=%cpp_build_dir%\gen\proto

if %build_cpp% EQU 1 (
	echo "build cpp"

	if exist %cpp_proto_dir% (
		rd /s /q %cpp_proto_dir%
	)
	md %cpp_proto_dir%

	echo "------------------------"
	echo "generate cpp protobuf"

	for /R %res_dir% %%f in (*.proto) do ( 
		echo generate %%f
		%protoc% --proto_path=%res_dir% --cpp_out=%cpp_proto_dir% %%~nxf
		set filelist=%%~nxf !filelist!
	)

	echo "------------------------"
	if %build_source% EQU 1 (
		echo "build cpp project"

		md %cpp_build_dir%
		cd %cpp_build_dir%
		cmake .. -DCMAKE_PREFIX_PATH=%pkg_dir% -DCMAKE_INSTALL_PREFIX=%pkg_dir%
		cmake --build . --config %BUILD_TYPE%
		cmake --build . --target INSTALL
	) else (
		echo "build source off, ignore build cpp source"
	)
) else (
	echo "build cpp off, ignore cpp"
)

:: build java
echo "------------------------"

cd %origin_dir%

set java_dir=%origin_dir%java
set java_proto_dir=%java_dir%\example\src\main\java
set java_desc_dir=%java_dir%\example\src\main\resources\proto\desc
set java_proto_gen_dir=%java_proto_dir%\gen

if %build_java% EQU 1 (
	echo "build java"

	if exist %java_proto_gen_dir% (
		rd /s /q %java_proto_gen_dir%
	)
	md %java_proto_gen_dir%

	if exist %java_desc_dir% (
		rd /s /q %java_desc_dir%
	)
	md %java_desc_dir%

	echo "------------------------"
	echo "generate java protobuf"

	set filelist=
	for /R %res_dir% %%f in (*.proto) do ( 
		echo generate %%f
		%protoc% --proto_path=%res_dir% --java_out=%java_proto_dir% %%~nxf
		set filelist=%%~nxf !filelist!
	)

	echo generate descriptor set file
	%protoc% --proto_path=%res_dir% --descriptor_set_out=%java_desc_dir%\proto.desc !filelist!

	echo "------------------------"
	if %build_source% EQU 1 (
		echo "build java project"

		cd %java_dir%
		REM mvn clean package -Dmaven.test.skip=true
		mvn clean package
	) else (
		echo "build source off, ignore build java source"
	)
) else (
	echo "build java off, ignore java"
)

:: cd origin_dir
cd %origin_dir%
