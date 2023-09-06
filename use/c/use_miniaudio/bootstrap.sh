#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[ERROR] bootstrap without build type"
	echo "[ERROR] Usage: bootstrap.sh <Debug|Release|RelWithDebInfo>"
	exit 1
else
	# to lowercase
	BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
fi

echo "Build Type: $BUILD_TYPE"

# project root directory
origin_dir="$(dirname "$(readlink -f "$0")")"

# directories
dep_src_dir=$origin_dir/deps
dep_build_dir=$origin_dir/_deps/build
build_dir=$origin_dir/build
install_dir=$origin_dir/dist

# create directories
if [ ! -d $dep_build_dir ]; then
	echo "create build directory: $dep_build_dir"
	mkdir -p $dep_build_dir
fi

if [ ! -d $build_dir ]; then
	echo "create build directory: $build_dir"
	mkdir -p $build_dir
fi

if [ ! -d $install_dir ]; then
	echo "create install directory: $install_dir"
	mkdir -p $install_dir
fi

# build deps
cmake \
	-S $dep_src_dir -B $dep_build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build $dep_build_dir --target install

# build
cmake \
	-S $origin_dir -B $build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install_dir
