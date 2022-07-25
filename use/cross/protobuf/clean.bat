@echo off

set origin_dir=%~dp0

set clear_protobuf=1

:: clean package
echo "--------------------------------"
echo "clean package directory"

set pkg_dir=%origin_dir%build\pkg

echo remove dir: %pkg_dir%
if exist %pkg_dir% (
    RD /S /Q %pkg_dir%
)

:: clean protobuf
echo "--------------------------------"
if %clear_protobuf% EQU 1 (
	echo "clean protobuf directory"

	set protobuf_build_dir=%origin_dir%protobuf\build

	echo remove dir: %protobuf_build_dir%
	if exist %protobuf_build_dir% (
	    RD /S /Q %protobuf_build_dir%
	)
) else (
	echo "clear_protobuf off, ignore clear protobuf"
)

:: clean cpp example
echo "--------------------------------"
echo "clean cpp example directory"

set cpp_build_dir=%origin_dir%cpp\build

echo remove dir: %cpp_build_dir%
if exist %cpp_build_dir% (
    RD /S /Q %cpp_build_dir%
)

:: clean java example
echo "--------------------------------"
echo "clean java example directory"

set java_dir=%origin_dir%java
set java_proto_gen_dir=%java_dir%\example\src\main\java\gen
set java_desc_dir=%java_dir%\example\src\main\resources\proto\desc

echo "maven clean"
cd %java_dir%
call mvn clean
cd %origin_dir%

echo remove dir: %java_proto_gen_dir%
if exist %java_proto_gen_dir% (
    RD /S /Q %java_proto_gen_dir%
)

echo remove dir: %java_desc_dir%
if exist %java_desc_dir% (
    RD /S /Q %java_desc_dir%
)

:: cd origin_dir
cd %origin_dir%
