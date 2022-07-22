cmake_minimum_required (VERSION 3.0.2)
project(lib_vars)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# set standard and print features
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# cmake
set(COMMON_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../common/cmake)
include(${COMMON_CMAKE_DIR}/muggle_utils.cmake)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# include directories
include_directories(
	${CMAKE_CURRENT_LIST_DIR}
)

# cache
set(foo_type STATIC CACHE STRING "lib foo type")
set(bar_type SHARED CACHE STRING "lib bar type")

# foo
set(foo foo)
message("build foo with type ${foo_type}")
muggle_add_project(${foo} ${CMAKE_CURRENT_LIST_DIR}/foo ${foo_type})
if (${foo_type} STREQUAL STATIC)
	set_target_properties(${foo} PROPERTIES
		POSITION_INDEPENDENT_CODE ON
	)
endif()

# bar
set(bar bar)
message("build bar with type ${bar_type}")
muggle_add_project(${bar} ${CMAKE_CURRENT_LIST_DIR}/bar ${bar_type})
if (${bar_type} STREQUAL STATIC)
	set_target_properties(${bar} PROPERTIES
		POSITION_INDEPENDENT_CODE ON
	)
endif()
target_link_libraries(${bar} ${foo})
add_dependencies(${bar} ${foo})

# hello
set(hello hello)
muggle_add_project(${hello} ${CMAKE_CURRENT_LIST_DIR}/hello EXE)
target_link_libraries(hello ${foo} ${bar})
add_dependencies(hello ${foo} ${bar})
