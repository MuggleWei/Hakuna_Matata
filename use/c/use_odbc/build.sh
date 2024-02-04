#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[ERROR] build.sh without build type"
	echo "[ERROR] Usage: build.sh <Debug|Release|RelWithDebInfo|Coverage>"
	exit 1
else
	# to lowercase
	BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
fi

origin_dir="$(dirname "$(readlink -f "$0")")"
build_dir=$origin_dir/build
deps_dir=~/.local/opt/unixodbc

cd $origin_dir

mkdir -p $build_dir

cmake \
	-S $origin_dir -B $build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$deps_dir
cmake --build .
