#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")

echo "---------------------------"
echo "- build hello-service docker image"
echo "---------------------------"
$origin_dir/hello-service/docker_gen_image.sh

echo "---------------------------"
echo "- run hello-service docker image"
echo "---------------------------"
sudo docker container run -p 10001:8080 -e service_number=1 -d hello-service:0.0.1
sudo docker container run -p 10002:8080 -e service_number=2 -d hello-service:0.0.1
sudo docker container run -p 10003:8080 -e service_number=3 -d hello-service:0.0.1
sudo docker container run -p 10004:8080 -e service_number=4 -d hello-service:0.0.1
sudo docker container run -p 10005:8080 -e service_number=5 -d hello-service:0.0.1
