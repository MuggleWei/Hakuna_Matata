#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

echo "###########################"
echo "# generate CA"
echo "###########################"

./gen_ca.sh

echo "###########################"
echo "# run hello-service"
echo "###########################"

./run_service.sh

echo "###########################"
echo "# run nginx"
echo "###########################"

./run_nginx.sh
