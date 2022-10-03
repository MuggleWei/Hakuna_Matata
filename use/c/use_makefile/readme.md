# 使用Makefile的例子

## hello
[hello](./hello/makefiles/Makefile)是一个初级的例子, 用于展示编译一个执行文件. 直接运行目录中的`build.sh`进行编译. 在编译之前, 确保`make`和`bear`已正确安装, 其中`bear`用于生成例子的`compilation database`.  

## link_mugglec
[link_mugglec](./link_mugglec/makefiles/Makefile)是一个链接`mugglec`库的例子, 在运行例子之前, 先要确保`mugglec`库已安装, 包括动态库和静态库. 接着运行目录中的`build.sh`, 将会在`build/bin`目录中生成`hello`和`hello_standalone`, 其中`hello`是链接`mugglec`的动态库, 而`hello_standalone`使用了静态链接.  
