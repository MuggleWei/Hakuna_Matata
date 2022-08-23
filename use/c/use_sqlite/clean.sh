#!/bin/bash

clean_sqlite=1

origin_dir="$(dirname "$(readlink -f "$0")")"

pkg_dir=$origin_dir/build/pkg
sqlite_dir=$origin_dir/sqlite

echo "------------------------"
if [ $clean_sqlite -eq 1 ]; then
	echo "clean sqlite"

	rm -rf $sqlite_dir/build/sqlite-build
else
	echo "clean sqlite off, ignore clean sqlite"
fi

echo "------------------------"
echo "clean c examples"

c_build_dir=$origin_dir/c/build
rm -rf $c_build_dir

echo "------------------------"
echo "clean package directory"

if [ -d $pkg_dir ]; then
	rm -rf $pkg_dir
fi
