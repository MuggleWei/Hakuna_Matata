@echo off

setlocal ENABLEDELAYEDEXPANSION

set origin_dir=%~dp0

:: build project
md %origin_dir%build
cd %origin_dir%build

cmake .. ^
	-G "Visual Studio 16 2019" -A x64 ^
	-DWOLFSSL_DEBUG=ON ^
	-DWOLFSSL_TLS13=ON ^
	-DWOLFSSL_DTLS=ON ^
	-DWOLFSSL_DTLS13=ON ^
	-DWOLFSSL_EXAMPLES=OFF ^
	-DWOLFSSL_CRYPT_TESTS=OFF ^
	-DWOLFSSL_CRYPT_TESTS_LIBS=OFF ^
	-DCMAKE_INSTALL_PREFIX=%origin_dir%build/pkg