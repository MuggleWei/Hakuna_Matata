cmake_minimum_required (VERSION 3.18.6)
project(haclog)

################################
# general config
################################

message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (${CMAKE_C_COMPILER_ID} MATCHES Clang)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

	if (MSVC_VERSION GREATER_EQUAL 1920)
		message("MSVC version >= VS 16.0 (v142 toolset)")

		# see: https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-170
		# The /Zc:__cplusplus compiler option enables updated __cplusplus macro
		add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>")
		add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/std:c++20>")
	endif()
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

message("-- c compiler support features: ")
foreach(feature ${CMAKE_C_COMPILE_FEATURES})
	message("support feature: ${feature}")
endforeach()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# for LSP
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################
# deps
################################
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

# set mugglec compile options
set(MUGGLE_BUILD_TRACE OFF CACHE BOOL "")
set(MUGGLE_BUILD_SHARED_LIB ON CACHE BOOL "")
set(MUGGLE_BUILD_STATIC_PIC ON CACHE BOOL "")
set(MUGGLE_BUILD_BENCHMARK OFF CACHE BOOL "")
set(MUGGLE_BUILD_TESTING OFF CACHE BOOL "")
set(MUGGLE_BUILD_EXAMPLE OFF CACHE BOOL "")

message("# fetch mugglec")
FetchContent_Declare(
	mugglec
	GIT_REPOSITORY https://github.com/MuggleWei/mugglec.git
	GIT_TAG v2.5.0
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(mugglec)

################################
# options
################################
option(BUILD_SANITIZER "Compile with sanitizer" OFF)

################################
# app
################################
set(app tcp_n_datas)
set(src_dir ${CMAKE_CURRENT_LIST_DIR}/src)

FILE(GLOB_RECURSE app_c "${src_dir}/*.c")
FILE(GLOB_RECURSE app_h "${src_dir}/*.h")
add_executable(${app} ${app_c} ${app_h})
target_include_directories(${app} PUBLIC
	$<BUILD_INTERFACE:${src_dir}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(${app} PUBLIC mugglec)

# sanitizer
if (BUILD_SANITIZER)
	message("build with sanitizer")
	if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") OR
		(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
		target_compile_options(${app} PUBLIC
			-fsanitize=undefined
			-fsanitize=address
			-fsanitize=leak)
		target_link_options(${app} PUBLIC
			-fsanitize=undefined
			-fsanitize=address
			-fsanitize=leak)
	endif()
endif()
