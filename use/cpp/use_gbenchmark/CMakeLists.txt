cmake_minimum_required (VERSION 3.18.6)
project(use_gbenchmark)

################################
# general config
################################

message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

# set compile parameter
if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (${CMAKE_C_COMPILER_ID} MATCHES Clang)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-missing-field-initializers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

	# see: https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-170
	# The /Zc:__cplusplus compiler option enables updated __cplusplus macro
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/std:c++20>")
endif()

# Mac M1 clang compiler default c++ version set to c++97, it will lead 
# gbenchmark_haclog compile error
if (APPLE)
	if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
		message("# detect APPLE arm, set c&cxx standard to 11")
		set(CMAKE_C_STANDARD 11)
		set(CMAKE_C_STANDARD_REQUIRED ON)
		set(CMAKE_CXX_STANDARD 11)
		set(CMAKE_CXX_STANDARD_REQUIRED ON)
	endif()
endif()

message("-- c compiler support features: ")
foreach(feature ${CMAKE_C_COMPILE_FEATURES})
	message("support feature: ${feature}")
endforeach()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################
# options
################################

set(CMAKE_BUILD_TYPE "Release" CACHE STRING
	"Build type (Debug, RelWithDebInfo, Release, MinSizeRel or Coverage)")

################################
# deps
################################
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

################################
# gbenchmark
################################

message("-- fetch google benchmark")
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "")
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "")
FetchContent_Declare(
	benchmark
	GIT_REPOSITORY https://github.com/google/benchmark.git
	GIT_TAG v1.8.3
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
	)
FetchContent_MakeAvailable(benchmark)

function(add_gbenchmark name folder)
	message("add gbenchmark ${name} ${folder}")
	set(name gbenchmark_${name})

	file(GLOB_RECURSE tmp_h ${folder}/*.h)
	file(GLOB_RECURSE tmp_c ${folder}/*.c)
	file(GLOB_RECURSE tmp_cpp ${folder}/*.cpp)
	file(GLOB_RECURSE tmp_cc ${folder}/*.cc)

	if (MSVC OR MINGW)
		add_executable(${name} ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		set_target_properties(${name}
			PROPERTIES
			FOLDER "example"
			VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
		)
	else()
		add_executable(${name} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		if (APPLE)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "@executable_path/../lib"
			)
		elseif (UNIX)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "\$ORIGIN/../lib"
			)
		endif()
	endif(MSVC OR MINGW)
	target_link_libraries(${name} benchmark)
	add_dependencies(${name} benchmark)

	# save temp object file, such as: asm code
	if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU") OR
		(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
		target_compile_options(${name} PUBLIC -save-temps=obj)
	endif()
endfunction()

set(gbenchmark_root_dir ${CMAKE_CURRENT_LIST_DIR}/gbenchmark)
FILE(GLOB subdirs RELATIVE ${gbenchmark_root_dir} ${gbenchmark_root_dir}/*)
FOREACH(subdir ${subdirs})
	IF(IS_DIRECTORY ${gbenchmark_root_dir}/${subdir})
		add_gbenchmark(${subdir} ${gbenchmark_root_dir}/${subdir})
	ENDIF()
ENDFOREACH()
