#!/bin/bash

# cd script directory
origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

# stop
sudo docker compose down
