cmake_minimum_required (VERSION 3.0.2)
project(app)

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

# version
set(APP_VER_MAJOR 0)
set(APP_VER_MINOR 0)
set(APP_VER_BUILD 1)

set(APP_VERSION "${APP_VER_MAJOR}.${APP_VER_MINOR}.${APP_VER_BUILD}")
set(APP_SOVERSION "${APP_VER_MAJOR}")

# app
set(app app)
muggle_add_project(${app} ${CMAKE_CURRENT_LIST_DIR}/src EXE)
set_target_properties(${app} PROPERTIES
	LINKER_LANGUAGE C
	VERSION ${APP_VERSION}
	SOVERSION ${APP_SOVERSION}
)
target_compile_definitions(${app} PUBLIC
	APP_VER_MAJOR=${APP_VER_MAJOR}
	APP_VER_MINOR=${APP_VER_MINOR}
	APP_VER_BUILD=${APP_VER_BUILD}
)
target_include_directories(${app} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/src
)

# thirdparty
set(foo_soname 0)
set(foo_ver 0.0.1)

set(bar_soname 0)
set(bar_ver 0.0.1)

set(baz_soname 0)
set(baz_ver 0.0.1)

target_include_directories(${app} PUBLIC
	${CMAKE_INSTALL_PREFIX}/include/foo/${foo_ver}
	${CMAKE_INSTALL_PREFIX}/include/bar/${bar_ver}
	${CMAKE_INSTALL_PREFIX}/include/baz/${baz_ver}
)
if ("${LIB_TYPE}" STREQUAL "SHARED")
	find_library(foo REQUIRED
		NAMES libfoo.so.${foo_soname}
		PATHS ${CMAKE_INSTALL_PREFIX}/lib
	)

	find_library(bar REQUIRED
		NAMES libbar.so.${bar_soname}
		PATHS ${CMAKE_INSTALL_PREFIX}/lib
	)

	find_library(baz REQUIRED
		NAMES libbaz.so.${baz_soname}
		PATHS ${CMAKE_INSTALL_PREFIX}/lib
	)
	target_link_libraries(app
		${foo}
		${bar}
		${baz}
	)
else()
	find_library(foo REQUIRED
		NAMES libfoo.a
		PATHS ${CMAKE_INSTALL_PREFIX}/lib/static
	)
	find_library(bar REQUIRED
		NAMES libbar.a
		PATHS ${CMAKE_INSTALL_PREFIX}/lib/static
	)
	find_library(baz REQUIRED
		NAMES libbaz.a
		PATHS ${CMAKE_INSTALL_PREFIX}/lib/static
	)
	find_library(base REQUIRED
		NAMES libbase.a
		PATHS ${CMAKE_INSTALL_PREFIX}/lib/static
	)
	target_link_libraries(app
		${foo}
		${bar}
		${baz}
		${base}
	)
endif()

# install
target_compile_definitions(${app} PUBLIC
	BAR_VER_MAJOR=${BAR_VER_MAJOR}
	BAR_VER_MINOR=${BAR_VER_MINOR}
	BAR_VER_BUILD=${BAR_VER_BUILD}
)
install(TARGETS ${app}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static/)
muggle_install_headers(${CMAKE_CURRENT_LIST_DIR}/src include/${app}/${BAR_VERSION})
