#!/bin/bash

dir="res/proto"

cpp_outdir="gen/cpp/proto"
java_outdir="gen/java/proto"
desc_outdir="gen/proto/desc"

mkdir -p $cpp_outdir
mkdir -p $java_outdir
mkdir -p $desc_outdir

filelist=""

for file in $dir/*.proto; do
	echo generate $file
	protoc --proto_path=$dir --cpp_out=$cpp_outdir $file
	protoc --proto_path=$dir --java_out=$java_outdir $file
	filelist="$file $filelist"
done

protoc --proto_path=$dir --descriptor_set_out=$desc_outdir/proto.desc $filelist

# move into cpp example project
cpp_targetdir="cpp/build/gen/proto"
if [ -d $cpp_targetdir ]; then
	rm -rf $cpp_targetdir
fi
mkdir -p $cpp_targetdir
mv $cpp_outdir/* $cpp_targetdir

# move into java example project
java_targetdir=java/src/main/java/gen/proto
if [ -d $java_targetdir ]; then
	rm -rf $java_targetdir
fi
mkdir -p $java_targetdir
mv $java_outdir/gen/proto/* $java_targetdir

# java need descriptor to generate message dynamiclly
java_descdir=java/src/main/resources/proto/desc
if [ -d $java_descdir ]; then
	rm -rf $java_descdir
fi
mkdir -p $java_descdir
mv $desc_outdir/* $java_descdir


rm -rf gen/
