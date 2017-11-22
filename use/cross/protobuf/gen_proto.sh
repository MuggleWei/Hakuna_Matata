#!/bin/bash

dir="res/proto"

cpp_outdir="gen/cpp/proto"
java_outdir="gen/java/proto"

mkdir -p $cpp_outdir
mkdir -p $java_outdir

for file in $dir/*.proto; do
	echo generate $file
	protoc --proto_path=$dir --cpp_out=$cpp_outdir $file
	protoc --proto_path=$dir --java_out=$java_outdir $file
done

# move into cpp example project
cpp_targetdir="cpp/build/gen"

mkdir -p $cpp_targetdir
mv $cpp_outdir $cpp_targetdir

rm -rf gen/
