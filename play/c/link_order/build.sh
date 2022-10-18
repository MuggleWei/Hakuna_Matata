#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[ERROR] build without build type"
	echo "[ERROR] Usage: build.sh <Debug|Release>"
	exit 1
else
	# to lowercase
	BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
fi

echo "Build Type: $BUILD_TYPE"

# project root directory
origin_dir="$(dirname "$(readlink -f "$0")")"

pkg_dir=$origin_dir/build/pkg
mkdir -p $pkg_dir

# build foo
cd $origin_dir/base-1.0
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$pkg_dir
cmake --build . 
cmake --build . --target install

cd $origin_dir/foo-1.0
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$pkg_dir -DCMAKE_PREFIX_PATH=$pkg_dir
cmake --build . 
cmake --build . --target install

cd $origin_dir

# build bar
rm -rf $pkg_dir/lib/static
rm -rf $pkg_dir/include/base

cd $origin_dir/base-1.1
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$pkg_dir
cmake --build . 
cmake --build . --target install

cd $origin_dir/bar-1.0
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$pkg_dir -DCMAKE_PREFIX_PATH=$pkg_dir
cmake --build . 
cmake --build . --target install

cd $origin_dir

# build app
cd $origin_dir/app-1.0
mkdir build
#gcc app.c -I $pkg_dir/include -L $pkg_dir/lib -lfoo -lbar -o build/app
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$pkg_dir
cmake --build . 
cmake --build . --target install

# show sym
echo "---------------------------------"
echo "foo.base sym"
objdump -t $origin_dir/build/pkg/lib/libfoo.so | grep "base_"
echo ""

echo "bar.base sym"
objdump -t $origin_dir/build/pkg/lib/libbar.so | grep "base_"
echo ""

# run
echo "---------------------------------"
echo "run app"
cd $origin_dir/build/pkg/bin
./app
cd $origin
