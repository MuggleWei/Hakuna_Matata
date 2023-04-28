#!/bin/bash

# dependencies version
unity_repo="https://github.com/ThrowTheSwitch/Unity.git"
unity_ver="v2.5.2"

# directories
origin_dir="$(dirname "$(readlink -f "$0")")"
build_dir=$origin_dir/build
tmp_dep_dir=$build_dir/_download_deps
dep_dir=$origin_dir/_deps

if [ -d $dep_dir ]; then
	echo "Dependencies source directory already exists: $dep_dir"
	exit 0
fi

# create tmp dependencies source directory
if [ -d $tmp_dep_dir ]; then
	rm -rf $tmp_dep_dir
fi
mkdir -p $tmp_dep_dir
cd $tmp_dep_dir

# download unity test
git clone --branch $unity_ver --depth 1 $unity_repo unity
status=$?
if [ $status -eq 0 ]; then
	echo "Success download unity"
else
	echo "Failed download unity"
	exit 1
fi

# move tmp deps to deps
cd $origin
mv $tmp_dep_dir $dep_dir
echo "move $tmp_dep_dir -> $dep_dir"
