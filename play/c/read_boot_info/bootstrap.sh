#!/bin/bash

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
build_util_linux=1

# directories
scripts_dir=$origin_dir/scripts
build_dir=$origin_dir/build
pkg_dir=$build_dir/pkg
dep_dir=$origin_dir/_deps

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

# download mugglec
echo "------------------------"
if [ $build_mugglec -eq 1 ]; then
	echo "download mugglec"
	cd $dep_dir

	mugglec_git=https://github.com/MuggleWei/mugglec.git
	mugglec_tag=v1.0.0-alpha.3
	mugglec_name=mugglec-$mugglec_tag
	mugglec_src_dir=$dep_dir/$mugglec_name
	mugglec_build_dir=$build_dir/_deps/mugglec

	download_src $mugglec_name $mugglec_git $mugglec_tag $mugglec_src_dir
else
	echo "option build mugglec off, ignore download mugglec"
fi


# download util-linux
echo "------------------------"
if [ $build_util_linux -eq 1 ]; then
	echo "download util-linux"

	util_linux_git="https://github.com/util-linux/util-linux.git"
	util_linux_tag="v2.38.1"
	util_linux_name=util_linux-$util_linux_tag
	util_linux_src_dir=$dep_dir/$util_linux_name
	util_linux_build_dir=$build_dir/_deps/util_linux

	download_src $util_linux_name $util_linux_git $util_linux_tag $util_linux_src_dir
else
	echo "option build util-linux off, ignore download util-linux"
fi

# build mugglec
echo "------------------------"
if [ $build_mugglec -eq 1 ]; then
	echo "build mugglec"
	cd $dep_dir

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
else
	echo "option build mugglec off, ignore build mugglec"
fi

# build util-linux
echo "------------------------"
if [ $build_util_linux -eq 1 ]; then
	echo "build util-linux"

	if [ ! -d $util_linux_build_dir ]; then
		mkdir -p $util_linux_build_dir
	fi

	cd $util_linux_src_dir

	##################################################
	# fix install error

	# -execprefixdir = '/usr'
	# +execprefixdir = prefixdir
	sed -i "24s/.*/execprefixdir = prefixdir/" meson.build

	# -  systemdsystemunitdir = systemd.get_pkgconfig_variable('systemdsystemunitdir')
	# +  systemdsystemunitdir = join_paths(prefixdir, 'systemd')
	sed -i "725s/.*/  systemdsystemunitdir = join_paths(prefixdir, 'systemd')/" meson.build

	# -      install_dir : bash_completion.get_variable(pkgconfig : 'completionsdir')
	# +      install_dir : join_paths(prefixdir, 'completionsdir')
	sed -i "3234s/.*/      install_dir : join_paths(prefixdir, 'completionsdir')/" meson.build

	# -      join_paths(bash_completion.get_variable(pkgconfig : 'completionsdir'), link_name))
	# +      join_paths(join_paths(prefixdir, 'completionsdir'), link_name))
	sed -i "3240s/.*/      join_paths(join_paths(prefixdir, 'completionsdir'), link_name))/" meson.build

	##################################################

	meson setup $util_linux_build_dir

	cd $util_linux_build_dir
	if [ $BUILD_TYPE == "release" ]; then
		meson configure \
			--buildtype=release \
			--prefix=$pkg_dir
	else
		meson configure \
			--buildtype=debug \
			--debug \
			--prefix=$pkg_dir
	fi

	meson compile
	meson install

	ln -sf $util_linux_build_dir/compile_commands.json $util_linux_src_dir
else
	echo "option build util-linux off, ignore build util-linux"
fi

# build
echo "------------------------"
echo "generate build project for examples"
cd $build_dir
cmake .. \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	-DCMAKE_PREFIX_PATH=$pkg_dir \
	-DCMAKE_INSTALL_PREFIX=$pkg_dir
ln -sf $build_dir/compile_commands.json $origin_dir/
