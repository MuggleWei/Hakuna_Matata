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

set(BOB_VERSION "1.0.0")
set(BOB_SOVERSION "1")

FILE(GLOB_RECURSE bob_src_c "${CMAKE_CURRENT_LIST_DIR}/bob/*.c")
FILE(GLOB_RECURSE bob_src_h "${CMAKE_CURRENT_LIST_DIR}/bob/*.h")
add_library(bob SHARED
	${bob_src_c}
	${bob_src_h})
target_include_directories(bob PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(bob PRIVATE foo)
set_target_properties(bob PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	VERSION ${BOB_VERSION}
	SOVERSION ${BOB_SOVERSION})

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
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bob"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.hpp")

# cmake package config
set(bob_export_dir "${CMAKE_INSTALL_LIBDIR}/cmake/bob")
set(bob_gen_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

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
