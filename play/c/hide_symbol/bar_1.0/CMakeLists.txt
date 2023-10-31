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

set(BAR_VERSION "1.0.0")
set(BAR_SOVERSION "1")

FILE(GLOB_RECURSE bar_src_c "${CMAKE_CURRENT_LIST_DIR}/bar/*.c")
FILE(GLOB_RECURSE bar_src_h "${CMAKE_CURRENT_LIST_DIR}/bar/*.h")
add_library(bar SHARED
	${bar_src_c}
	${bar_src_h})
target_include_directories(bar PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(bar PRIVATE foo)
set_target_properties(bar PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	C_VISIBILITY_PRESET hidden
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN YES
	VERSION ${BAR_VERSION}
	SOVERSION ${BAR_SOVERSION})

# install
include(GNUInstallDirs)
install(TARGETS bar
	EXPORT barTargets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bar"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.hpp")

# cmake package config
set(bar_export_dir "${CMAKE_INSTALL_LIBDIR}/cmake/bar")
set(bar_gen_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

install(EXPORT barTargets
	FILE barTargets.cmake
	DESTINATION "${bar_export_dir}")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_LIST_DIR}/cmake/barConfig.cmake.in"
    "${bar_gen_dir}/barConfig.cmake"
	INSTALL_DESTINATION "${bar_export_dir}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
    "${bar_gen_dir}/barConfigVersion.cmake"
	VERSION ${BAR_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES
	"${bar_gen_dir}/barConfig.cmake"
	"${bar_gen_dir}/barConfigVersion.cmake"
	DESTINATION ${bar_export_dir})
