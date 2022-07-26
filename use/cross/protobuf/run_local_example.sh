#!/bin/bash

# run cpp
if [ -f bin/cpp_example_local ]; then
	echo ""
	echo "################################"
	echo "run cpp local example"
	echo "################################"
	export LD_LIBRARY_PATH=$PWD/lib
	./bin/cpp_example_local
fi

# run java
if [ -f example-0.0.1-jar-with-dependencies.jar ]; then
	echo ""
	echo "################################"
	echo "run java local example"
	echo "################################"
	java -cp example-0.0.1-jar-with-dependencies.jar com.muggle.example.local.Hello
fi
