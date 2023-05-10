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

echo "Build Type: $BUILD_TYPE"

# dependencies
mugglec_repo="https://github.com/MuggleWei/mugglec.git"
mugglec_ver="v1.2.2"

# directories
origin_dir="$(dirname "$(readlink -f "$0")")"
build_dir=$origin_dir/build
pkg_dir=$build_dir/pkg
dep_dir=$origin_dir/_deps

if [ -d $dep_dir ]; then
	rm -rf $dep_dir
fi
mkdir -p $dep_dir

if [ -d $build_dir ]; then
	rm -rf $build_dir
fi
mkdir -p $build_dir

echo "------------------------"
echo "build mugglec"

cd $dep_dir

git clone --branch $mugglec_ver --depth 1 $mugglec_repo mugglec
status=$?
if [ $status -eq 0 ]; then
	echo "Success download mugglec"
else
	echo "Failed download mugglec"
	exit 1
fi

mugglec_src_dir=$dep_dir/mugglec
mugglec_build_dir=$dep_dir/build/mugglec

mkdir -p $mugglec_build_dir

cmake \
	-S $mugglec_src_dir -B $mugglec_build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_INSTALL_PREFIX=$pkg_dir
cmake --build $mugglec_build_dir
cmake --build $mugglec_build_dir --target install
ln -sf $mugglec_build_dir/compile_commands.json $mugglec_src_dir/

echo "------------------------"
echo "build current project"

cd $build_dir
cmake .. \
	-DCMAKE_PREFIX_PATH=$pkg_dir \
	-DCMAKE_INSTALL_PREFIX=$pkg_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE
ln -sf $build_dir/compile_commands.json $origin_dir/

cmake --build .
cmake --build . --target install
