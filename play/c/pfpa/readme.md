# PFPA

## 概述
PFPA(Pcap File Parsing Assistant) **pcap文件解析助手**，方便直接针对业务场景直接写回调函数; 当前暂时忽略了 TCP 包乱序与重传的处理

## 例子

### hello
对任意的抓包，打印 TCP 和 UDP 信息

### str
* 使用 [mugglec](https://github.com/MuggleWei/mugglec) 当中的例子 [echo_server](https://github.com/MuggleWei/mugglec/tree/master/examples/src/network/echo_serv) 和 [str_client](https://github.com/MuggleWei/mugglec/tree/master/examples/src/network/str_client), 进行 TCP 和 UDP 的内容传输
* 对上面的例子进行抓包, 例如: `sudo tcpdump -i enp0s3 port 10102 -w client.cap`
* 使用 `str -i client.cap -o client.txt` 生成内容

### tgw
解析深交所的包头
