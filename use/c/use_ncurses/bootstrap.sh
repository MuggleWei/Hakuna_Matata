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

# project root directory
origin_dir="$(dirname "$(readlink -f "$0")")"

# build option
build_mugglec=1
build_ncurses=1

# directories
scripts_dir=$origin_dir/scripts
build_dir=$origin_dir/build
pkg_dir=$build_dir/pkg
dep_dir=$origin_dir/_deps

# include scripts function
source $scripts_dir/func.sh

# create directories
if [ ! -d $build_dir ]; then
	echo "create build directory"
	mkdir -p $build_dir
fi

if [ ! -d $pkg_dir ]; then
	echo "create package directory"
	mkdir -p $pkg_dir
fi

if [ ! -d $dep_dir ]; then
	echo "create dependency directory"
	mkdir -p $dep_dir
fi

# mugglec
echo "------------------------"
if [ $build_mugglec -eq 1 ]; then
	echo "build mugglec"
	cd $dep_dir

	mugglec_git=https://github.com/MuggleWei/mugglec.git
	mugglec_tag=v1.3.0
	mugglec_name=mugglec-$mugglec_tag
	mugglec_src_dir=$dep_dir/$mugglec_name
	mugglec_build_dir=$build_dir/_deps/mugglec

	download_src $mugglec_name $mugglec_git $mugglec_tag $mugglec_src_dir

	if [ ! -d $mugglec_build_dir ]; then
		mkdir -p $mugglec_build_dir
	fi
	cd $mugglec_build_dir

	cmake \
		-S $mugglec_src_dir -B $mugglec_build_dir \
		-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
		-DMUGGLE_BUILD_SHARED_LIB=OFF \
		-DMUGGLE_BUILD_STATIC_PIC=ON \
		-DMUGGLE_BUILD_EXAMPLE=OFF \
		-DMUGGLE_BUILD_TESTING=OFF \
		-DMUGGLE_BUILD_BENCHMARK=OFF \
		-DMUGGLE_BUILD_TRACE=OFF \
		-DCMAKE_INSTALL_PREFIX=$pkg_dir

	cmake --build $mugglec_build_dir
	cmake --build $mugglec_build_dir --target install
	ln -sf $mugglec_build_dir/compile_commands.json $mugglec_src_dir/
else
	echo "option build mugglec off, ignore build mugglec"
fi

# ncurses
echo "------------------------"
if [ $build_ncurses -eq 1 ]; then
	echo "build ncurses"
	cd $dep_dir

	ncurses_git=https://github.com/mirror/ncurses.git
	ncurses_tag=v6.3
	ncurses_name=ncurses-$ncurses_tag
	ncurses_src_dir=$dep_dir/$ncurses_name
	ncurses_build_dir=$build_dir/_deps/ncurses

	download_src $ncurses_name $ncurses_git $ncurses_tag $ncurses_src_dir

	if [ ! -d $ncurses_build_dir ]; then
		mkdir -p $ncurses_build_dir
	fi
	cd $ncurses_build_dir

	if [ $BUILD_TYPE == "release" ]; then
		$ncurses_src_dir/configure --prefix=$pkg_dir --with-normal --without-debug
	else
		$ncurses_src_dir/configure --prefix=$pkg_dir --without-normal --with-debug
	fi

	bear -- make
	make install
	ln -sf $ncurses_build_dir/compile_commands.json $ncurses_src_dir
else
	echo "option build ncurses off, ignore build ncurses"
fi

# build
cd $build_dir
cmake .. \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$pkg_dir \
	-DCMAKE_INSTALL_PREFIX=$pkg_dir
ln -sf $build_dir/compile_commands.json $origin_dir/
