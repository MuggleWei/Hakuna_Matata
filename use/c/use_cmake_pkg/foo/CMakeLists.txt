cmake_minimum_required(VERSION 3.18.6)
project(foo)

# print compiler
message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

# set compile parameter
if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra")
elseif (${CMAKE_C_COMPILER_ID} MATCHES Clang)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# set standard and print features
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

message("-- c compiler support features: ")
foreach(feature ${CMAKE_C_COMPILE_FEATURES})
	message("support feature: ${feature}")
endforeach()

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################
# foo
################################

# version
file(STRINGS "version.txt" foo_version)
string(REPLACE "-" ";" foo_semver_ext ${foo_version})
list(GET foo_semver_ext 0 foo_semver)
string(REPLACE "." ";" foo_semver_list ${foo_semver})

list(GET foo_semver_list 0 FOO_VERSION_MAJOR)
list(GET foo_semver_list 1 FOO_VERSION_MINOR)
list(GET foo_semver_list 2 FOO_VERSION_PATCH)
set(FOO_VERSION "${FOO_VERSION_MAJOR}.${FOO_VERSION_MINOR}.${FOO_VERSION_PATCH}")

# configure
set(foo_src_dir ${CMAKE_CURRENT_LIST_DIR})
set(foo_gen_dir ${CMAKE_CURRENT_BINARY_DIR}/generated)
if (BUILD_SHARED_LIBS)
	set(FOO_USE_DLL ON)
	set(FOO_LIB_TYPE SHARED)
else()
	set(FOO_USE_DLL OFF)
	set(FOO_LIB_TYPE STATIC)
endif()

configure_file(
	${foo_src_dir}/foo/foo_config.h.in
	${foo_gen_dir}/foo/foo_config.h)

# add library
FILE(GLOB_RECURSE foo_src_c "${foo_src_dir}/foo/*.c")
FILE(GLOB_RECURSE foo_src_h "${foo_src_dir}/foo/*.h")
FILE(GLOB_RECURSE foo_gen_h "${foo_gen_dir}/foo/*.h")
add_library(foo ${FOO_LIB_TYPE}
	${foo_src_c}
	${foo_src_h}
	${foo_gen_h})
target_include_directories(foo PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<BUILD_INTERFACE:${foo_gen_dir}>
	$<INSTALL_INTERFACE:include>)

target_compile_definitions(foo PRIVATE FOO_EXPORTS)
set_target_properties(foo PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	VERSION ${FOO_VERSION}
	SOVERSION ${FOO_VERSION_MAJOR})

# install
include(GNUInstallDirs)
install(TARGETS foo
	EXPORT fooTargets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${foo_src_dir}/foo" "${foo_gen_dir}/foo"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.hpp")

# cmake package config
set(foo_export_dir "${CMAKE_INSTALL_LIBDIR}/cmake/foo")
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

# pkgconfig
configure_file(
	"${CMAKE_CURRENT_LIST_DIR}/cmake/foo.pc.in"
	"${foo_gen_dir}/foo.pc" @ONLY)
install(FILES
	"${foo_gen_dir}/foo.pc"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

# cpack
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${FOO_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${FOO_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${FOO_VERSION_PATCH})
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)
