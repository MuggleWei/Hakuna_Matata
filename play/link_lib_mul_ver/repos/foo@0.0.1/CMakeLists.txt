cmake_minimum_required (VERSION 3.0.2)
project(foo)

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
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/muggle_utils.cmake)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# version
set(FOO_VER_MAJOR 0)
set(FOO_VER_MINOR 0)
set(FOO_VER_BUILD 1)

set(FOO_VERSION "${FOO_VER_MAJOR}.${FOO_VER_MINOR}.${FOO_VER_BUILD}")
set(FOO_SOVERSION "${FOO_VER_MAJOR}")

# foo
set(foo foo)
muggle_add_project(${foo} ${CMAKE_CURRENT_LIST_DIR}/src ${LIB_TYPE})
set_target_properties(${foo} PROPERTIES
	LINKER_LANGUAGE C
	VERSION ${FOO_VERSION}
	SOVERSION ${FOO_SOVERSION}
)
target_compile_definitions(${foo} PUBLIC
	FOO_VER_MAJOR=${FOO_VER_MAJOR}
	FOO_VER_MINOR=${FOO_VER_MINOR}
	FOO_VER_BUILD=${FOO_VER_BUILD}
)
target_include_directories(${foo} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/src
)

# thirdparty
set(base_soname 0)
set(base_ver 0.0.1)
target_include_directories(${foo} PUBLIC
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
target_link_libraries(${foo}
	${base}
)

# install
target_compile_definitions(${foo} PUBLIC
	FOO_VER_MAJOR=${FOO_VER_MAJOR}
	FOO_VER_MINOR=${FOO_VER_MINOR}
	FOO_VER_BUILD=${FOO_VER_BUILD}
)
install(TARGETS ${foo}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static/)
muggle_install_headers(${CMAKE_CURRENT_LIST_DIR}/src include/${foo}/${FOO_VERSION})
