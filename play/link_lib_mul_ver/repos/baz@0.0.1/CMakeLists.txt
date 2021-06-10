cmake_minimum_required (VERSION 3.0.2)
project(baz)

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

# set values
set(LIB_TYPE "SHARED" CACHE STRING "build lib type")

# cmake
set(COMMON_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../common/cmake)
include(${COMMON_CMAKE_DIR}/muggle_utils.cmake)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# version
set(BAZ_VER_MAJOR 0)
set(BAZ_VER_MINOR 0)
set(BAZ_VER_BUILD 1)

set(BAZ_VERSION "${BAZ_VER_MAJOR}.${BAZ_VER_MINOR}.${BAZ_VER_BUILD}")
set(BAZ_SOVERSION "${BAZ_VER_MAJOR}")

# baz
set(baz baz)
muggle_add_project(${baz} ${CMAKE_CURRENT_LIST_DIR}/src ${LIB_TYPE})
set_target_properties(${baz} PROPERTIES
	LINKER_LANGUAGE C
	VERSION ${BAZ_VERSION}
	SOVERSION ${BAZ_SOVERSION}
)
target_compile_definitions(${baz} PUBLIC
	BAZ_VER_MAJOR=${BAZ_VER_MAJOR}
	BAZ_VER_MINOR=${BAZ_VER_MINOR}
	BAZ_VER_BUILD=${BAZ_VER_BUILD}
)
target_include_directories(${baz} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/src
)

# thirdparty
set(base_soname 1)
set(base_ver 1.0.0)
target_include_directories(${baz} PUBLIC
	${CMAKE_INSTALL_PREFIX}/include/base/${base_ver}
)
if ("${LIB_BASE_TYPE}" STREQUAL "SHARED")
	find_library(base REQUIRED
		NAMES libbase.so.${base_soname}
		PATHS ${CMAKE_INSTALL_PREFIX}/lib
	)
else()
	find_library(base REQUIRED
		NAMES libbase.a
		PATHS ${CMAKE_INSTALL_PREFIX}/lib/static
	)
endif()
target_link_libraries(${baz}
	${base}
)

# install
target_compile_definitions(${baz} PUBLIC
	BAZ_VER_MAJOR=${BAZ_VER_MAJOR}
	BAZ_VER_MINOR=${BAZ_VER_MINOR}
	BAZ_VER_BUILD=${BAZ_VER_BUILD}
)
install(TARGETS ${baz}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static/)
muggle_install_headers(${CMAKE_CURRENT_LIST_DIR}/src include/${baz}/${BAZ_VERSION})
