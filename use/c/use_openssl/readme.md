# 使用 OpenSSL

## 构建
```
hpb build -c build.yml

# generate rsa key for example
./gen_keys.sh
```

## 使用示例 - libcrypto
### crypt01_digest
[crypt01_digest](./example/crypt01_digest/digest.c) 消息摘要操作

### crypt02_cipher
[crypt02_cipher](./example/crypt02_cipher/cipher.c) 以 DES-EDE3-CBC，AES-128-CBC 和 AES-256-CBC 为例进行加解密

### crypt03_rsa
[crypt03_rsa](./example/crypt03_rsa/rsa.c)  
此例子模拟 alice 和 bob 通讯，确保 gen_keys.sh 已经成功生成 alice 和 bob 的密钥  
由 bob 发送消息，使用 alice 的公钥加密，接着使用 alice 的私钥解密

## 使用示例
* [bignum](./example/bignum/bignum.c): 展示了使用 OpenSSL 中的大整型运算
