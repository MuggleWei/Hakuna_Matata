#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "---------------------------"
echo "- run mysql"
echo "---------------------------"

sudo docker compose up -d
