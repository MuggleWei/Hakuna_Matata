cmake_minimum_required (VERSION 3.0.2)
project(proto_cpp)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# find protobuf
message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindProtobufBin.cmake)
if (ProtobufBin_FOUND)
	message("protobuf include dir: ${PROTOBUF_INCLUDE_DIR}")
	message("protobuf libs: ${PROTOBUF_LIBS}")
else()
	message(FATAL_ERROR "Can't find protobuf binary")
endif()

# gen proto lib
set(gen_proto protomsg)
file(GLOB_RECURSE proto_h_files ${CMAKE_BINARY_DIR}/gen/proto/*.h)
file(GLOB_RECURSE proto_cc_files ${CMAKE_BINARY_DIR}/gen/proto/*.cc)
add_library(${gen_proto} STATIC
	${proto_h_files}
	${proto_cc_files}
)
set_target_properties(${gen_proto} PROPERTIES 
	DEBUG_POSTFIX d
)
target_include_directories(${gen_proto}
	PUBLIC ${CMAKE_BINARY_DIR} ${PROTOBUF_INCLUDE_DIR})
target_link_libraries(${gen_proto}
	PUBLIC ${PROTOBUF_LIBS})
if (WIN32)
	target_compile_definitions(${gen_proto}
		PUBLIC PROTOBUF_USE_DLLS)
endif()

# example
set(hello proto_cpp_example)
file(GLOB_RECURSE h_files ${CMAKE_CURRENT_LIST_DIR}/example/hello/*.h)
file(GLOB_RECURSE cpp_files ${CMAKE_CURRENT_LIST_DIR}/example/hello/*.cpp)
if (WIN32)
	add_executable(${hello}
		${h_files}
		${cpp_files}
	)
	
	set_target_properties(${hello} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
else()
	add_executable(${hello}
		${cpp_files}
	)
endif()
add_dependencies(${hello} ${gen_proto})
target_include_directories(${hello} PUBLIC ${PROJECT_BINARY_DIR})
target_link_libraries(${hello} PUBLIC ${gen_proto})

# install
install(TARGETS ${gen_proto} ${hello}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static
)

