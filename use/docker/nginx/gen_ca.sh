#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir/conf.d

# gen root CA
openssl req -x509 \
	-newkey rsa:2048 \
	-nodes \
	-days 365 \
	-subj "/CN=demo.mugglewei.com/C=CN/L=SH" \
	-keyout rootCA.key -out rootCA.crt

# generate server private key
openssl genrsa -out server.key 2048

# generate server csr
openssl req \
	-new \
	-config $origin_dir/cert/csr.conf \
	-key server.key \
	-out server.csr

# generate server cert
openssl x509 -req \
	-in server.csr \
	-CA rootCA.crt -CAkey rootCA.key \
	-CAcreateserial \
	-days 365 \
	-sha256 \
	-extfile $origin_dir/cert/cert.conf \
	-out server.crt
