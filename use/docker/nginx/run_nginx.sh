#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "---------------------------"
echo "- run nginx docker image"
echo "---------------------------"

# # docker run
# sudo docker run \
# 	--name my-nginx \
# 	-p 80:80 -p 443:443 \
#  	-v $PWD/nginx.conf:/etc/nginx/nginx.conf:ro \
#  	-v $PWD/conf.d:/etc/nginx/conf.d \
# 	--log-opt max-size=512m \
# 	--log-opt max-file=3 \
# 	-d nginx:1.23.1-alpine
# 	# -d registry.docker-cn.com/library/nginx

# docker compose run
sudo docker compose up -d
