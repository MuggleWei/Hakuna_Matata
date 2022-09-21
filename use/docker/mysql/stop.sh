#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "---------------------------"
echo "- stop mysql"
echo "---------------------------"

sudo docker compose stop
sudo docker compose rm -f
