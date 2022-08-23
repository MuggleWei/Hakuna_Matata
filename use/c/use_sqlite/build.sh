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

build_sqlite=1

# script directory
origin_dir="$(dirname "$(readlink -f "$0")")"

pkg_dir=$origin_dir/build/pkg
sqlite_dir=$origin_dir/sqlite

# create package directory
mkdir -p $pkg_dir

# build sqlite
echo "------------------------"
if [ $build_sqlite -eq 1 ]; then
	echo "build sqlite"
	mkdir -p $sqlite_dir/build
	cd $sqlite_dir/build

	if [ -d $sqlite_dir/build/sqlite-src ]; then
		echo "sqlite source directory already exists"
	else
		echo "sqlite source directory not exists, clone"
		git clone \
			--branch version-3.39.2 \
			--depth 1 \
			https://github.com/sqlite/sqlite.git sqlite-src
	fi

	if [ -d $sqlite_dir/build/sqlite-build ]; then
		rm -rf $sqlite_dir/build/sqlite-build
	fi
	mkdir -p $sqlite_dir/build/sqlite-build
	cd $sqlite_dir/build/sqlite-build

	if [ $BUILD_TYPE -eq "Release" ]; then
		$sqlite_dir/build/sqlite-src/configure --prefix=$pkg_dir
	else
		$sqlite_dir/build/sqlite-src/configure --prefix=$pkg_dir --enable-debug
	fi

	make && make install
else
	echo "build sqlite off, ignore sqlite"
fi

# build c examples
echo "------------------------"
cd $origin_dir
echo "build c examples"

c_src_dir=$origin_dir/c
c_build_dir=$origin_dir/c/build

mkdir -p $c_build_dir
cd $c_build_dir
cmake .. \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$pkg_dir \
	-DCMAKE_INSTALL_PREFIX=$pkg_dir
make && make install
