cmake_minimum_required (VERSION 3.17.6)
project(false_sharing_benchmark)

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
# deps
################################

# set fetch content
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

# mugglec
set(MUGGLE_BUILD_TRACE OFF CACHE BOOL "")
set(MUGGLE_BUILD_SHARED_LIB ON CACHE BOOL "")
set(MUGGLE_BUILD_STATIC_PIC ON CACHE BOOL "")
set(MUGGLE_BUILD_BENCHMARK OFF CACHE BOOL "")
set(MUGGLE_BUILD_TESTING OFF CACHE BOOL "")
set(MUGGLE_BUILD_EXAMPLE OFF CACHE BOOL "")

message("fetch mugglec")
FetchContent_Declare(
	mugglec
	GIT_REPOSITORY https://github.com/MuggleWei/mugglec.git
	GIT_TAG v2.0.0-alpha.2
)
FetchContent_MakeAvailable(mugglec)

################################
# examples
################################

function(add_example name folder)
	message("add example ${name} ${folder}")
	
	set(name ${name})

	file(GLOB_RECURSE tmp_h ${folder}/*.h)
	file(GLOB_RECURSE tmp_c ${folder}/*.c)
	file(GLOB_RECURSE tmp_cpp ${folder}/*.cpp)
	file(GLOB_RECURSE tmp_cc ${folder}/*.cc)

	if (WIN32)
		add_executable(${name} ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		add_executable(${name}_64 ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		add_executable(${name}_128 ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		set_target_properties(${name}
			PROPERTIES
			FOLDER "example"
			VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
		)
	else()
		add_executable(${name} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		add_executable(${name}_64 ${tmp_c} ${tmp_cpp} ${tmp_cc})
		add_executable(${name}_128 ${tmp_c} ${tmp_cpp} ${tmp_cc})
	endif(WIN32)

	target_compile_definitions(${name}_64 PRIVATE DATA_ALIGN_SIZE_64)
	target_compile_definitions(${name}_128 PRIVATE DATA_ALIGN_SIZE_128)

	add_dependencies(${name} mugglec)
	target_link_libraries(${name} PUBLIC mugglec)
	target_compile_options(${name} PUBLIC -save-temps=obj)

	add_dependencies(${name}_64 mugglec)
	target_link_libraries(${name}_64 PUBLIC mugglec)
	target_compile_options(${name}_64 PUBLIC -save-temps=obj)

	add_dependencies(${name}_128 mugglec)
	target_link_libraries(${name}_128 PUBLIC mugglec)
	target_compile_options(${name}_128 PUBLIC -save-temps=obj)
endfunction()

# examples
SUBDIRLIST(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/example)
FOREACH(subdir ${example_root_dir})
	add_example(${subdir} ${CMAKE_CURRENT_LIST_DIR}/example/${subdir})
ENDFOREACH()
