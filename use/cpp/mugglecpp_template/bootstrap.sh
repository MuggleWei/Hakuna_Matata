#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[WARNING] Usage: build.sh <Debug|Release>"
	echo "[INFO] input without build type, use Release by default"
	BUILD_TYPE=Release
else
	BUILD_TYPE=$1
fi

echo "Build Type: $BUILD_TYPE"

# script directory
origin_dir="$(dirname "$(readlink -f "$0")")"
cd $origin_dir

mkdir build
cd build
cmake .. \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DBUILD_SHARED_LIBS=ON \
	-DMUGGLE_CPP_BUILD_SHARED_LIB_DEFAULT=ON
cmake --build .
