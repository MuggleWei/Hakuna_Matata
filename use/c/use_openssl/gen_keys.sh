#!/bin/bash

origin_dir="$(dirname "$(readlink -f "$0")")"
cd $origin_dir

mkdir -p $origin_dir/build/keys
rm -rf $origin_dir/build/keys/*
cd $origin_dir/build/keys

openssl genrsa -out alice.private 2048
openssl rsa -in alice.private -out alice.public -pubout -outform PEM

openssl genrsa -out bob.private 2048
openssl rsa -in bob.private -out bob.public -pubout -outform PEM
