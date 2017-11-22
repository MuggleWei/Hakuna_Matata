@echo off 

set dir=res\proto

set cpp_outdir=gen\cpp\proto
set java_outdir=gen\java\proto

IF not exist %cpp_outdir% (mkdir %cpp_outdir%)
IF not exist %java_outdir% (mkdir %java_outdir%)

for /R %dir% %%f in (*.proto) do ( 
	echo generate %%f
	protoc.exe --proto_path=%dir% --cpp_out=%cpp_outdir% %%~nxf
	protoc.exe --proto_path=%dir% --java_out=%java_outdir% %%~nxf
)

:: move into cpp example project
set cpp_targetdir=cpp\build\gen

IF exist %cpp_targetdir% (rmdir /s /q %cpp_targetdir%)
mkdir %cpp_targetdir%
MOVE /Y %cpp_outdir% %cpp_targetdir%

rmdir /s /q gen