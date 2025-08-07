cmake_minimum_required (VERSION 3.17.6)
project(hugepage_test)

################################
# general config
################################

# print compiler
message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

# set compile parameter
if (NOT CMAKE_CROSSCOMPILING)
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
	endif()
else()
	if (ANDROID)
		message("-- cmake run cross compiling, building for android")
	else()
		message(FATAL_ERROR "-- building for unsupport platform")
	endif()
endif()

# set standard and print features
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
# examples
################################
add_executable(hugepage_test ${CMAKE_CURRENT_LIST_DIR}/src/main.c)
