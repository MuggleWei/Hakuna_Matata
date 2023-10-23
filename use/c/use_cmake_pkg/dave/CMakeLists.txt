cmake_minimum_required(VERSION 3.18.6)
project(dave)

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
# deps
################################
find_package(foo 1 REQUIRED)
if (foo_FOUND)
	message("found foo: ${foo_DIR}")
else()
	message(FATAL_ERROR "Failed found foo")
endif()

################################
# dave
################################

# version
file(STRINGS "version.txt" dave_version)
string(REPLACE "-" ";" dave_semver_ext ${dave_version})
list(GET dave_semver_ext 0 dave_semver)
string(REPLACE "." ";" dave_semver_list ${dave_semver})

list(GET dave_semver_list 0 DAVE_VERSION_MAJOR)
list(GET dave_semver_list 1 DAVE_VERSION_MINOR)
list(GET dave_semver_list 2 DAVE_VERSION_PATCH)
set(DAVE_VERSION "${DAVE_VERSION_MAJOR}.${DAVE_VERSION_MINOR}.${DAVE_VERSION_PATCH}")

# configure
set(dave_src_dir ${CMAKE_CURRENT_LIST_DIR})
set(dave_gen_dir ${CMAKE_CURRENT_BINARY_DIR}/generated)
if (BUILD_SHARED_LIBS)
	set(DAVE_USE_DLL ON)
	set(DAVE_LIB_TYPE SHARED)
else()
	set(DAVE_USE_DLL OFF)
	set(DAVE_LIB_TYPE STATIC)
endif()

configure_file(
	${dave_src_dir}/dave/dave_config.h.in
	${dave_gen_dir}/dave/dave_config.h)

# add library
FILE(GLOB_RECURSE dave_src_c "${dave_src_dir}/dave/*.c")
FILE(GLOB_RECURSE dave_src_h "${dave_src_dir}/dave/*.h")
FILE(GLOB_RECURSE dave_gen_h "${dave_gen_dir}/dave/*.h")
add_library(dave ${DAVE_LIB_TYPE}
	${dave_src_c}
	${dave_src_h}
	${dave_gen_h})
target_link_libraries(dave foo)
target_include_directories(dave PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<BUILD_INTERFACE:${dave_gen_dir}>
	$<INSTALL_INTERFACE:include>)

target_compile_definitions(dave PRIVATE DAVE_EXPORTS)
set_target_properties(dave PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	VERSION ${DAVE_VERSION}
	SOVERSION ${DAVE_VERSION_MAJOR})

# install
include(GNUInstallDirs)
install(TARGETS dave
	EXPORT daveTargets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${dave_src_dir}/dave" "${dave_gen_dir}/dave"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.hpp")

# cmake package config
set(dave_export_dir "${CMAKE_INSTALL_LIBDIR}/cmake/dave")
install(EXPORT daveTargets
	FILE daveTargets.cmake
	DESTINATION "${dave_export_dir}")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_LIST_DIR}/cmake/daveConfig.cmake.in"
    "${dave_gen_dir}/daveConfig.cmake"
	INSTALL_DESTINATION "${dave_export_dir}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
    "${dave_gen_dir}/daveConfigVersion.cmake"
	VERSION ${DAVE_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES
	"${dave_gen_dir}/daveConfig.cmake"
	"${dave_gen_dir}/daveConfigVersion.cmake"
	DESTINATION ${dave_export_dir})
