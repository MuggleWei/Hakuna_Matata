#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

mkdir -p $origin_dir/build/certs
rm -rf $origin_dir/build/certs/*
cd $origin_dir/build/certs

# -------- 生成CA根证书 --------
# 生成CA私钥(.key) -> 生成CA证书请求(.csr) -> 自签名得到根证书

# Generate CA private key
echo "----------------------------------"
echo "gen CA private key"
openssl genrsa -out ca.key 2048

# Generate CA CSR
echo "----------------------------------"
echo "gen CA CSR"
openssl req -key ca.key -config $origin_dir/openssl.cnf -new -out ca.csr

# Generate CA self signed certificate
echo "----------------------------------"
echo "gen CA self-signed certificate"
openssl x509 -req \
	-days 3650 \
	-in ca.csr \
	-signkey ca.key \
	-outform pem -out ca.crt -extfile $origin_dir/v3.ext

# -------- 生成用户证书 --------
# 生成私钥(.key) -> 生成证书请求(.csr) -> 用CA根证书签名得到证书(.crt)

# Generate Server private key
echo "Gen Server private key"
openssl genrsa -out server.key 2048

# Generate Server CSR
echo "----------------------------------"
echo "Gen Server CSR"
openssl req -key server.key -config $origin_dir/openssl.cnf -new -out server.csr

# Generate Server certificate
echo "----------------------------------"
echo "Gen Server certificate"
openssl x509 -req \
	-days 3650 \
	-in server.csr \
	-CA ca.crt \
	-CAkey ca.key \
	-CAcreateserial \
	-outform pem -out server.crt -extfile $origin_dir/v3.ext
