cmake_minimum_required (VERSION 3.18.6)
project(hide_symbol)

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
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
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

# for lsp
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
option(BUILD_SHARED_LIBS "Build shared or static library" ON)
if (BUILD_SHARED_LIBS)
	set(BOB_LIB_TYPE SHARED)
	set(BOB_USE_DLL ON)
else()
	set(BOB_LIB_TYPE STATIC)
	set(BOB_USE_DLL OFF)
endif()

################################
# version
################################

file(STRINGS "version.txt" bob_version)
string(REPLACE "-" ";" bob_semver_ext ${bob_version})
list(GET bob_semver_ext 0 bob_semver)
string(REPLACE "." ";" bob_semver_list ${bob_semver})

list(GET bob_semver_list 0 BOB_VERSION_MAJOR)
list(GET bob_semver_list 1 BOB_VERSION_MINOR)
list(GET bob_semver_list 2 BOB_VERSION_PATCH)

set(BOB_VERSION "${bob_version}")
set(BOB_SOVERSION "${BOB_VERSION_MAJOR}")

################################
# deps
################################
find_package(foo REQUIRED)
if (foo_FOUND)
	message("Found foo: ${foo_DIR}")
else()
	message(FATAL_ERROR "failed found foo")
endif()

################################
# project
################################

message("--------------------------------")
message("# bob version: ${BOB_VERSION}")
message("# CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("# BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("--------------------------------")

set(bob_src_dir ${CMAKE_CURRENT_LIST_DIR})
set(bob_gen_dir ${CMAKE_BINARY_DIR}/gen)
configure_file(
	"${bob_src_dir}/bob/bob_config.h.in"
	"${bob_gen_dir}/bob/bob_config.h")

FILE(GLOB_RECURSE bob_src_c "${bob_src_dir}/bob/*.c")
FILE(GLOB_RECURSE bob_src_h "${bob_src_dir}/bob/*.h")
FILE(GLOB_RECURSE bob_gen_h "${bob_gen_dir}/bob/*.h")
add_library(bob ${BOB_LIB_TYPE}
	${bob_src_c}
	${bob_src_h}
	${bob_gen_h})
target_include_directories(bob PUBLIC 
	$<BUILD_INTERFACE:${bob_src_dir}>
	$<BUILD_INTERFACE:${bob_gen_dir}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(bob PRIVATE foo)
set_target_properties(bob PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	VERSION ${BOB_VERSION}
	SOVERSION ${BOB_SOVERSION})
target_compile_definitions(bob PRIVATE BAR_EXPORTS)

if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
	target_link_options(bob PRIVATE "-Wl,--exclude-libs,ALL")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL Clang)
	target_link_options(bob PRIVATE "-Wl,--exclude-libs,ALL")
endif()

# install
include(GNUInstallDirs)
install(TARGETS bob
	EXPORT bobTargets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${bob_src_dir}/bob" "${bob_gen_dir}/bob"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.hpp")

# cmake package config
set(bob_export_dir "${CMAKE_INSTALL_LIBDIR}/cmake/bob")

install(EXPORT bobTargets
	FILE bobTargets.cmake
	DESTINATION "${bob_export_dir}")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_LIST_DIR}/cmake/bobConfig.cmake.in"
    "${bob_gen_dir}/bobConfig.cmake"
	INSTALL_DESTINATION "${bob_export_dir}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
    "${bob_gen_dir}/bobConfigVersion.cmake"
	VERSION ${BOB_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES
	"${bob_gen_dir}/bobConfig.cmake"
	"${bob_gen_dir}/bobConfigVersion.cmake"
	DESTINATION ${bob_export_dir})
