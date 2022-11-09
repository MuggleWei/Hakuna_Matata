cmake_minimum_required (VERSION 3.0.2)
project(read_boot_info)

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

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FindPackageHandleStandardArgs)

###############################
# mugglec
find_path(MUGGLEC_INCLUDE_DIR
	NAMES muggle/c/muggle_c.h
	PATH_SUFFIXES include
	PATHS ${CMAKE_PREFIX_PATH})

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWER_BUILD_TYPE)
if (${LOWER_BUILD_TYPE} STREQUAL "debug")
	message("try find mugglec libs with debug mode")
	find_library(MUGGLEC_LIBRARIES
		NAMES mugglecd
		PATH_SUFFIXES lib
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH)
else()
	message("try find mugglec libs with release mode")
	find_library(MUGGLEC_LIBRARIES
		NAMES mugglec
		PATH_SUFFIXES lib
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH)
endif()

find_package_handle_standard_args(MUGGLEC REQUIRED_VARS
	MUGGLEC_INCLUDE_DIR
	MUGGLEC_LIBRARIES
)
if (MUGGLEC_FOUND)
	message("find mugglec include dir: ${MUGGLEC_INCLUDE_DIR}")
	message("find mugglec libraries: ${MUGGLEC_LIBRARIES}")
else()
	message(FATAL_ERROR "failed found mugglec")
endif()

###############################
# libblkid
find_path(BLKID_INCLUDE_DIR
	NAMES blkid/blkid.h
	PATH_SUFFIXES include
	NO_CMAKE_ENVIRONMENT_PATH
	NO_SYSTEM_ENVIRONMENT_PATH)
find_library(BLKID_LIBRARIES
	NAMES blkid
	PATH_SUFFIXES lib
	NO_CMAKE_ENVIRONMENT_PATH
	NO_SYSTEM_ENVIRONMENT_PATH)
find_package_handle_standard_args(BLKID REQUIRED_VARS
	BLKID_INCLUDE_DIR
	BLKID_LIBRARIES)
if (BLKID_FOUND)
	message("find blkid include dir: ${BLKID_INCLUDE_DIR}")
	message("find blkid libraries: ${BLKID_LIBRARIES}")
else()
	message(${BLKID_INCLUDE_DIR})
	message(${BLKID_LIBRARIES})
	message(FATAL_ERROR "failed found blkid")
endif()

#------------------------------

# include cmake
include(${CMAKE_CURRENT_LIST_DIR}/_deps/mugglec-v1.0.0-alpha.3/cmake/muggle_utils.cmake)

#------------------------------

# function
function(add_example name folder)
	message("add example ${name} ${folder}")
	
	set(name example_${name})

	file(GLOB_RECURSE tmp_h ${folder}/*.h)
	file(GLOB_RECURSE tmp_c ${folder}/*.c)
	file(GLOB_RECURSE tmp_cpp ${folder}/*.cpp)
	file(GLOB_RECURSE tmp_cc ${folder}/*.cc)

	if (WIN32)
		add_executable(${name} ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		set_target_properties(${name}
			PROPERTIES
			FOLDER "example"
			VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
		)
	else()
		add_executable(${name} ${tmp_c} ${tmp_cpp} ${tmp_cc})
	endif(WIN32)
	target_include_directories(${name} PUBLIC
		${folder}
		${MUGGLEC_INCLUDE_DIR}
		${BLKID_INCLUDE_DIR}
	)
	target_link_libraries(${name} ${MUGGLEC_LIBRARIES} ${BLKID_LIBRARIES})

	target_compile_definitions(${name} PUBLIC MUGGLE_HOLD_LOG_MACRO)
endfunction()

# examples
SUBDIRLIST(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/example)
FOREACH(subdir ${example_root_dir})
	add_example(${subdir} ${CMAKE_CURRENT_LIST_DIR}/example/${subdir})
ENDFOREACH()
