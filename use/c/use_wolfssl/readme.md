# 使用wolfssl
本项目展示了wolfssl的tls用法

## 构建项目
先运行gen_cert.sh生成根证书、私钥和服务器的私钥、证书，再运行build脚本生成项目构建文件，接着去编译项目即可

## 例子简介
此项目中包含一个echo服务和一个发送时间的客户端，都使用wolfssl TLS对通信进行加密