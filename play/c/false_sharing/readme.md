# false sharing

## 概述
此工程展示 `false sharing` 问题
* 每个例子都会生成 3 个结果, 对所要研究的问题进行对齐或间隔 0/1倍cacheline/2倍cacheline
* 例子都可以指定线程绑定的 cpu, 可以对比绑定相同和不同 numa 节点下的 cpu 的区别

## case1
此例子中, 数据不对齐/对齐1倍cacheline/对齐2倍cacheline, 接着每个线程对数据进行自增 1 的操作

## case2
此例子中, 每个线程对数据进行增加 x 的操作, 而 x 会与其中一个数据 不间隔/间隔1倍cacheline/间隔2倍cacheline
