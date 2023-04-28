#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[ERROR] bootstrap without build type"
	echo "[ERROR] Usage: bootstrap.sh <Debug|Release>"
	exit 1
else
	# to lowercase
	BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
fi

build_share_libs=ON

echo "Build Type: $BUILD_TYPE"
echo "Build Shared Lib: $build_shared_libs"

# directories
origin_dir="$(dirname "$(readlink -f "$0")")"
dep_dir=$origin_dir/_deps
build_dir=$origin_dir/build
pkg_dir=$build_dir/pkg
dep_pkg_dir=$build_dir/deps
test_dep_pkg_dir=$build_dir/test_deps

cd $origin_dir

# ensure deps exists
if [ ! -d $dep_dir ]; then
	$origin_dir/download_deps.sh true
	status=$?
	if [ $status -eq 0 ]; then
		echo "Success download dependencies"
	else
		echo "Failed download dependencies"
		exit 1
	fi
else
	echo "Found dependencies source directory: $dep_dir"
fi

# create build directories
build_dirs=("$build_dir" "$dep_pkg_dir" "$test_dep_pkg_dir")
for val in ${build_dirs[@]}; do
	if [ ! -d $val ]; then
		mkdir -p $val
		echo "Create directory: $val"
	else
		echo "Found directory: $val"
	fi
done

# build dependencies
unity_src_dir=$dep_dir/unity
unity_build_dir=$dep_dir/build/unity
if [ -d $unity_build_dir ]; then
	echo "Remove old unity build directory: $unity_build_dir"
	rm -rf $unity_build_dir
fi
mkdir -p $unity_build_dir

echo "Generate project: unity"
cmake \
	-S $unity_src_dir \
	-B $unity_build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DBUILD_SHARED_LIBS=$build_share_libs \
	-DCMAKE_INSTALL_PREFIX=$test_dep_pkg_dir

echo "Compile: unity"
cmake --build $unity_build_dir --config $BUILD_TYPE --target install

# project
cmake \
	-S $origin_dir \
	-B $build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DBUILD_SHARED_LIBS=$build_share_libs \
	-DCMAKE_PREFIX_PATH="$dep_pkg_dir;$test_dep_pkg_dir" \
	-DCMAKE_INSTALL_PREFIX=$pkg_dir \
	-DBUILD_TESTING=ON
