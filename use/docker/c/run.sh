#!/bin/bash

# cd script directory
origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

# run
sudo docker compose up -d
