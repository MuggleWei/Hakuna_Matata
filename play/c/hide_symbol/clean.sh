#!/bin/bash

# directories
origin_dir="$(dirname "$(readlink -f "$0")")"

# foo 1.0
foo_1_0_dir=$origin_dir/foo_1.0
cd $foo_1_0_dir
if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi

# bar 1.0
bar_1_0_dir=$origin_dir/bar_1.0
cd $bar_1_0_dir
if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi

# foo 1.1
foo_1_1_dir=$origin_dir/foo_1.1
cd $foo_1_1_dir
if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi

# baz 1.0
baz_1_0_dir=$origin_dir/baz_1.0
cd $baz_1_0_dir
if [ -d build ]; then
	rm -rf build
fi
if [ -d dist ]; then
	rm -rf dist
fi
