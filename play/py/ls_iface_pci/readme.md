# 显示 ip、网卡以及插槽的信息

## 概述
通常情况下，使用 ifconfig 可以列出当前的 ip 情况，但是却无法将其与当前机器上的网卡之间对应起来，此工程的目的是为了一次性的显示 ip、网卡以及插槽的信息; 即将 "IP <---> 逻辑接口名 <---> 内核驱动 <---> PCI 设备 <---> 厂商/型号" 一次性串起来

## 相关知识

1. **查看网络接口以及ip**
```
ifconfig
```

可以得到类似结果
```
ens3f0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 192.168.1.36  netmask 255.255.255.0  broadcast 192.168.1.255
        ether 00:0f:53:b2:0d:d0  txqueuelen 1000  (Ethernet)
        RX packets 27705757  bytes 18319249942 (17.0 GiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 27600035  bytes 18312486181 (17.0 GiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
        device interrupt 16  

ens3f1: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
        ether 00:0f:53:b2:0d:d1  txqueuelen 1000  (Ethernet)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
        device interrupt 17
```

2. **对每个接口, 查看它的总线信息**
```
ethtool -i <dev_name>

e.g.
ethtool -i ens3f0
```

可以得到类似的结果
```
driver: sfc
version: 5.3.16.1004
firmware-version: 8.5.0.1002 rx1 tx1
expansion-rom-version: 
bus-info: 0000:43:00.0
supports-statistics: yes
supports-test: yes
supports-eeprom-access: no
supports-register-dump: yes
supports-priv-flags: yes
```

其中的 `bus-info` 的值的意义为 `<PCI域名>:<总线>:<设备>.<功能>`

3. **查询 pci 信息**
拿上一步得到的 `bus-info`，查询对应的 pci 信息
```
lspci -nn -s <bus-info>

e.g.
lspci -nn -s 0000:43:00.0
```

到了这一步，已经做到了 "IP <---> 逻辑接口名 <---> 内核驱动 <---> PCI 设备 <---> 厂商/型号" 串起来这一目的  

如果想要显示更详细的信息, 可以使用
```
lspci -vv -s 0000:43:00.0
```
其中的 `LnkCap:` 当中可以显示该 PCIe 设备支持的最大链路能力(如 2.5/5/8/16/32GT/s) 和最大通道宽度(如 x1/x4/x8/x16)


4. **查看主板上的物理 PCIe 插槽**
使用 dmidecode 查看主板插槽信息
```
sudo dmidecode -t slot
```

可以得到如下类似信息
```
Handle 0x00BC, DMI type 9, 17 bytes
System Slot Information
        Designation: Slot 3
        Type: x8 PCI Express 4 x8
        Current Usage: In Use
        Length: Short
        ID: 3
        Characteristics:
                3.3 V is provided
                Opening is shared
                PME signal is supported
        Bus Address: 0000:43:00.0
```
其中
* Designation: 表示插槽标签 (主板丝印)
* Type: 类型，可以通过此来判断是否能插网卡 (x1/x4/x8/x16 都行)
* Current Usage: `Available` 表示空闲, `In Use` 表示已插卡
* Bus Address: 可以和之前的 pci 信息对应起来
