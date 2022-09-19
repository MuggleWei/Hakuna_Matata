#!/bin/bash

name=hello-service

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir/$name

go build -o $name

nohup ./$name -bind=":10001" -number="1" & 
nohup ./$name -bind=":10002" -number="2" &
nohup ./$name -bind=":10003" -number="3" &
nohup ./$name -bind=":10004" -number="4" &
nohup ./$name -bind=":10005" -number="5" &
