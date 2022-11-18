#!/bin/bash

# cd script directory
origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

# arguments
OS_VER=22.10

# generate dev image
docker build \
	--build-arg OS_VER=$OS_VER \
	-f dockerfiles/build_base.Dockerfile \
	-t ubuntu-dev:$OS_VER \
	.
