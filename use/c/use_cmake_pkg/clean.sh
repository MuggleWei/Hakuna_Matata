#!/bin/bash

origin_dir="$(dirname "$(readlink -f "$0")")"
install_dir=$origin_dir/build/usr
pkg_dir=$origin_dir/build/pkg

foo_dir=$origin_dir/foo
foo_build_dir=$foo_dir/build

bar_dir=$origin_dir/bar
bar_build_dir=$bar_dir/build

baz_dir=$origin_dir/baz
baz_build_dir=$baz_dir/build

bob_dir=$origin_dir/bob
bob_build_dir=$bob_dir/build

rm -rf $install_dir
rm -rf $pkg_dir
rm -rf $foo_build_dir
rm -rf $bar_build_dir
rm -rf $baz_build_dir
rm -rf $bob_build_dir
