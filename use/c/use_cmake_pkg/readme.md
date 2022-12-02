# 使用CMake的导出配置及打包

## foo
[foo](./foo/CMakeLists.txt) 展示了生成一个名为 `foo` 的库，并在安装时导出配置(包括了 CMake 的配置文件和 pkg-config 的文件)，并且打包(使用 `cpack` 命令)

## bar
[bar](./bar/CMakeLists.txt) 使用 `find_package` 来发现 `foo`并使用

## baz
[baz](./baz/CMakeLists.txt) 使用 `pkg_check_modules` 来发现 `foo` 并使用
