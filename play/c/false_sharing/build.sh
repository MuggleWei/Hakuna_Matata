#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[WARNING] Usage: bootstrap.sh <Debug|Release|RelWithDebInfo>"
	echo "[WARNING] bootstrap without build type, use release by default"
	BUILD_TYPE=RelWithDebInfo
fi

# to lowercase
BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
echo "Build Type: $BUILD_TYPE"

# project root directory
origin_dir="$(dirname "$(readlink -f "$0")")"

# directories
build_dir=$origin_dir/build
install_dir=$build_dir/dist
dep_dir=$origin_dir/_deps

# create directories
if [ ! -d $build_dir ]; then
	echo "create build directory"
	mkdir -p $build_dir
fi

if [ ! -d $install_dir ]; then
	echo "create install directory"
	mkdir -p $install_dir
fi

if [ ! -d $dep_dir ]; then
	echo "create dependency directory"
	mkdir -p $dep_dir
fi

# function
download_src() {
	local name=$1
	local git_repo=$2
	local tag=$3
	local src_dir=$4

	if [ -d $src_dir ]; then
		echo "$name source directory already exists"
	else
		echo "$name source directory not exists, clone"
		echo "name=$name"
		echo "git_repo=$git_repo"
		echo "tag=$tag"
		echo "src_dir=$src_dir"

		git clone --branch $tag --depth 1 $git_repo $src_dir
	fi
}


# build option
build_mugglec=1

# mugglec
echo "------------------------"
if [ $build_mugglec -eq 1 ]; then
	echo "build mugglec"
	cd $dep_dir

	mugglec_git=https://github.com/MuggleWei/mugglec.git
	mugglec_tag=v1.4.4
	mugglec_name=mugglec-$mugglec_tag
	mugglec_src_dir=$dep_dir/$mugglec_name
	mugglec_build_dir=$build_dir/_deps/mugglec

	if [ ! -d $mugglec_src_dir ]; then
		download_src $mugglec_name $mugglec_git $mugglec_tag ${mugglec_src_dir}_tmp
		mv ${mugglec_src_dir}_tmp $mugglec_src_dir
	fi

	if [ ! -d $mugglec_build_dir ]; then
		mkdir -p $mugglec_build_dir
	fi
	cd $mugglec_build_dir

	cmake \
		-S $mugglec_src_dir -B $mugglec_build_dir \
		-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
		-DBUILD_SHARED_LIBS=ON \
		-DMUGGLE_BUILD_STATIC_PIC=ON \
		-DMUGGLE_BUILD_TRACE=OFF \
		-DMUGGLE_BUILD_SANITIZER=OFF \
		-DBUILD_TESTING=OFF \
		-DMUGGLE_BUILD_EXAMPLE=OFF \
		-DMUGGLE_BUILD_BENCHMARK=OFF \
		-DCMAKE_INSTALL_PREFIX=$install_dir

	cmake --build $mugglec_build_dir
	cmake --build $mugglec_build_dir --target install
else
	echo "option build mugglec off, ignore build mugglec"
fi
cd $origin_dir

# build
cd $origin_dir
cmake \
	-S $origin_dir -B $build_dir \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$install_dir \
	-DCMAKE_INSTALL_PREFIX=$install_dir
cmake --build $build_dir
cmake --build $build_dir --target install
