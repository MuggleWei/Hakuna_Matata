cmake_minimum_required (VERSION 3.0.2)
project(app)

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

include(FindPackageHandleStandardArgs)

# find foo library
find_path(FOO_INCLUDE_DIR
	NAMES foo/foo.h
	PATH_SUFFIXES include
	PATHS ${CMAKE_PREFIX_PATH})
find_library(FOO_LIBRARIES
	NAMES foo
	PATH_SUFFIXES lib
	NO_CMAKE_ENVIRONMENT_PATH
	NO_SYSTEM_ENVIRONMENT_PATH)

message(${FOO_INCLUDE_DIR})
message(${FOO_LIBRARIES})

find_package_handle_standard_args(FOO REQUIRED_VARS
	FOO_INCLUDE_DIR
	FOO_LIBRARIES)

if (FOO_FOUND)
	message("find foo include dir:  ${FOO_INCLUDE_DIR}")
	message("find foo libraries: ${FOO_LIBRARIES}")
else()
	message(FATAL_ERROR "failed found foo library")
endif()

# find bar library
find_path(BAR_INCLUDE_DIR
	NAMES bar/bar.h
	PATH_SUFFIXES include
	PATHS ${CMAKE_PREFIX_PATH})
find_library(BAR_LIBRARIES
	NAMES bar
	PATH_SUFFIXES lib
	NO_CMAKE_ENVIRONMENT_PATH
	NO_SYSTEM_ENVIRONMENT_PATH)

message(${BAR_INCLUDE_DIR})
message(${BAR_LIBRARIES})

find_package_handle_standard_args(BAR REQUIRED_VARS
	BAR_INCLUDE_DIR
	BAR_LIBRARIES)

if (BAR_FOUND)
	message("find bar include dir:  ${BAR_INCLUDE_DIR}")
	message("find bar libraries: ${BAR_LIBRARIES}")
else()
	message(FATAL_ERROR "failed found bar library")
endif()

# app
FILE(GLOB tmp_h ${CMAKE_CURRENT_LIST_DIR}/*.h)
FILE(GLOB tmp_c ${CMAKE_CURRENT_LIST_DIR}/*.c)
message(${tmp_c})
add_executable(app ${tmp_c})
if (NOT WIN32)
	set_target_properties(app
		PROPERTIES
		INSTALL_RPATH "\$ORIGIN/../lib"
	)
endif()
set_target_properties(app PROPERTIES
    VERSION 1.0.0
    SOVERSION 1)
target_include_directories(app PUBLIC
    ${FOO_INCLUDE_DIR}
    ${BAR_INCLUDE_DIR}
)
target_link_libraries(app
    ${FOO_LIBRARIES}
    ${BAR_LIBRARIES}
)
install(TARGETS app
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static)
