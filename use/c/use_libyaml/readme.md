# 使用 libyaml

## 编译
进入工程根目录, 执行
```
.bootstrap.sh [debug|release]
cd build
make
```

## 例子
* case1: 初始化 yaml parse, 并且读入文件
* case2: 解析 yaml 文件, 按照 token 模式遍历
* case3: 解析 yaml 文件, 按照 event 模式遍历
