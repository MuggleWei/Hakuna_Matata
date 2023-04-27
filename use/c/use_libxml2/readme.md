# 使用 libxml2
本工程中的所有例子均来源于 https://gnome.pages.gitlab.gnome.org/libxml2/tutorial/index.html

## 编译
进入工程根目录, 执行
```
.bootstrap.sh
cd build
make
```

## 例子
* case1: 读取文件, 逐级寻找获取 keyword 的值
* case2: 读取文件, 使用 XPath 寻找 keyword 的值
* case3: 读取文件, 找到 story/storyinfo, 向其中追加一个 Element
* case4: 读取文件, 找到 story, 向其中追加一个 Element, 并增加 attribute
* case5: 读取 case4 生成的文件, 找到 reference, 并读取其中的 uri 值
