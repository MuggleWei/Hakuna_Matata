#!/bin/bash

# 获取代码根目录并进入
source_dir=$(dirname $(readlink -f "$0"))
cd $source_dir

# 创建build目录并进入
rm -rf build
mkdir -p build
cd build

# 编译
gcc -I$source_dir/src -I$source_dir/tests/ --coverage $source_dir/src/foo.c $source_dir/tests/test_foo.c -lgcov -o test_foo
