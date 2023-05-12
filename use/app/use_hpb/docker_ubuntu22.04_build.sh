#!/bin/bash

docker build \
	--network host \
	-f Dockerfile \
	-t hpb/hpb_docker_examples:latest \
	.

docker container create --name extract hpb/hpb_docker_examples:latest
docker container cp extract:/app/dist ./
docker container rm -f extract
