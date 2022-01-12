#!/bin/bash

# 获取代码根目录并进入
source_dir=$(dirname $(readlink -f "$0"))
cd $source_dir

# 进入build目录
cd build

# 运行测试
./test_foo

# 使用gcov, 生成代码覆盖率报告
gcov -s $source_dir/src foo.c

# 使用lcov, 图形化gcov输出带结果
lcov -c -d . -o coverage.info
genhtml coverage.info -o coverage
