if (WIN32)
	cmake_minimum_required(VERSION 3.8.2)
else()
	cmake_minimum_required(VERSION 3.0.2)
endif()
project(use_protobuf)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# protobuf
if (WIN32)
	set(USE_PROTOBUF_WITH_SOURCE ON CACHE BOOL "use protobuf with source, if it OFF, try to find protobuf binary in system")
else()
	set(USE_PROTOBUF_WITH_SOURCE OFF CACHE BOOL "use protobuf with source, if it OFF, try to find protobuf binary in system")
endif()

if (USE_PROTOBUF_WITH_SOURCE)

	include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindProtobufSrc.cmake)
	if (ProtobufSrc_FOUND)
		SET(protobuf_BUILD_TESTS OFF CACHE BOOL "protobuf test")
		SET(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "protobuf test")
		SET(protobuf_BUILD_SHARED_LIBS ON CACHE BOOL "protobuf build shared")
		
		add_subdirectory(${PROTOBUF_SOURCE_DIR}/cmake)
		include_directories(${PROTOBUF_INCLUDE_DIR})
		
		if (WIN32)
		set_target_properties(libprotobuf libprotobuf-lite libprotoc protoc js_embed
			PROPERTIES
			FOLDER "protobuf"
		)
		endif()
		
	else()
		message(FATAL_ERROR "Can't find protobuf source")
	endif()
else()

	include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindProtobufBin.cmake)
	if (ProtobufBin_FOUND)
		include_directories(${PROTOBUF_INCLUDE_DIR})
		link_directories(${PROTOBUF_LIB_DIR})
	else()
		message(FATAL_ERROR "Can't find protobuf binary")
	endif()
endif()


# examples
file(GLOB_RECURSE h_files ${CMAKE_CURRENT_LIST_DIR}/example/hello/*.h)
file(GLOB_RECURSE cpp_files ${CMAKE_CURRENT_LIST_DIR}/example/hello/*.cpp)
file(GLOB_RECURSE proto_h_files ${PROJECT_BINARY_DIR}/gen/proto/*.h)
file(GLOB_RECURSE proto_cc_files ${PROJECT_BINARY_DIR}/gen/proto/*.cc)
if (WIN32)
	add_executable(hello
		${h_files}
		${proto_h_files}
		${cpp_files}
		${proto_cc_files}
	)
	if (proto_h_files)
		source_group(gen\\proto FILES ${proto_h_files} ${proto_cc_files})	
	else() 
		message(AUTHOR_WARNING
			"Can't find protobuf gen cpp source file, "
			"generate source with script cross/protobuf/gen_proto.*"
		)
	endif()
	
	set_target_properties(hello PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
else()
	add_executable(hello
		${cpp_files}
		${proto_cc_files}
	)
endif()
target_include_directories(hello PUBLIC
	${PROJECT_BINARY_DIR}
)
if (USE_PROTOBUF_WITH_SOURCE)
	target_link_libraries(hello
		libprotobuf
	)
	add_dependencies(hello
		libprotobuf
	)
else()
	target_link_libraries(hello
		protobuf
	)
endif()
