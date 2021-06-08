#!/bin/bash

build_repo() {
	cur_dir=$1
	repo=$2
	ver=$3
	lib_type=$4
	link_base_type=$5

	cd $cur_dir

	src=$repo@$ver
	if [[ ! -d "repos/$src" ]]
	then
		git clone localgit.com/$repo -b $ver repos/$src
	fi

	cd repos/$src
	if [[ ! -d "build" ]]
	then
		mkdir build
	fi
	cd build
	cmake .. \
		-DCMAKE_INSTALL_PREFIX=$cur_dir/output \
		-DLIB_TYPE=$lib_type \
		-DLIB_BASE_TYPE=$link_base_type
	make
	make install
}

clean() {
	cd $cur_dir

	rm -rf output
	rm -rf repos/*/build
}

# get current directory
cur_path=$(readlink -f "$0")
cur_dir=`dirname $cur_path`
echo $cur_dir

# clean
clean

lib_base_type=SHARED
libs_type=SHARED

# foo
repo=base
ver=0.0.1
build_repo $cur_dir $repo $ver $lib_base_type $lib_base_type

repo=foo
ver=0.0.1
build_repo $cur_dir $repo $ver $libs_type $lib_base_type

# bar
repo=base
ver=0.0.2
build_repo $cur_dir $repo $ver $lib_base_type $lib_base_type

repo=bar
ver=0.0.1
build_repo $cur_dir $repo $ver $libs_type $lib_base_type

# baz
repo=base
ver=1.0.0
build_repo $cur_dir $repo $ver $lib_base_type $lib_base_type

repo=baz
ver=0.0.1
build_repo $cur_dir $repo $ver $libs_type $lib_base_type

# app
repo=app
ver=0.0.1
build_repo $cur_dir $repo $ver $libs_type $lib_base_type
