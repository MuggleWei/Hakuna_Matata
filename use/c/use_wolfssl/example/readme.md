# 例子

## 概述
wolfSSL 库的头文件被分为以下几个部分
* wolfSSL: `wolfssl/`
* wolfCrypt: `wolfssl/wolfcrypt`
* wolfSSL OpenSSL 兼容层: `wolfssl/openssl/`

所以，此工程中的例子分为了两个部分:
* example_crypt: 包含了使用对称加密，非对称加密，哈希函数等内容
* example_ssl: 包含了使用 SSL 的内容

## crypt

### example_crypt01_hash
生成消息摘要，如 MD5, SHA256, SHA512 [crypt01_hash.c](./example_crypt01_md5/use_md5.c)

## example_crypt02_hmac
HMAC 的例子 [hmac.c](./example_crypt02_hmac/hmac.c)
* 首先为用户生成 AK/SK
* 接着使用 `HMAC(SK, 消息+时间戳)` 来生成 Hash 值
* 最后打印出用户的消息结构

## example_crypt03_passwd_hash
密码 hash + 盐 的例子 [passwd_hash.c](./example_crypt03_passwd_hash/passwd_hash.c)

## ssl

## example_ssl01_echo_serv
TCP TLS 的 Echo 服务 [echo_serv.c](./example_ssl01_echo_serv/echo_serv.c)

## example_ssl02_timer_client
TCP TLS 的定时发送时间字符串的客户端, 可与 echo_serv 配合使用, 查看结果 [timer_client.c](./example_ssl02_timer_client/timer_client.c)
