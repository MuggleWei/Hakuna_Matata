cmake_minimum_required(VERSION 3.0.2)
project(use_gflags)

# print compiler
message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# set standard and print features
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set FetchContent base dir
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

# gflags
message("-- prepare thirdparty: gflags")
configure_file(
	${CMAKE_CURRENT_LIST_DIR}/cmake/gflags.cmake.in
	${FETCHCONTENT_BASE_DIR}/gflags-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/gflags-download)
if (result)
	message(FATAL_ERROR "cmake step for gflags failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/gflags-download)
if (result)
	message(FATAL_ERROR "build step for gflags failed: ${result}")
endif()

add_subdirectory(${FETCHCONTENT_BASE_DIR}/gflags-src
	${FETCHCONTENT_BASE_DIR}/gflags-build)

include_directories(${FETCHCONTENT_BASE_DIR}/gflags-build/include)

add_executable(example src/example.cpp)
add_dependencies(example gflags)
target_link_libraries(example gflags)
