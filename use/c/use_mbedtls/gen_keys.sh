#!/bin/bash

origin_dir="$(dirname "$(readlink -f "$0")")"
cd $origin_dir

build_dir=$origin_dir/build
mbedtls_dir=$build_dir/_hpb/deps

if [ ! -d $mbedtls_dir ]; then
	echo "[ERROR] mbedtls directory not found: $mbedtls_dir"
	exit 1
fi

gen_key=$mbedtls_dir/bin/gen_key

etc_dir=$build_dir/etc/
if [ ! -d $etc_dir ]; then
	mkdir -p $etc_dir
fi

cd $etc_dir
export LD_LIBRARY_PATH=$mbedtls_dir/lib
$gen_key type=rsa rsa_keysize=1024 filename=rsa_private.key format=pem
