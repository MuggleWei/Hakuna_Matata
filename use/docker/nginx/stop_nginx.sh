#!/bin/bash

echo "---------------------------"
echo "- stop & rm nginx"
echo "---------------------------"

# # docker stop & rm
# sudo docker stop my-nginx
# sudo docker rm my-nginx

# docker compose stop & rm
sudo docker compose stop
sudo docker compose rm -f
