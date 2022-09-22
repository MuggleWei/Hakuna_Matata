#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "---------------------------"
echo "- stop docker"
echo "---------------------------"

sudo docker compose stop
sudo docker compose rm -f

echo "---------------------------"
echo "- clean ca"
echo "---------------------------"

./clean_ca.sh
