cmake_minimum_required(VERSION 3.18.6)
project(foo)

set(FOO_VERSION "1.0.0")
set(FOO_VER_MAJOR "1")
set(FOO_VER_MINOR "0")
set(FOO_VER_PATCH "0")

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

# set shared or static, and configure file
if (BUILD_SHARED_LIBS)
	set(FOO_USE_DLL ON)
else()
	set(FOO_USE_DLL OFF)
endif()

set(generated_dir ${CMAKE_CURRENT_BINARY_DIR}/generated)
configure_file(
	${CMAKE_CURRENT_LIST_DIR}/foo/foo_config.h.in
	${generated_dir}/foo/foo_config.h)

# add library
add_library(foo
	${CMAKE_CURRENT_LIST_DIR}/foo/foo.c)
target_include_directories(foo PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<BUILD_INTERFACE:${generated_dir}>
	$<INSTALL_INTERFACE:include>)
target_compile_definitions(foo PRIVATE FOO_EXPORTS)
set_target_properties(foo PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	VERSION ${FOO_VERSION}
	SOVERSION ${FOO_VER_MAJOR})

# install and add export configuration
install(TARGETS foo
    DESTINATION lib
    EXPORT fooTargets)
install(EXPORT fooTargets
    FILE fooTargets.cmake
    DESTINATION lib/cmake/foo)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/fooConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/foo"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/fooConfigVersion.cmake"
    VERSION ${FOO_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/fooConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/fooConfigVersion.cmake"
    DESTINATION lib/cmake/foo
)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/foo/foo.h" DESTINATION include/foo)
install(FILES "${generated_dir}/foo/foo_config.h" DESTINATION include/foo)

# cpack
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${FOO_VER_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${FOO_VER_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${FOO_VER_PATCH})
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)

# pkg-config
include(GNUInstallDirs)
configure_file(
	"${CMAKE_CURRENT_LIST_DIR}/foo.pc.in"
	"${generated_dir}/foo.pc" @ONLY)
install(FILES
	"${generated_dir}/foo.pc"
	DESTINATION lib/pkgconfig)
