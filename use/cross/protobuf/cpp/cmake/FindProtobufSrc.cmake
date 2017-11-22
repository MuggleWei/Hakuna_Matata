if (NOT PROTOBUF_SEARCH_PATH)
	SET(PROTOBUF_SEARCH_PATH "${CMAKE_CURRENT_LIST_DIR}/../thirdparty/google/protobuf")
endif()

FIND_PATH(PROTOBUF_SOURCE_DIR
    NAMES src/google/protobuf/message.cc
    PATHS ${PROTOBUF_SEARCH_PATH})
	
FIND_PATH(PROTOBUF_INCLUDE_DIR
    NAMES google/protobuf/message.h
	PATH_SUFFIXES src
    PATHS ${PROTOBUF_SEARCH_PATH})
	
INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ProtobufSrc DEFAULT_MSG
    PROTOBUF_SOURCE_DIR
	PROTOBUF_INCLUDE_DIR)