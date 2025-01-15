## 使用 POSIX shm
* shm_writer: 打开共享内存(如果没有, 就建立一个), 并往里写入当前的时间戳
* shm_reader: 打开共享内存(如果没有, 则失败), 从里读出 writer 写入的时间戳
* rm_shm: 删除共享内存
