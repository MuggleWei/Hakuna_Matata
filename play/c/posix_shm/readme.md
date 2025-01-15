## 使用 POSIX shm

### 概述与提醒
例子分为两类
1. 使用 `shm_open + mmap` 来打开共享内存, 进行进程间通讯
2. 使用 `shmget` 来打开共享内存, 进行进程间通讯

这里面有个要特别注意的地方, 使用 `shm_open + mmap` 打开的共享内存, 默认情况下内容会被自动同步至文件中, 这里有可能带来效率问题

### 应用
* shm_mmap_writer/shmget_writer: 打开共享内存(如果没有, 就建立一个), 并往里写入当前的时间戳
* shm_mmap_reader/shmget_reader: 打开共享内存(如果没有, 则失败), 从里读出 writer 写入的时间戳
* rm_shm_mmap/rm_shmget: 删除共享内存
