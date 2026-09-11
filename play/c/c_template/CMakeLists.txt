cmake_minimum_required (VERSION 3.18.6)
project(hello)

################################
# general config
################################

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
set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
	"Build type (Debug, RelWithDebInfo, Release or MinSizeRel)")
option(BUILD_SHARED_LIBS "Build shared or static library" ON)
option(BUILD_TESTING "Build testing" OFF)

################################
# deps
################################

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

# mugglec
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

message("# fetch haclog")
FetchContent_Declare(
	haclog
	GIT_REPOSITORY https://github.com/MuggleWei/haclog.git
	GIT_TAG v0.4.5
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(haclog)

################################
# version
################################
file(STRINGS "version.txt" foo_version)
string(REPLACE "-" ";" foo_semver_ext ${foo_version})
list(GET foo_semver_ext 0 foo_semver)
string(REPLACE "." ";" foo_semver_list ${foo_semver})

list(GET foo_semver_list 0 FOO_VERSION_MAJOR)
list(GET foo_semver_list 1 FOO_VERSION_MINOR)
list(GET foo_semver_list 2 FOO_VERSION_PATCH)

set(FOO_VERSION "${foo_version}")
set(FOO_SOVERSION "${FOO_VERSION_MAJOR}")

################################
# output options
################################
string(TOUPPER ${CMAKE_BUILD_TYPE} upper_build_type)

message("--------------------------------")
message("# CMAKE_BUILD_TYPE: ${upper_build_type}")
message("# BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("# c compiler: ${CMAKE_C_COMPILER}")
message("# c compiler version: ${CMAKE_C_COMPILER_VERSION}")
message("# c compiler flags: ${CMAKE_C_FLAGS}")
message("# c compiler build type flags: ${CMAKE_C_FLAGS_${upper_build_type}}")
message("# CMAKE_C_STANDARD: ${CMAKE_C_STANDARD}")
message("# CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	message("# deps runtime: ${deps_rt}")
	message("# distro: ${distro}")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	message("# CMAKE_OSX_DEPLOYMENT_TARGET: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
	message("# CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")
endif()
message("# app version: ${FOO_VERSION}")

################################
# functions
################################

function(add_app name folder)
	message("add app ${name} ${folder}")

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
	endif(WIN32)
	target_include_directories(${name} PUBLIC ${folder})
	target_link_libraries(${name} PUBLIC mugglec haclog)
	target_include_directories(${name} PUBLIC ${gen_dir})

	install(TARGETS ${name} RUNTIME DESTINATION bin)
endfunction()

################################
# app
################################

# configure
set(src_dir ${CMAKE_CURRENT_LIST_DIR}/src)
set(gen_dir ${CMAKE_BINARY_DIR}/gen)
configure_file(
	"${src_dir}/foo/config.h.in"
	"${gen_dir}/foo/config.h")

# add application
add_app(foo ${src_dir}/foo)
