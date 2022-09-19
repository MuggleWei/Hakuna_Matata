#!/bin/bash

origin_dir=$(readlink -f "$(dirname "$0")")
cd $origin_dir/conf.d

rm -rf rootCA.* server.*
