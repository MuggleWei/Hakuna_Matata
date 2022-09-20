#!/bin/bash

echo "docker kill hello-service"
sudo docker kill $(sudo docker ps -a -q --filter ancestor=hello-service:0.0.1)

echo "docker rm hello-service"
sudo docker rm $(sudo docker ps -a -q --filter ancestor=hello-service:0.0.1)

echo "docker rm image: hello-service"
sudo docker rmi $(sudo docker images  -a -q hello-service)
