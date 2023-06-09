#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "---------------------------"
echo "- stop docker"
echo "---------------------------"

sudo docker compose -f docker-compose.mariadb.yml down
