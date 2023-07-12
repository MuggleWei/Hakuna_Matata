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

build_dir=$origin_dir/build
deps_dir=$origin_dir/_deps
install_dir=$build_dir/dist

mkdir build
cd build
cmake -S $origin_dir -B $build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DBUILD_SHARED_LIBS=ON \
	-DCMAKE_INSTALL_PREFIX=$install_dir \
	-DMUGGLE_CPP_DOWNLOAD_MUGGLEC=ON
cmake --build .
cmake --build . --target install
