#!/bin/bash

# project root directory
origin_dir="$(dirname "$(readlink -f "$0")")"

dep_build_dir=$origin_dir/_deps/build
build_dir=$origin_dir/build
install_dir=$origin_dir/dist

rm -rf $dep_build_dir
rm -rf $build_dir
rm -rf $install_dir
