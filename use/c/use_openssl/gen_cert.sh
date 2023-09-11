#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

mkdir -p $origin_dir/build/certs
rm -rf $origin_dir/build/certs/*
cd $origin_dir/build/certs

cfg_dir=$origin_dir/cert_conf

# -------- 生成CA根证书 --------
# 生成CA私钥(.key) -> 生成CA证书请求(.csr) -> 自签名得到根证书

# Generate CA private key
echo "----------------------------------"
echo "gen CA private key"
echo "----------------------------------"
openssl genrsa -out ca.key 2048
echo ""

# Generate CA CSR
echo "----------------------------------"
echo "gen CA CSR"
echo "----------------------------------"
openssl req \
	-key ca.key \
	-config $cfg_dir/ca_csr.conf \
	-new -out ca.csr
echo ""

# Generate CA self signed certificate
echo "----------------------------------"
echo "gen CA self-signed certificate"
echo "----------------------------------"
openssl x509 -req \
	-days 3650 \
	-in ca.csr \
	-signkey ca.key \
	-outform pem -out ca.crt -extfile $cfg_dir/ca_csr.conf
echo ""

# -------- 生成用户证书 --------
# 生成私钥(.key) -> 生成证书请求(.csr) -> 用CA根证书签名得到证书(.crt)

# Generate Server private key
echo "----------------------------------"
echo "Gen Server private key"
echo "----------------------------------"
openssl genrsa -out server.key 2048
echo ""

# Generate Server CSR
echo "----------------------------------"
echo "Gen Server CSR"
echo "----------------------------------"
openssl req -key server.key -config $cfg_dir/server_csr.conf -new -out server.csr
echo ""

# Generate Server certificate
echo "----------------------------------"
echo "Gen Server certificate"
echo "----------------------------------"
openssl x509 -req \
	-days 3650 \
	-in server.csr \
	-CA ca.crt \
	-CAkey ca.key \
	-CAcreateserial \
	-outform pem -out server.crt -extfile $cfg_dir/server_csr.conf
