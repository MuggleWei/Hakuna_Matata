# 使用CMake的导出配置及打包

## foo
[foo](./foo/CMakeLists.txt) 展示了生成一个名为 `foo` 的库，并在安装时导出配置(包括了 CMake 的配置文件和 pkg-config 的文件)，并且打包(使用 `cpack` 命令)

## bar
[bar](./bar/CMakeLists.txt) 使用 `find_package` 来发现 `foo`并使用

## baz
[baz](./baz/CMakeLists.txt) 使用 `pkg_check_modules` 来发现 `foo` 并使用

## bob
[bob](./bob/meson.build) 使用 `meson` 来构建工程, 发现 `foo` 并使用

## alice
[alice](./alice/CMakeLists.txt) 本身为一个动态库, 假设 `foo` 仅有头文件和库, 没有任何导出文件, 此时 `alice` 要调用 `foo`

## Pat
[pat](./pat/CMakeLists.txt) 使用 `find_package` 来发现 `alice`, 可直接调用 `alice` 的函数; 由于 `alice` 并未向外部暴露 `foo`, 所以无需其他关于 `foo` 的配置; 若想直接调用 `foo`, 可在 CMakeLists.txt 中 `include FindFoo`, 之后则可直接调用 `foo` 的函数

## dave
[dave](./dave/CMakeLists.txt) 本身为一个动态库, 使用 `find_package` 来发现 `foo` 并使用

## peggy
[peggy](./peggy/CMakeLists.txt) 使用 `find_package` 来发现 `dave`, 可直接调用 `dave` 的函数; 由于 `dave` 并未向外部暴露 `foo`, 所以若想直接调用 `foo`, 可以:
* 在 CMakeLists.txt 中 `find_package(foo)`, 之后则可直接调用 `foo` 的函数
* 修改 `dave` 的 daveConfig.cmake.in, 在其中直接添加 `find_package(foo)`
