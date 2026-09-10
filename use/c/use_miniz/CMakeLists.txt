cmake_minimum_required (VERSION 3.18.0)
project(use_miniz)

################################
# general config
################################

# set compile parameter
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

message("-- use c compiler ${CMAKE_C_COMPILER}")
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
# options
################################
set(CMAKE_BUILD_TYPE "Release" CACHE STRING
	"Build type (Debug, RelWithDebInfo, Release or MinSizeRel)")
option(BUILD_SHARED_LIBS "Build shared or static library" ON)
option(BUILD_TESTING "Build testing" OFF)

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
	GIT_TAG v2.5.1
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(mugglec)

message("# fetch miniz")
FetchContent_Declare(
	miniz
	GIT_REPOSITORY https://github.com/richgel999/miniz.git
	GIT_TAG 3.1.2
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(miniz)

################################
# output options
################################
message("--------------------------------")
message("# CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("# BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("# CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("# CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
if (APPLE)
	message("# CMAKE_OSX_DEPLOYMENT_TARGET: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
	message("# CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")
endif()
message("# c compiler: ${CMAKE_C_COMPILER}")
message("# c compiler version: ${CMAKE_C_COMPILER_VERSION}")
message("# c compiler flags: ${CMAKE_C_FLAGS}")
message("# c compiler build flags: ${CMAKE_C_FLAGS_${upper_cmake_build_type}}")
message("# BUILD_TESTING: ${BUILD_TESTING}")

################################
# configure
################################
set(src_dir ${CMAKE_CURRENT_LIST_DIR}/src)
set(gen_dir ${CMAKE_BINARY_DIR}/gen)
configure_file(
	"${src_dir}/config.h.in"
	"${gen_dir}/config.h")

################################
# examples
################################
function(add_example name folder)
    message("add example ${name} ${folder}")

    set(name example_${name})

    file(GLOB_RECURSE tmp_h ${folder}/*.h)
    file(GLOB_RECURSE tmp_c ${folder}/*.c)
    file(GLOB_RECURSE tmp_cpp ${folder}/*.cpp)
    file(GLOB_RECURSE tmp_cc ${folder}/*.cc)

    if (WIN32)
        add_executable(${name} ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
        set_target_properties(${name}
            PROPERTIES
            FOLDER "example"
            VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
        )
    else()
        add_executable(${name} ${tmp_c} ${tmp_cpp} ${tmp_cc})
    endif(WIN32)
    target_include_directories(${name} PUBLIC ${folder} ${generated_dir})
	target_link_libraries(${name} PUBLIC mugglec miniz)

	if ((${CMAKE_C_COMPILER_ID} STREQUAL "GNU") OR
		(${CMAKE_C_COMPILER_ID} STREQUAL "Clang"))
		target_compile_options(${name} PUBLIC -save-temps=obj)
	endif()
endfunction()

set(example_root_dir ${src_dir})
FILE(GLOB subdirs RELATIVE ${example_root_dir} ${example_root_dir}/*)
FOREACH(subdir ${subdirs})
	IF(IS_DIRECTORY ${example_root_dir}/${subdir})
		add_example(${subdir} ${example_root_dir}/${subdir})
	ENDIF()
ENDFOREACH()
