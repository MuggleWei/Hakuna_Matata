#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[ERROR] build.sh without build type"
	echo "[ERROR] Usage: build.sh <Debug|Release>"
	exit 1
else
	# to lowercase
	BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
fi

origin_dir="$(dirname "$(readlink -f "$0")")"

# gen certs
cd $origin_dir
if [ -d build/certs ]; then
	echo "$origin_dir/build/certs is already exists"
else
	echo "$origin_dir/build/certs not exists, gen certs"
	./gen_cert.sh
fi

# build project
mkdir -p $origin_dir/build
cd $origin_dir/build

cmake .. \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DWOLFSSL_DEBUG=ON \
	-DWOLFSSL_TLS13=ON \
	-DWOLFSSL_DTLS=ON \
	-DWOLFSSL_DTLS13=ON \
	-DWOLFSSL_EXAMPLES=OFF \
	-DWOLFSSL_CRYPT_TESTS=OFF \
	-DWOLFSSL_CRYPT_TESTS_LIBS=OFF \
	-DCMAKE_INSTALL_PREFIX=$origin_dir/dist
