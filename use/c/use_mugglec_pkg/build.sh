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

# functions
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

# directories
origin_dir="$(dirname "$(readlink -f "$0")")"

build_dir=$origin_dir/build
pkg_dir=$build_dir/usr
dep_dir=$origin_dir/_deps

if [ ! -d $build_dir ]; then
	echo "create build directory"
	mkdir -p $build_dir
fi

if [ ! -d $dep_dir ]; then
	echo "create dependency directory"
	mkdir -p $dep_dir
fi

echo "------------------------"
echo "build mugglec"

cd $dep_dir

mugglec_git=https://github.com/MuggleWei/mugglec.git
mugglec_tag=v1.0.0-alpha.7
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
	-DMUGGLE_BUILD_SHARED_LIB=ON \
	-DMUGGLE_BUILD_STATIC_PIC=ON \
	-DMUGGLE_BUILD_EXAMPLE=OFF \
	-DMUGGLE_BUILD_TESTING=OFF \
	-DMUGGLE_BUILD_BENCHMARK=OFF \
	-DMUGGLE_BUILD_TRACE=OFF \
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
