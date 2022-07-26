#!/bin/bash

# handle argv
if [ "$#" -lt 1 ]; then
	echo "[WARNING] Usage: build.sh <Debug|Release|RelWithDebInfo|MinSizeRel>"
	echo "[INFO] input without build type, use Release by default"
	BUILD_TYPE=Release
else
	BUILD_TYPE=$1
fi

echo "Build Type: $BUILD_TYPE"

build_protobuf=1
build_source=1
build_cpp=1
build_java=1

# script directory
origin_dir="$(dirname "$(readlink -f "$0")")"

pkg_dir=$origin_dir/build/pkg
protobuf_dir=$origin_dir/protobuf
protoc=$pkg_dir/bin/protoc

res_dir=$origin_dir/res/proto

# create package directory
mkdir -p $pkg_dir

# build protobuf
echo "------------------------"
if [ $build_protobuf -eq 1 ]; then
	echo "build protobuf"
	mkdir -p $protobuf_dir/build
	cd $protobuf_dir/build
	cmake .. \
		-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
		-DBUILD_SHARED_LIBS=ON \
		-Dprotobuf_BUILD_SHARED_LIBS=ON \
		-Dprotobuf_BUILD_TESTS=OFF \
		-DCMAKE_INSTALL_PREFIX=$pkg_dir
	make && make install
else
	echo "build protobuf off, ignore protobuf"
fi

# build cpp
echo "------------------------"
if [ $build_cpp -eq 1 ]; then
	echo "build cpp"
	
	cd $origin_dir
	
	cpp_dir=$origin_dir/cpp
	cpp_build_dir=$cpp_dir/build
	cpp_proto_dir=$cpp_build_dir/gen/proto
	
	if [ -d $cpp_proto_dir ]; then
		rm -rf $cpp_proto_dir 
	fi
	mkdir -p $cpp_proto_dir
	
	echo "------------------------"
	echo "generate cpp protobuf"
	
	filelist=""
	for file in $res_dir/*.proto; do
		echo "generate cpp proto $file -> $cpp_proto_dir"
		$protoc --proto_path=$res_dir --cpp_out=$cpp_proto_dir $file
		filelist="$file $filelist"
	done
	
	echo "------------------------"
	if [ $build_source -eq 1 ]; then
		echo "build cpp project"

		mkdir -p $cpp_build_dir
		cd $cpp_build_dir
		cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_PREFIX_PATH=$pkg_dir -DCMAKE_INSTALL_PREFIX=$pkg_dir
		make && make install
	else
		echo "build source off, ignore build cpp source"
	fi
else
	echo "build cpp off, ignore cpp"
fi

# build java
echo "------------------------"
if [ $build_java -eq 1 ]; then
	echo "build java"
	
	cd $origin_dir
	
	java_dir=$origin_dir/java
	java_proto_dir=$java_dir/example/src/main/java
	java_desc_dir=$java_dir/example/src/main/resources/proto/desc
	java_proto_gen_dir=$java_proto_dir/gen
	
	if [ -d $java_proto_gen_dir ]; then
		rm -rf $java_proto_gen_dir
	fi
	mkdir -p $java_proto_gen_dir

	if [ -d $java_desc_dir ]; then
		rm -rf $java_desc_dir
	fi
	mkdir -p $java_desc_dir
	
	echo "------------------------"
	echo "generate java protobuf"
	
	filelist=""
	for file in $res_dir/*.proto; do
		echo "generate java proto $file -> $java_proto_dir"
		$protoc --proto_path=$res_dir --java_out=$java_proto_dir $file
		filelist="$file $filelist"
	done
	$protoc --proto_path=$res_dir --descriptor_set_out=$java_desc_dir/proto.desc $filelist

	echo "------------------------"
	if [ $build_source -eq 1 ]; then
		echo "build java project"

		cd $java_dir
		# mvn clean package -Dmaven.test.skip=true
		mvn clean package

		cp $java_dir/example/target/example-0.0.1-jar-with-dependencies.jar $pkg_dir
	else
		echo "build source off, ignore build java source"
	fi
else
	echo "build java off, ignore java"
fi

# copy run scripts
cd $origin_dir
cp ./run_local_example.sh $pkg_dir

# back to origin directory
cd $origin_dir
