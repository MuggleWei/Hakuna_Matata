#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "---------------------------"
echo "- run nginx docker image"
echo "---------------------------"
sudo docker run \
	--name my-nginx \
	-p 80:80 -p 443:443 \
 	-v $PWD/nginx.conf:/etc/nginx/nginx.conf:ro \
 	-v $PWD/conf.d:/etc/nginx/conf.d \
	-d nginx 
	# -d registry.docker-cn.com/library/nginx 

