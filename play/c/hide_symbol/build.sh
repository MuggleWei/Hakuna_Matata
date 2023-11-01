#!/bin/bash

# # handle argv
# if [ "$#" -lt 1 ]; then
# 	echo "[ERROR] bootstrap without build type"
# 	echo "[ERROR] Usage: bootstrap.sh <Debug|Release>"
# 	exit 1
# else
# 	# to lowercase
# 	BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
# fi
BUILD_TYPE=Release

# directories
origin_dir="$(dirname "$(readlink -f "$0")")"

# build foo 1.1
foo_1_1_dir=$origin_dir/foo_1.1
cd $foo_1_1_dir

if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi

mkdir build
cmake \
	-S $foo_1_1_dir -B $foo_1_1_dir/build \
	-DBUILD_SHARED_LIBS=static \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_INSTALL_PREFIX=$foo_1_1_dir/dist
cmake --build $foo_1_1_dir/build --target install

# build foo 1.0
foo_1_0_dir=$origin_dir/foo_1.0
cd $foo_1_0_dir

if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi

mkdir build
cmake \
	-S $foo_1_0_dir -B $foo_1_0_dir/build \
	-DBUILD_SHARED_LIBS=static \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_INSTALL_PREFIX=$foo_1_0_dir/dist
cmake --build $foo_1_0_dir/build --target install

# build bar 1.0
bar_1_0_dir=$origin_dir/bar_1.0
cd $bar_1_0_dir

if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi

mkdir build
cmake \
	-S $bar_1_0_dir -B $bar_1_0_dir/build \
	-DBUILD_SHARED_LIBS=static \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$foo_1_0_dir/dist \
	-DCMAKE_INSTALL_PREFIX=$bar_1_0_dir/dist
cmake --build $bar_1_0_dir/build --target install

# build foo 1.2
foo_1_2_dir=$origin_dir/foo_1.2
cd $foo_1_2_dir

if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi

mkdir build
cmake \
	-S $foo_1_2_dir -B $foo_1_2_dir/build \
	-DBUILD_SHARED_LIBS=static \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_INSTALL_PREFIX=$foo_1_2_dir/dist
cmake --build $foo_1_2_dir/build --target install

# build bob 1.0
bob_1_0_dir=$origin_dir/bob_1.0
cd $bob_1_0_dir

if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi

mkdir build
cmake \
	-S $bob_1_0_dir -B $bob_1_0_dir/build \
	-DBUILD_SHARED_LIBS=static \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$foo_1_2_dir/dist \
	-DCMAKE_INSTALL_PREFIX=$bob_1_0_dir/dist
cmake --build $bob_1_0_dir/build --target install

# build baz 1.0
baz_1_0_dir=$origin_dir/baz_1.0
cd $baz_1_0_dir

if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi

mkdir build
cmake \
	-S $baz_1_0_dir -B $baz_1_0_dir/build \
	-DBUILD_SHARED_LIBS=static \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH="$bar_1_0_dir/dist;$foo_1_1_dir/dist;$bob_1_0_dir/dist" \
	-DCMAKE_INSTALL_PREFIX=$baz_1_0_dir/dist
cmake --build $baz_1_0_dir/build --target install

# readelf
echo ""
echo "readelf -s $foo_1_1_dir/dist/lib/libfoo.so | grep hello"
readelf -s $foo_1_1_dir/dist/lib/libfoo.so | grep hello

echo ""
echo "readelf -s $foo_1_0_dir/dist/lib/libfoo.a | grep hello"
readelf -s $foo_1_0_dir/dist/lib/libfoo.a | grep hello

echo ""
echo "readelf -s $bar_1_0_dir/dist/lib/libbar.so | grep hello"
readelf -s $bar_1_0_dir/dist/lib/libbar.so | grep hello

echo ""
echo "readelf -s $foo_1_2_dir/dist/lib/libfoo.a | grep hello"
readelf -s $foo_1_2_dir/dist/lib/libfoo.a | grep hello

echo ""
echo "readelf -s $bob_1_0_dir/dist/lib/libbob.so | grep hello"
readelf -s $bob_1_0_dir/dist/lib/libbob.so | grep hello

echo ""
echo "readelf -s $baz_1_0_dir/dist/bin/baz | grep hello"
readelf -s $baz_1_0_dir/dist/bin/baz | grep hello

echo ""
$baz_1_0_dir/dist/bin/baz
