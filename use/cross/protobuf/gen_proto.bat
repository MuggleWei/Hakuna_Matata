@echo off 

setlocal ENABLEDELAYEDEXPANSION

set dir=res\proto

set cpp_outdir=gen\cpp\proto
set java_outdir=gen\java\proto
set desc_outdir=gen\proto\desc

IF not exist %cpp_outdir% (mkdir %cpp_outdir%)
IF not exist %java_outdir% (mkdir %java_outdir%)
IF not exist %desc_outdir% (mkdir %desc_outdir%)

for /R %dir% %%f in (*.proto) do ( 
	echo generate %%f
	protoc.exe --proto_path=%dir% --cpp_out=%cpp_outdir% %%~nxf
	protoc.exe --proto_path=%dir% --java_out=%java_outdir% %%~nxf
::	protoc.exe --proto_path=%dir% --descriptor_set_out=%desc_outdir%\%%~nxf.desc %%~nxf
	set filelist=%%~nxf !filelist!
)

protoc.exe --proto_path=%dir% --descriptor_set_out=%desc_outdir%\proto.desc !filelist!

:: move into cpp example project
set cpp_targetdir=cpp\build\gen\proto

IF exist %cpp_targetdir% (rmdir /s /q %cpp_targetdir%)
mkdir %cpp_targetdir%
MOVE /Y %cpp_outdir%\* %cpp_targetdir%

:: move into java example project
set java_targetdir=java\src\main\java\gen\proto
set java_descdir=java\src\main\resources\proto\desc

IF exist %java_targetdir% (rmdir /s /q %java_targetdir%)
mkdir %java_targetdir%
move /Y %java_outdir%\gen\proto\* %java_targetdir%

:: java need descriptor to generate message dynamiclly
IF exist %java_descdir% (rmdir /s /q %java_descdir%)
mkdir %java_descdir%
MOVE /Y %desc_outdir%\* %java_descdir%

rmdir /s /q gen

endlocal