#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

mkdir -p build
cp makefiles/* build/

cd build
# use bear to generate compilation database
#make
bear -- make

# gen link compilation database
cd $origin_dir
ln -sf build/compile_commands.json .
