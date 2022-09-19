#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "###########################"
echo "# stop nginx"
echo "###########################"

./stop_nginx.sh

echo "###########################"
echo "# stop hello-service"
echo "###########################"

./stop_service.sh

echo "###########################"
echo "# clean ca"
echo "###########################"

./clean_ca.sh
