# 测试大页效果
这里分别测试 **HUGE_TLB**(标准大页) 和 **THP**(透明大页) 的在随即访问和顺序访问的效果

## 测试用例
**关闭 HUGETLB, 关闭 THP**  
```
sudo su
echo "never" > /sys/kernel/mm/transparent_hugepage/enabled
sysctl -w vm.nr_hugepages=0
./bin/hugepage_test -p 0 -r 0
./bin/hugepage_test -p 0 -r 1
```

**关闭 HUGETLB, 开启 THP**  
```
sudo su
echo "always" > /sys/kernel/mm/transparent_hugepage/enabled
sysctl -w vm.nr_hugepages=0
./bin/hugepage_test -p 0 -r 0
./bin/hugepage_test -p 0 -r 1
```

**开启 HUGETLB(mmap + MAP_HUGETLB), 关闭 THP**  
```
sudo su
echo "never" > /sys/kernel/mm/transparent_hugepage/enabled
sysctl -w vm.nr_hugepages=1024

# show hugepage config info
cat /proc/meminfo | grep -i huge
sysctl -a | grep -i huge

./bin/hugepage_test -p 1 -r 0
./bin/hugepage_test -p 1 -r 1
```

**注意**: 还可以通过入参 `-s` 指定使用的 `HugepageSize`, 如果想使用 1GB 大小的大页(大部分 linux 发行版默认是 2MB 大页), 需要更改 grub 选项
1. 更改 `/etc/default/grub`, 更改 `GRUB_CMDLINE_LINUX_DEFAULT`
```
GRUB_CMDLINE_LINUX_DEFAULT=default_hugepagesz=2MB hugepagesz=1G hugepages=2 hugepagesz=2M hugepages=1024
```
其中
* `default_hugepagesz=2MB`: 设置默认大页为 2MB
* `hugepagesz=1G hugepages=2`: 设置 2 个 1GB 的大页
* `hugepagesz=2M hugepages=1024`: 设置 1024 个 2MB 的大页

2. 更新 grub, 并重启机器
```
sudo update-grub
sudo reboot
```

若 `sudo update-grub` 出现 `command not found` 的报错, 也可以使用命令
```
sudo grub-mkconfig -o /boot/grub/grub.cfg
```

**开启 HUGETLB(hugetlbfs + mmap), 关闭 THP**
```
sudo su
echo "never" > /sys/kernel/mm/transparent_hugepage/enabled
sysctl -w vm.nr_hugepages=1024

# show hugepage config info
cat /proc/meminfo | grep -i huge
sysctl -a | grep -i huge

# get uid, replace <username> to current user name
export UID=$(id -u <username>)
export GID=$(id -g <username>)
echo "UID=$UID, GID=$GID"

# mount hugetlbfs, see: https://docs.kernel.org/admin-guide/mm/hugetlbpage.html
sudo mkdir -p /mnt/hugetlbfs_2M
sudo mount -t hugetlbfs none /mnt/hugetlbfs_2M -o uid=$UID,gid=$GID,pagesize=2M,size=2G
sudo mkdir -p /mnt/hugetlbfs_1G
sudo mount -t hugetlbfs none /mnt/hugetlbfs_1G -o uid=$UID,gid=$GID,pagesize=1G,size=2G

# show current hugetlbfs
mount | grep huge

./bin/hugepage_test -p 2 -r 0 -f /mnt/hugetlbfs_2M/test
./bin/hugepage_test -p 2 -r 1 -f /mnt/hugetlbfs_2M/test
./bin/hugepage_test -p 2 -r 0 -f /mnt/hugetlbfs_1G/test
./bin/hugepage_test -p 2 -r 1 -f /mnt/hugetlbfs_1G/test
```

**注意**: 这里使用 1G 大页有可能会失败, 这是因为系统没有预留 1G 的大页, 可以参考上一小节中的附加说明, 更改 grub 去预留 1G 大页
