cmake_minimum_required (VERSION 3.0.2)
project(build_protobuf)

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

# set deps dir
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

###############################
# protobuf
configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/protobuf.cmake.in
		${FETCHCONTENT_BASE_DIR}/protobuf-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/protobuf-download)
if (result)
        message(FATAL_ERROR "cmake step for protobuf failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
		WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/protobuf-download)
if (result)
        message(FATAL_ERROR "build step for protobuf failed: ${result}")
endif()

set(protobuf_BUILD_TESTS OFF)

# add protobuf to build
add_subdirectory(
	${FETCHCONTENT_BASE_DIR}/protobuf-src
    ${FETCHCONTENT_BASE_DIR}/protobuf-build)
