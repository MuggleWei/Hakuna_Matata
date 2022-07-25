#!/bin/bash

origin_dir="$(dirname "$(readlink -f "$0")")"

clear_protobuf=1

# clean package
echo "--------------------------------"
echo "clean package directory"

pkg_dir=$origin_dir/build/pkg

echo "remove dir: $pkg_dir"
rm -rf $pkg_dir

# clean protobuf
echo "--------------------------------"
if [ $clear_protobuf -eq 1 ]; then
	echo "clean protobuf directory"

	protobuf_build_dir=$origin_dir/protobuf/build

	echo "remove dir: $protobuf_build_dir"
	rm -rf $protobuf_build_dir
else
	echo "clear_protobuf off, ignore clear protobuf"
fi

# clean cpp example
echo "--------------------------------"
echo "clean cpp example directory"

cpp_build_dir=$origin_dir/cpp/build

echo "remove dir: $cpp_build_dir"
rm -rf $cpp_build_dir

# clean java example
echo "--------------------------------"
echo "clean java example directory"

java_dir=$origin_dir/java
java_proto_dir=$java_dir/example/src/main/java/gen/proto
java_desc_dir=$java_dir/example/src/main/resources/proto/desc

echo "maven clean"
cd $java_dir
mvn clean
cd $origin_dir

echo "remove dir: $java_proto_dir"
rm -rf $java_proto_dir

echo "remove dir: $java_desc_dir"
rm -rf $java_desc_dir
