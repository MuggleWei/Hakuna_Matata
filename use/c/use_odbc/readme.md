# 使用 odbc

## 例子
参考了: https://www.easysoft.com/developer/languages/c/odbc_tutorial.html

## 准备工作

### 安装 unixodbc
运行 `install_unixodbc.sh`, 从源码编译安装 unixodbc, 并且将 `etc` 目录中的配置文件拷贝至安装目录下的 `etc` 目录中; 注意, 由于配置文件要求绝对路径, 所以这里有可能需要手动修改文件 `~/.local/opt/unixodbc/etc/odbcinst.ini`

### 安装 mysql odbc
下载 mysql Connector/ODBC 并解压, 将 `lib` 目录下的文件全部拷贝至上面的安装目录中(这里只是为了方便, 其实这步拷贝至哪里都可以, 只要对应修改一下 `odbcinst.ini` 文件即可)

### 编译
`./build.sh <debug|release>`
