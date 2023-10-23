cmake_minimum_required(VERSION 3.18.6)
project(alice)

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
include(GNUInstallDirs)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/deps.cmake)

################################
# alice
################################

# version
file(STRINGS "version.txt" alice_version)
string(REPLACE "-" ";" alice_semver_ext ${alice_version})
list(GET alice_semver_ext 0 alice_semver)
string(REPLACE "." ";" alice_semver_list ${alice_semver})

list(GET alice_semver_list 0 ALICE_VERSION_MAJOR)
list(GET alice_semver_list 1 ALICE_VERSION_MINOR)
list(GET alice_semver_list 2 ALICE_VERSION_PATCH)
set(ALICE_VERSION "${ALICE_VERSION_MAJOR}.${ALICE_VERSION_MINOR}.${ALICE_VERSION_PATCH}")

# configure
set(alice_src_dir ${CMAKE_CURRENT_LIST_DIR})
set(alice_gen_dir ${CMAKE_CURRENT_BINARY_DIR}/generated)
if (BUILD_SHARED_LIBS)
	set(ALICE_USE_DLL ON)
	set(ALICE_LIB_TYPE SHARED)
else()
	set(ALICE_USE_DLL OFF)
	set(ALICE_LIB_TYPE STATIC)
endif()

configure_file(
	${alice_src_dir}/alice/alice_config.h.in
	${alice_gen_dir}/alice/alice_config.h)

# add library
FILE(GLOB_RECURSE alice_src_c "${alice_src_dir}/alice/*.c")
FILE(GLOB_RECURSE alice_src_h "${alice_src_dir}/alice/*.h")
FILE(GLOB_RECURSE alice_gen_h "${alice_gen_dir}/alice/*.h")
add_library(alice ${ALICE_LIB_TYPE}
	${alice_src_c}
	${alice_src_h}
	${alice_gen_h})
target_link_libraries(alice foo)
target_include_directories(alice PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<BUILD_INTERFACE:${alice_gen_dir}>
	$<INSTALL_INTERFACE:include>)

target_compile_definitions(alice PRIVATE ALICE_EXPORTS)
set_target_properties(alice PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	VERSION ${ALICE_VERSION}
	SOVERSION ${ALICE_VERSION_MAJOR})

# install
install(TARGETS alice
	EXPORT aliceTargets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${alice_src_dir}/alice" "${alice_gen_dir}/alice"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "*.hpp")

# cmake package config
set(alice_export_dir "${CMAKE_INSTALL_LIBDIR}/cmake/alice")
install(EXPORT aliceTargets
	FILE aliceTargets.cmake
	DESTINATION "${alice_export_dir}")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_LIST_DIR}/cmake/aliceConfig.cmake.in"
    "${alice_gen_dir}/aliceConfig.cmake"
	INSTALL_DESTINATION "${alice_export_dir}"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
    "${alice_gen_dir}/aliceConfigVersion.cmake"
	VERSION ${ALICE_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES
	"${alice_gen_dir}/aliceConfig.cmake"
	"${alice_gen_dir}/aliceConfigVersion.cmake"
	DESTINATION ${alice_export_dir})
