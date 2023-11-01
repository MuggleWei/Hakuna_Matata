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
# project
################################

set(FOO_VERSION "1.2.0")
set(FOO_SOVERSION "1")

FILE(GLOB_RECURSE foo_src_c "${CMAKE_CURRENT_LIST_DIR}/foo/*.c")
FILE(GLOB_RECURSE foo_src_h "${CMAKE_CURRENT_LIST_DIR}/foo/*.h")
add_library(foo STATIC
	${foo_src_c}
	${foo_src_h})
target_include_directories(foo PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<INSTALL_INTERFACE:include>)
set_target_properties(foo PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	POSITION_INDEPENDENT_CODE ON
	VERSION ${FOO_VERSION}
	SOVERSION ${FOO_SOVERSION})

# install
include(GNUInstallDirs)
install(TARGETS foo
	EXPORT fooTargets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/foo"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.hpp")

# cmake package config
set(foo_export_dir "${CMAKE_INSTALL_LIBDIR}/cmake/foo")
set(foo_gen_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

install(EXPORT fooTargets
	FILE fooTargets.cmake
	DESTINATION "${foo_export_dir}")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_LIST_DIR}/cmake/fooConfig.cmake.in"
    "${foo_gen_dir}/fooConfig.cmake"
	INSTALL_DESTINATION "${foo_export_dir}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
    "${foo_gen_dir}/fooConfigVersion.cmake"
	VERSION ${FOO_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES
	"${foo_gen_dir}/fooConfig.cmake"
	"${foo_gen_dir}/fooConfigVersion.cmake"
	DESTINATION ${foo_export_dir})
