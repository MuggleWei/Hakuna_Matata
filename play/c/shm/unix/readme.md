## 使用 *unix shm

### 概述与提醒
例子分为两类
1. 使用 `shm_open + mmap` 来打开共享内存, 进行进程间通讯
2. 使用 `shmget` 来打开共享内存, 进行进程间通讯

### 注意
1. 这里面有个要特别注意的地方, 使用 `shm_open + mmap` 打开的共享内存, 默认情况下内容会被自动同步至文件中, 这里有可能带来效率问题
2. 在计划 `shmget` 要分配的内存大小时, 记得检查一下系统是否支持此大小: `sysctl -a | grep shm`
  * kernel.shmmax: 单个共享内存段的最大值, 此值要大于我们计划分配的内存大小
  * kernel.shmall: 可以使用的共享内存总页数, linux 一般默认共享内存页大小为 4KB, 所以设此值为 x, 则 x * 4KB 应该要大于我们计划分配的内存

### 应用
* shm_mmap_writer/shmget_writer: 打开共享内存(如果没有, 就建立一个), 并往里写入当前的时间戳
* shm_mmap_reader/shmget_reader: 打开共享内存(如果没有, 则失败), 从里读出 writer 写入的时间戳
* rm_shm_mmap/rm_shmget: 删除共享内存
