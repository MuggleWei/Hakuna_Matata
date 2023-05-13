# 使用 MbedTLS

## 构建
```
hpb build -m task -c mbedtls.yml
hpb build -c build.yml
```

## PSA 例子
* [PSA Example01 - import_key.c](../psa_example/psa_example01_import_key/import_key.c): 导入一个 AES Key
* [PAS Example02 - hash_msg.c](../psa_example/psa_example02_hash_msg/hash_msg.c): 计算消息的 SHA256 哈希值, 并以十六进制格式输出
