#!/bin/bash

# dependencies version
libxml2_repo="https://github.com/GNOME/libxml2.git"
libxml2_ver="v2.10.4"

mugglec_repo="https://github.com/MuggleWei/mugglec.git"
mugglec_ver="v1.0.2"

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

git clone --branch $libxml2_ver --depth 1 $libxml2_repo libxml2
status=$?
if [ $status -eq 0 ]; then
	echo "Success download libxml2"
else
	echo "Failed download libxml2"
	exit 1
fi

git clone --branch $mugglec_ver --depth 1 $mugglec_repo mugglec
status=$?
if [ $status -eq 0 ]; then
	echo "Success download mugglec"
else
	echo "Failed download mugglec"
	exit 1
fi

# move tmp deps to deps
cd $origin
mv $tmp_dep_dir $dep_dir
echo "move $tmp_dep_dir -> $dep_dir"
