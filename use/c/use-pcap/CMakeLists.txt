cmake_minimum_required (VERSION 3.0.2)
project(use_pcap)

# set compile parameter
if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra -Wno-unused-parameter")
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

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# set FetchContent base dir
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

# set mugglec compile options
set(MUGGLE_BUILD_TRACE OFF CACHE BOOL "")
set(MUGGLE_BUILD_SHARED_LIB ON CACHE BOOL "")
set(MUGGLE_BUILD_STATIC_PIC ON CACHE BOOL "")
set(MUGGLE_BUILD_BENCHMARK OFF CACHE BOOL "")
set(MUGGLE_BUILD_TESTING OFF CACHE BOOL "")
set(MUGGLE_BUILD_EXAMPLE OFF CACHE BOOL "")

# add mugglec to build
add_subdirectory(
	${FETCHCONTENT_BASE_DIR}/mugglec-src
	${FETCHCONTENT_BASE_DIR}/mugglec-build)

################################
# libpcap

# Download and unpack libpcap at configure time
configure_file(
	${CMAKE_CURRENT_LIST_DIR}/cmake/libpcap.cmake.in
	${FETCHCONTENT_BASE_DIR}/libpcap-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/libpcap-download)
if (result)
	message(FATAL_ERROR "cmake step for libpcap failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/libpcap-download)
if (result)
	message(FATAL_ERROR "build step for libpcap failed: ${result}")
endif()

# add mugglec to build
add_subdirectory(
	${FETCHCONTENT_BASE_DIR}/libpcap-src
	${FETCHCONTENT_BASE_DIR}/libpcap-build)

################################
# functions

# cmake
include(${FETCHCONTENT_BASE_DIR}/mugglec-src/cmake/muggle_utils.cmake)

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
	target_link_libraries(${name} mugglec pcap)
	add_dependencies(${name} mugglec pcap)
	target_include_directories(${name} PUBLIC
		${FETCHCONTENT_BASE_DIR}/mugglec-src
		${FETCHCONTENT_BASE_DIR}/libpcap-src
	)
	target_compile_definitions(${name} PRIVATE
		MUGGLE_HOLD_LOG_MACRO=1
	)
endfunction()

################################
# projects

message("---------------------- examples ----------------------")
SUBDIRLIST(example_dirs ${CMAKE_CURRENT_LIST_DIR}/examples)
FOREACH(subdir ${example_dirs})
	add_example(${subdir} ${CMAKE_CURRENT_LIST_DIR}/examples/${subdir})
ENDFOREACH()
