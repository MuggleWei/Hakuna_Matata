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

echo "Build Type: $BUILD_TYPE"

# directories
origin_dir="$(dirname "$(readlink -f "$0")")"
dep_dir=$origin_dir/_deps
build_dir=$origin_dir/build
pkg_dir=$build_dir/pkg
dep_pkg_dir=$build_dir/deps

cd $origin_dir

# ensure deps exists
if [ ! -d $dep_dir ]; then
	$origin_dir/download_deps.sh
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

# create build directory
if [ ! -d $build_dir ]; then
	mkdir -p $build_dir
	echo "Create build directory: $build_dir"
else
	echo "Found build directory: $build_dir"
fi

# create deps package directory
if [ ! -d $dep_pkg_dir ]; then
	mkdir -p $dep_pkg_dir
	echo "Create dependencies package directory: $dep_pkg_dir"
else
	echo "Found dependencies package directory: $dep_pkg_dir"
fi

# build dependencies
libxml2_src_dir=$dep_dir/libxml2
libxml2_build_dir=$dep_dir/build/libxml2
if [ -d $libxml2_build_dir ]; then
	echo "Remove old libxml2 build directory: $libxml2_build_dir"
	rm -rf $libxml2_build_dir
fi
mkdir -p $libxml2_build_dir

echo "Generate project: libxml2"
cmake \
	-S $libxml2_src_dir \
	-B $libxml2_build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DBUILD_SHARED_LIBS=ON \
	-DCMAKE_INSTALL_PREFIX=$dep_pkg_dir \
	-DLIBXML2_WITH_PROGRAMS=OFF \
	-DLIBXML2_WITH_TESTS=OFF \
	-DLIBXML2_WITH_ICONV=OFF \
	-DLIBXML2_WITH_LZMA=OFF \
	-DLIBXML2_WITH_PYTHON=OFF \
	-DLIBXML2_WITH_ZLIB=OFF

echo "Compile: libxml2"
cmake --build $libxml2_build_dir --config $BUILD_TYPE --target install

mugglec_src_dir=$dep_dir/mugglec
mugglec_build_dir=$dep_dir/build/mugglec
if [ -d $mugglec_build_dir ]; then
	echo "Remove old mugglec build directory: $mugglec_build_dir"
	rm -rf $mugglec_build_dir
fi
mkdir -p $mugglec_build_dir

echo "Generate project: mugglec"
cmake \
	-S $mugglec_src_dir \
	-B $mugglec_build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DBUILD_SHARED_LIBS=ON \
	-DCMAKE_INSTALL_PREFIX=$dep_pkg_dir \
	-DBUILD_TESTING=OFF

echo "Compile: mugglec"
cmake --build $mugglec_build_dir --config $BUILD_TYPE --target install

# build examples
if [ ! -d $build_dir ];then
	mkdir -p $build_dir
fi

cmake \
	-S $origin_dir \
	-B $build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$dep_pkg_dir
