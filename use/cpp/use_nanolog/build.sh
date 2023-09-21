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
build_mugglecpp=1
build_nanolog=1

# mugglec
echo "------------------------"
if [ $build_mugglec -eq 1 ]; then
	echo "build mugglec"
	cd $dep_dir

	mugglec_git=https://github.com/MuggleWei/mugglec.git
	mugglec_tag=v1.3.2
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

# mugglecpp
echo "------------------------"
if [ $build_mugglecpp -eq 1 ]; then
	echo "build mugglecpp"
	cd $dep_dir

	mugglecpp_git=https://github.com/MuggleWei/mugglecpp.git
	mugglecpp_tag=v1.2.3
	mugglecpp_name=mugglecpp-$mugglecpp_tag
	mugglecpp_src_dir=$dep_dir/$mugglecpp_name
	mugglecpp_build_dir=$build_dir/_deps/mugglecpp

	if [ ! -d $mugglecpp_src_dir ]; then
		download_src $mugglecpp_name $mugglecpp_git $mugglecpp_tag ${mugglecpp_src_dir}_tmp
		mv ${mugglecpp_src_dir}_tmp $mugglecpp_src_dir
	fi

	if [ ! -d $mugglecpp_build_dir ]; then
		mkdir -p $mugglecpp_build_dir
	fi
	cd $mugglecpp_build_dir

	cmake \
		-S $mugglecpp_src_dir -B $mugglecpp_build_dir \
		-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
		-DBUILD_SHARED_LIBS=ON \
		-DCMAKE_INSTALL_PREFIX=$install_dir

	cmake --build $mugglecpp_build_dir
	cmake --build $mugglecpp_build_dir --target install
else
	echo "option build mugglecpp off, ignore build mugglecpp"
fi
cd $origin_dir

# nanolog
echo "------------------------"
if [ $build_nanolog -eq 1 ]; then
	echo "build nanolog"
	cd $dep_dir

	nanolog_git=https://github.com/PlatformLab/NanoLog.git
	nanolog_tag=master
	nanolog_name=nanolog-$nanolog_tag
	nanolog_src_dir=$dep_dir/$nanolog_name
	nanolog_build_dir=$build_dir/_deps/nanolog

	if [ ! -d $nanolog_src_dir ]; then
		download_src $nanolog_name $nanolog_git $nanolog_tag ${nanolog_src_dir}_tmp
		mv ${nanolog_src_dir}_tmp $nanolog_src_dir
	fi

	cd $nanolog_src_dir
	cd runtime
	bear -- make

	mkdir -p ${install_dir}/include/nanolog
	mkdir -p ${install_dir}/lib/
	mkdir -p ${install_dir}/bin/
	cp *.h ${install_dir}/include/nanolog
	cp libNanoLog.a ${install_dir}/lib/
	cp decompressor ${install_dir}/bin/
else
	echo "option build nanolog off, ignore build nanolog"
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
