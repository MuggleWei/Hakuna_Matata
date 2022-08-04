cmake_minimum_required (VERSION 3.0.2)
project(event_loop)

# print compiler
message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

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

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

################################
# mugglec

# Download and unpack mugglec at configure time
configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/cmake/mugglec.cmake.in
        ${FETCHCONTENT_BASE_DIR}/mugglec-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/mugglec-download)
if (result)
        message(FATAL_ERROR "cmake step for mugglec failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/mugglec-download)
if (result)
        message(FATAL_ERROR "build step for mugglec failed: ${result}")
endif()

# add mugglec to build
add_subdirectory(
        ${FETCHCONTENT_BASE_DIR}/mugglec-src
        ${FETCHCONTENT_BASE_DIR}/mugglec-build)

# set mugglec compile options
set(MUGGLE_BUILD_TRACE OFF CACHE BOOL "")
set(MUGGLE_BUILD_SHARED_LIB ON CACHE BOOL "")
set(MUGGLE_BUILD_STATIC_PIC ON CACHE BOOL "")
set(MUGGLE_BUILD_BENCHMARK OFF CACHE BOOL "")
set(MUGGLE_BUILD_TESTING OFF CACHE BOOL "")
set(MUGGLE_BUILD_EXAMPLE OFF CACHE BOOL "")

################################
# project

set(muggle_c mugglec)

include(${FETCHCONTENT_BASE_DIR}/mugglec-src/cmake/muggle_utils.cmake)

# functions
function(add_example name folder)
	message("add example ${name} ${folder}")
	
	set(name example_${name})

	file(GLOB tmp_h ${folder}/*.h)
	file(GLOB tmp_c ${folder}/*.c)
	file(GLOB tmp_cpp ${folder}/*.cpp)
	file(GLOB tmp_cc ${folder}/*.cc)

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
	add_dependencies(${name} ${muggle_c})
	target_link_libraries(${name} ${muggle_c})
	target_include_directories(${name} PUBLIC
		${FETCHCONTENT_BASE_DIR}/mugglec-src)
	target_compile_definitions(${name} PUBLIC MUGGLE_HOLD_LOG_MACRO)
endfunction()

message("---------------------- example ----------------------")
SUBDIRLIST(example_dirs ${CMAKE_CURRENT_LIST_DIR}/example)
FOREACH(subdir ${example_dirs})
	add_example(${subdir} ${CMAKE_CURRENT_LIST_DIR}/example/${subdir})
ENDFOREACH()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	SUBDIRLIST(example_linux_dirs ${CMAKE_CURRENT_LIST_DIR}/example_linux)
	FOREACH(subdir ${example_linux_dirs})
		add_example(${subdir} ${CMAKE_CURRENT_LIST_DIR}/example_linux/${subdir})
	ENDFOREACH()
endif()
