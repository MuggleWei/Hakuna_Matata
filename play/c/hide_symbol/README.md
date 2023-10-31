# 符号隐藏
此项目展示了同时存在一个库的两个版本的场景
* baz 为可执行文件, 依赖于动态库: foo 1.1 和 bar 1.0
* foo 1.1 为动态库
* bar 1.0 为动态库, 依赖于静态库 foo 1.0
* foo 1.0 为静态库

依赖关系可表示为
```
baz
├── foo.so.1.1.0
│
└── bar.so.1.0.0
    └── foo.a (1.0.0)
```

此时, 在 foo.a (1.0.0) 和 bar 1.0 中将 gcc 的可见性设置为隐藏
```
C_VISIBILITY_PRESET hidden
CXX_VISIBILITY_PRESET hidden
VISIBILITY_INLINES_HIDDEN YES
```

只显示导出 bar 1.0 中的 `bar_say_hello` 和 foo 1.1 中的 `foo_say_hello`, 而 bar 1.0 中的 `bar_say_hello` 又调用了 foo 1.0 中的 `foo_say_hello`. 所以此时运行 baz, 可以看到 `foo_say_hello` 两个不同版本的输出.  
更直观的, 可以通过 `readelf -s` 来观察符号的可见性
