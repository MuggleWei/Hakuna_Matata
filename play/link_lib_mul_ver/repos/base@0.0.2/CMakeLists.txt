cmake_minimum_required (VERSION 3.0.2)
project(base)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# cmake
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/muggle_utils.cmake)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# set values
set(LIB_TYPE "SHARED" CACHE STRING "build lib type")

# include directories
include_directories(
	${CMAKE_CURRENT_LIST_DIR}
)

# version
set(BASE_VER_MAJOR 0)
set(BASE_VER_MINOR 0)
set(BASE_VER_BUILD 2)

set(BASE_VERSION "${BASE_VER_MAJOR}.${BASE_VER_MINOR}.${BASE_VER_BUILD}")
set(BASE_SOVERSION "${BASE_VER_MAJOR}")

# base
set(base base)
muggle_add_project(${base} ${CMAKE_CURRENT_LIST_DIR}/src ${LIB_TYPE})
set_target_properties(${base} PROPERTIES
	LINKER_LANGUAGE C
	VERSION ${BASE_VERSION}
	SOVERSION ${BASE_SOVERSION}
	POSITION_INDEPENDENT_CODE ON
)

# install
target_compile_definitions(${base} PUBLIC
	BASE_VER_MAJOR=${BASE_VER_MAJOR}
	BASE_VER_MINOR=${BASE_VER_MINOR}
	BASE_VER_BUILD=${BASE_VER_BUILD}
)
install(TARGETS ${base}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static/)
muggle_install_headers(${CMAKE_CURRENT_LIST_DIR}/src include/${base}/${BASE_VERSION})
