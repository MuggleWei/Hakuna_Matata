# 符号隐藏

## 概述
在真实的项目中, 有许多需要依赖第三方库的场景, 某些第三方的动态库中又会包含其依赖的静态库信息, 但当此静态库的其余版本又被引入项目时, 便出现了一个库的多个版本同时存在的场景.  
<img src="./doc/img/hide_symbol.svg" />

此时有可能会导致许多问题, 解决思路就是隐藏间接引入的依赖库符号, 有两种解决办法
* 编译 `foo` 的静态库时, 使用 `-fvisibility=hidden`
* 在 `bar` 链接 `foo` 时, 链接选项增加 `-Wl,--exclude-libs,foo` 或 `-Wl,--exclude-libs,ALL`

## 例子
此项目展示了同时存在一个库的多个版本的场景
* baz 为可执行文件, 依赖于动态库: foo 1.1, bar 1.0 和 bob 1.0
* foo 1.1 为动态库
* bar 1.0 为动态库, 依赖于静态库 foo 1.0
* foo 1.0 为静态库, 使用 `-fvisibility=hidden` (CMake: C_VISIBILITY_PRESET hidden)
* bob 1.0 为动态库, 依赖于静态库 foo 1.2, 链接时使用 `-Wl,--exclude-libs,ALL`
* foo 1.2 为静态库

依赖关系可表示为
```
baz
├── foo.so.1.1.0
│
├── bar.so.1.0.0
│   └── foo.a (1.0.0)
│
└── bob.so.1.0.0
    └── foo.a (1.2.0)
```

编译完成后, 运行 `baz` 便可看到同时调用了多个不同版本的 `foo`. 更直观的, 可以通过 `readelf -s` 来观察符号的可见性

```
readelf -s /home/mugglewei/project/Hakuna_Matata/play/c/hide_symbol/foo_1.1/dist/lib/libfoo.so | grep hello
     7: 0000000000001110    34 FUNC    GLOBAL DEFAULT   12 foo_say_hello
    12: 0000000000001110    34 FUNC    GLOBAL DEFAULT   12 foo_say_hello

readelf -s /home/mugglewei/project/Hakuna_Matata/play/c/hide_symbol/foo_1.0/dist/lib/libfoo.a | grep hello
     5: 0000000000000000    34 FUNC    GLOBAL HIDDEN     1 foo_say_hello

readelf -s /home/mugglewei/project/Hakuna_Matata/play/c/hide_symbol/bar_1.0/dist/lib/libbar.so | grep hello
     7: 0000000000001110    44 FUNC    GLOBAL DEFAULT   12 bar_say_hello
     5: 0000000000001140    34 FUNC    LOCAL  DEFAULT   12 foo_say_hello
    16: 0000000000001110    44 FUNC    GLOBAL DEFAULT   12 bar_say_hello

readelf -s /home/mugglewei/project/Hakuna_Matata/play/c/hide_symbol/foo_1.2/dist/lib/libfoo.a | grep hello
     5: 0000000000000000    34 FUNC    GLOBAL DEFAULT    1 foo_say_hello

readelf -s /home/mugglewei/project/Hakuna_Matata/play/c/hide_symbol/bob_1.0/dist/lib/libbob.so | grep hello
     7: 0000000000001110    44 FUNC    GLOBAL DEFAULT   12 bob_say_hello
     5: 0000000000001140    34 FUNC    LOCAL  DEFAULT   12 foo_say_hello
    16: 0000000000001110    44 FUNC    GLOBAL DEFAULT   12 bob_say_hello

readelf -s /home/mugglewei/project/Hakuna_Matata/play/c/hide_symbol/baz_1.0/dist/bin/baz | grep hello
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND foo_say_hello
     6: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND bob_say_hello
     7: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND bar_say_hello
    12: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND foo_say_hello
    18: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND bob_say_hello
    23: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND bar_say_hello
```
* 在 `foo_1.0/dist/lib/libfoo.a` 中, `foo_say_hello` 对应的属性 `Bind: GLOBAL, Vis: HIDDEN`, 在被 `bar` 链接之后, 在 `bar` 当中其属性为 `Bind: LOCAL, Vis: DEFAULT`, 根据 `readelf` 的说明, 当 `Vis: DEFAULT` 时, 可见性由 `Bind` 决定, 所以此时, 外部不会通过 `bar.so` 使用到 `foo_say_hello`
* 同理, 在 `bob.so` 当中, `foo_say_hello` 可见性也与 `bar.so` 当中的一样
