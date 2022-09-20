#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir

sudo docker image build -t hello-service:0.0.1 .
