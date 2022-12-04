#!/bin/bash

origin_dir="$(dirname "$(readlink -f "$0")")"
install_dir=$origin_dir/build/usr
pkg_dir=$origin_dir/build/pkg
mkdir -p $install_dir
mkdir -p $pkg_dir

# build and install lib
echo "--------------------"
echo "build foo"
cd $origin_dir/foo
mkdir -p build
cd build
cmake .. \
	-DBUILD_SHARED_LIBS=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build .
cmake --build . --target install
cpack

mv ./foo-*.tar.gz $pkg_dir

# build bin
echo "--------------------"
echo "build bar"
cd $origin_dir/bar
mkdir -p build
cd build
cmake .. \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build .
cmake --build . --target install

echo "--------------------"
echo "build baz"
cd $origin_dir/baz
mkdir -p build
cd build
cmake .. \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build .
cmake --build . --target install

echo "--------------------"
echo "build bob"
cd $origin_dir/bob
PKG_CONFIG_PATH=$install_dir/lib/pkgconfig meson setup -Dprefix=$install_dir build
cd build
PKG_CONFIG_PATH=$install_dir/lib/pkgconfig meson compile
meson install
