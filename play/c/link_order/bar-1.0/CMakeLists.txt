cmake_minimum_required (VERSION 3.0.2)
project(bar)

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

# find base library
include(FindPackageHandleStandardArgs)
find_path(BASE_INCLUDE_DIR
	NAMES base/base.h
	PATH_SUFFIXES include
	PATHS ${CMAKE_PREFIX_PATH})
find_library(BASE_LIBRARIES
	NAMES base
	PATH_SUFFIXES lib/static
	NO_CMAKE_ENVIRONMENT_PATH
	NO_SYSTEM_ENVIRONMENT_PATH)

message(${BASE_INCLUDE_DIR})
message(${BASE_LIBRARIES})

find_package_handle_standard_args(BASE REQUIRED_VARS
	BASE_INCLUDE_DIR
	BASE_LIBRARIES)

if (BASE_FOUND)
	message("find base include dir:  ${BASE_INCLUDE_DIR}")
	message("find base libraries: ${BASE_LIBRARIES}")
else()
	message(FATAL_ERROR "failed found base library")
endif()

# bar
FILE(GLOB tmp_h ${CMAKE_CURRENT_LIST_DIR}/*.h)
FILE(GLOB tmp_c ${CMAKE_CURRENT_LIST_DIR}/*.c)
add_library(bar SHARED ${tmp_c})
set_target_properties(bar PROPERTIES
	VERSION 1.0.0
	SOVERSION 1)
target_include_directories(bar PUBLIC ${BASE_INCLUDE_DIR})
target_link_libraries(bar ${BASE_LIBRARIES})

install(TARGETS bar
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static)
install(FILES ${tmp_h} DESTINATION include/bar)
