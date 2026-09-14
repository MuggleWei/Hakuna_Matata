#!/bin/bash
#
origin_dir="$(dirname "$(readlink -f "$0")")"
build_dir=$origin_dir/build

cd $origin_dir

if [ ! -d $build_dir ]; then
	mkdir -p $build_dir
fi

sudo tcpdump -i lo host 127.0.0.1 and port 10102 -w $build_dir/foo.pcap
