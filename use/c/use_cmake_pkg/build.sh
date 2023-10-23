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

build_shared_libs=ON

# directories
origin_dir="$(dirname "$(readlink -f "$0")")"
install_dir=$origin_dir/build/usr
install2_dir=$origin_dir/build/usr2
pkg_dir=$origin_dir/build/pkg

if [ ! -d $install_dir ]; then
	mkdir -p $install_dir
fi
if [ ! -d $pkg_dir ]; then
	mkdir -p $pkg_dir
fi

# build and install lib
echo "--------------------"
echo "build foo"

cd $origin_dir
foo_dir=$origin_dir/foo
foo_build_dir=$foo_dir/build
if [ ! -d $foo_build_dir ]; then
	mkdir -p $foo_build_dir
fi

cmake \
	-S $foo_dir \
	-B $foo_build_dir \
	-DBUILD_SHARED_LIBS=$build_shared_libs \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build $foo_build_dir
cmake --build $foo_build_dir --target install

cd $foo_build_dir
cpack

mv ./foo-*.tar.gz $pkg_dir

# build bar
echo "--------------------"
echo "build bar"

cd $origin_dir
bar_dir=$origin_dir/bar
bar_build_dir=$bar_dir/build
if [ ! -d $bar_build_dir ]; then
	mkdir -p $bar_build_dir
fi

cmake \
	-S $bar_dir \
	-B $bar_build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build $bar_build_dir
cmake --build $bar_build_dir --target install

echo "--------------------"
echo "build baz"

cd $origin_dir
baz_dir=$origin_dir/baz
baz_build_dir=$baz_dir/build
if [ ! -d $baz_build_dir ]; then
	mkdir -p $baz_build_dir
fi

cmake \
	-S $baz_dir \
	-B $baz_build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build $baz_build_dir
cmake --build $baz_build_dir --target install

echo "--------------------"
echo "build bob"

cd $origin_dir/bob
PKG_CONFIG_PATH=$install_dir/lib/pkgconfig meson setup -Dprefix=$install_dir build
cd build
PKG_CONFIG_PATH=$install_dir/lib/pkgconfig meson compile
meson install

# build alice
echo "--------------------"
echo "build alice"

cd $origin_dir
alice_dir=$origin_dir/alice
alice_build_dir=$alice_dir/build
if [ ! -d $alice_build_dir ]; then
	mkdir -p $alice_build_dir
fi

cmake \
	-S $alice_dir \
	-B $alice_build_dir \
	-DBUILD_SHARED_LIBS=$build_shared_libs \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install2_dir
cmake --build $alice_build_dir
cmake --build $alice_build_dir --target install

# build pat
echo "--------------------"
echo "build pat"

cd $origin_dir
pat_dir=$origin_dir/pat
pat_build_dir=$pat_dir/build
if [ ! -d $pat_build_dir ]; then
	mkdir -p $pat_build_dir
fi

cmake \
	-S $pat_dir \
	-B $pat_build_dir \
	-DBUILD_SHARED_LIBS=$build_shared_libs \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$install2_dir \
	-DCMAKE_INSTALL_PREFIX=$install2_dir
cmake --build $pat_build_dir
cmake --build $pat_build_dir --target install

# build dave
echo "--------------------"
echo "build dave"

install2_dir=$origin_dir/build/usr2

cd $origin_dir
dave_dir=$origin_dir/dave
dave_build_dir=$dave_dir/build
if [ ! -d $dave_build_dir ]; then
	mkdir -p $dave_build_dir
fi

cmake \
	-S $dave_dir \
	-B $dave_build_dir \
	-DBUILD_SHARED_LIBS=$build_shared_libs \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build $dave_build_dir
cmake --build $dave_build_dir --target install

# build peggy
echo "--------------------"
echo "build peggy"

cd $origin_dir
peggy_dir=$origin_dir/peggy
peggy_build_dir=$peggy_dir/build
if [ ! -d $peggy_build_dir ]; then
	mkdir -p $peggy_build_dir
fi

cmake \
	-S $peggy_dir \
	-B $peggy_build_dir \
	-DBUILD_SHARED_LIBS=$build_shared_libs \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build $peggy_build_dir
cmake --build $peggy_build_dir --target install
