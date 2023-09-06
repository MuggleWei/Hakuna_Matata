cmake_minimum_required (VERSION 3.18.6)
project(use_miniaudio_deps)

################################
# general config
################################

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
# deps
################################
include(FetchContent)
include(GNUInstallDirs)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

# mugglec
set(MUGGLE_BUILD_TRACE OFF CACHE BOOL "")
set(MUGGLE_BUILD_SHARED_LIB ON CACHE BOOL "")
set(MUGGLE_BUILD_STATIC_PIC ON CACHE BOOL "")
set(MUGGLE_BUILD_BENCHMARK OFF CACHE BOOL "")
set(MUGGLE_BUILD_TESTING OFF CACHE BOOL "")
set(MUGGLE_BUILD_EXAMPLE OFF CACHE BOOL "")

FetchContent_Declare(
	mugglec
	GIT_REPOSITORY https://github.com/MuggleWei/mugglec.git
	GIT_TAG v1.3.2
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(mugglec)

install(IMPORTED_RUNTIME_ARTIFACTS mugglec)

# miniaudio
FetchContent_Declare(
	miniaudio
	GIT_REPOSITORY https://github.com/mackron/miniaudio.git
	GIT_TAG 0.11.18
	GIT_SHALLOW TRUE
)
FetchContent_Populate(miniaudio)

install(FILES "${FETCHCONTENT_BASE_DIR}/miniaudio-src/miniaudio.h"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/miniaudio")
