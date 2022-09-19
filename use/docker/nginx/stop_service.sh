#!/bin/bash

name=hello-service

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir/$name

killall $name

rm -rf ./nohup*
go clean
