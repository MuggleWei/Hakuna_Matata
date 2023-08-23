# 使用 OpenSSL

## 编译依赖库
### *nix
```
hpb build -m task -c modules/zlib.yml
hpb build -m task -c modules/openssl.unix.yml
hpb build -m task -c modules/mugglec.yml
```

### Windows
* 安装 perl 和 NASM
* 进入 VS 的构建工具目录(例如: Microsoft Visual Studio/2019/BuildTools/VC/Auxiliary/Build), 执行 `vcvarall.bat <arch>`, `arch` 可选项可以为 x86, x86_amd64, x86_arm, x86_arm64, amd64, amd64_x86, amd64_arm 或者 amd64_arm64. 比如这里我选择 amd64; 若直接使用 `developer command prompt` 而不运行 `vcvarall.bat <arch>`, 则会看到类似这样的错误: `Only x64, ARM64 and IA64 supported`
* 执行命令
```
hpb build -m task -c modules/zlib.yml
hpb build -m task -c modules/openssl.win.yml
hpb build -m task -c modules/mugglec.yml
```

### 注意
原本在依赖的编译当中, openssl 编译时加入了 `zlib` 选项, 但是由于压缩可能导致 CRIME 攻击, 所以为了防止使用时忘记关闭, 直接在编译时就不链接 `zlib` 了, 如果要开启, 可以用被注释掉的部分替换当前的编译选项.  
如果编译链接了 `zlib`, 也可以在运行的时候关闭压缩选项:
```
long flags = 0;
flags = SSL_CTX_get_options(ctx);
flags |= SSL_OP_NO_COMPRESSION;
SSL_CTX_set_options(ctx, flags);
```

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

### crypt06_totp
[crypt06_totp](./example/crypt06_totp/totp.c) 一个简单的 TOTP(Time-based One-time Password) 生成器
