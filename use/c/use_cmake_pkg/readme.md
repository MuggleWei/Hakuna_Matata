# 使用CMake的导出配置及打包

## foo
[foo](./foo/CMakeLists.txt) 展示了生成一个名为 `foo` 的库，并在安装时导出配置，已经打包(使用 `cpack` 命令)

## bar
[bar](./bar/CMakeLists.txt) 使用 `find_package` 来发现 `foo`，并直接使用
