#!/bin/bash

origin_dir="$(dirname "$(readlink -f "$0")")"
deps_dir=$origin_dir/_deps
install_dir=~/.local/opt/unixodbc

mkdir -p $deps_dir
cd $deps_dir

if [ -d unixODBC ]; then
	echo "unixODBC already exists"
else
	echo "download unixODBC"
	git clone https://github.com/lurcher/unixODBC.git --depth 1 --branch 2.3.12
fi

cd unixODBC
autoreconf -fi
./configure --enable-gui=no --prefix=$install_dir --exec_prefix=$install_dir
make
make install

cd $origin_dir
cp -r etc/* $install_dir/etc/
