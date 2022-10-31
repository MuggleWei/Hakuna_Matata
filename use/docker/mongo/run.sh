#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "---------------------------"
echo "- run mongo"
echo "---------------------------"

sudo docker compose up -d
