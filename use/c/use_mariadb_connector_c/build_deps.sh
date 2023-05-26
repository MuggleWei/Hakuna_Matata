#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[ERROR] build.sh without build type"
	echo "[ERROR] Usage: build.sh <Debug|Release>"
	exit 1
else
	# to lowercase
	BUILD_TYPE=$(echo $1 | tr '[:upper:]' '[:lower:]')
fi

origin_dir="$(dirname "$(readlink -f "$0")")"

cd $origin_dir

hpb build -m task -c $origin_dir/modules/mugglec.yml -p build_type=$BUILD_TYPE
hpb build -m task -c $origin_dir/modules/zlib.yml -p build_type=$BUILD_TYPE
hpb build -m task -c $origin_dir/modules/openssl.yml -p build_type=$BUILD_TYPE
hpb build -m task -c $origin_dir/modules/mariadb_connector_c.yml -p build_type=$BUILD_TYPE
