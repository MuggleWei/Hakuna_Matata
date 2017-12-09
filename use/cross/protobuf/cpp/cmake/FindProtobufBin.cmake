set(PROTOBUF_PATH CACHE PATH "The directory of protobuf")
set(PROTOBUF_SEARCH_PATH /usr/local ${PROTOBUF_PATH})

FIND_PATH(PROTOBUF_INCLUDE_DIR
	NAMES google/protobuf/message.h
	PATH_SUFFIXES include
	PATHS ${PROTOBUF_SEARCH_PATH})
	
FIND_LIBRARY(PROTOBUF_LIB_DIR
	NAMES protoc
	PATH_SUFFIXES lib
	PATHS ${PROTOBUF_SEARCH_PATH})
	
FIND_PATH(PROTOBUF_BIN_DIR
	NAMES protoc
	PATH_SUFFIXES bin
	PATHS ${PROTOBUF_SEARCH_PATH})
	
INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ProtobufBin DEFAULT_MSG
    PROTOBUF_INCLUDE_DIR
	PROTOBUF_LIB_DIR
	PROTOBUF_BIN_DIR)