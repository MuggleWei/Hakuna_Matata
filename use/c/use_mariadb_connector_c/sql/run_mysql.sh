#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "---------------------------"
echo "- run docker"
echo "---------------------------"

sudo docker compose -f docker-compose.mysql.yml up -d
