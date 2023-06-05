# 使用 OpenSSL

## 构建
```
hpb build -c build.yml

# generate rsa key for example
./gen_keys.sh
```

## 使用示例 - libcrypto

### bignum
[bignum](./example/bignum/bignum.c): 展示了使用 OpenSSL 中的大整型运算

### crypt01_digest
[crypt01_digest](./example/crypt01_digest/digest.c) 消息摘要操作

### crypt02_cipher
[crypt02_cipher](./example/crypt02_cipher/cipher.c) 以 DES-EDE3-CBC，AES-128-CBC 和 AES-256-CBC 为例进行加解密

### crypt03_rsa_enc_dec
[crypt03_rsa_enc_dec](./example/crypt03_rsa_enc_dec/rsa_enc_dec.c)  
运行此例子, 需要先使用 gen_keys.sh 已经成功生成 alice 和 bob 的密钥  
此例子模拟 alice 和 bob 通讯中的加密/解密步骤，由 bob 发送消息，使用 alice 的公钥加密，接着使用 alice 的私钥解密

### crypt04_rsa_sign
[crypt04_rsa_sign](./example/crypt04_rsa_sign/rsa_sign.c)  
运行此例子, 需要先使用 gen_keys.sh 已经成功生成 alice 和 bob 的密钥  
此例子模拟 alice 和 bob 通讯中的签名和验证步骤，由 bob 发送消息，使用 bob 的私钥加密，接着使用 bob 的公钥验证签名

### crypt05_hmac
[crypt05_hmac](./example/crypt05_hmac/hmac.c) 使用 HMAC SHA256 生成消息认证码
