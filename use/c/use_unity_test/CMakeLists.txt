cmake_minimum_required(VERSION 3.18.6)
project(use_unity_test)

################################
# general config
################################

# print compiler
message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

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

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################
# foo
################################

# configure
set(FOO_VERSION_MAJOR 1)
set(FOO_VERSION_MINOR 0)
set(FOO_VERSION_PATCH 0)
set(FOO_VERSION "${FOO_VERSION_MAJOR}.${FOO_VERSION_MINOR}.${FOO_VERSION_PATCH}")

if (BUILD_SHARED_LIBS)
	set(FOO_USE_DLL ON)
	set(FOO_LIB_TYPE SHARED)
else()
	set(FOO_USE_DLL OFF)
	set(FOO_LIB_TYPE STATIC)
endif()

set(foo_src_dir ${CMAKE_CURRENT_LIST_DIR}/foo)
set(foo_gen_dir ${CMAKE_CURRENT_BINARY_DIR}/generated)
configure_file(
	${foo_src_dir}/foo_config.h.in
	${foo_gen_dir}/foo/foo_config.h)

# add library
FILE(GLOB_RECURSE foo_src_c "${foo_src_dir}/*.c")
FILE(GLOB_RECURSE foo_src_h "${foo_src_dir}/*.h")
FILE(GLOB_RECURSE foo_gen_h "${foo_gen_dir}/*.h")
add_library(foo ${FOO_LIB_TYPE}
	${foo_src_c}
	${foo_src_h}
	${foo_gen_h})
target_include_directories(foo PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<BUILD_INTERFACE:${foo_gen_dir}>
	$<INSTALL_INTERFACE:include>)

target_compile_definitions(foo PRIVATE FOO_EXPORTS)
set_target_properties(foo PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	VERSION ${FOO_VERSION}
	SOVERSION ${FOO_VERSION_MAJOR})

################################
# test
################################

# find unitest library
find_package(unity REQUIRED)
if (NOT unity_FOUND)
	message(FATAL_ERROR "Failed found Unity Test")
else()
	message("Found Unity Test: ${unity_DIR}")
endif()

# functions
function(add_unittest name folder)
	message("add test ${name} ${folder}")

	file(GLOB tmp_h ${folder}/*.h)
	file(GLOB tmp_c ${folder}/*.c)
	file(GLOB tmp_cpp ${folder}/*.cpp)
	file(GLOB tmp_cc ${folder}/*.cc)

	if (MSVC OR MINGW)
		add_executable(${name} ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		set_target_properties(${name}
			PROPERTIES
			FOLDER "test"
			VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
		)
	else()
		add_executable(${name} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		if (APPLE)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "@executable_path/../lib"
			)
		elseif (UNIX)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "\$ORIGIN/../lib"
			)
		endif(APPLE)
	endif(MSVC OR MINGW)
	add_dependencies(${name} foo)
	target_include_directories(${name} PUBLIC
		${foo_src_dir}
		${foo_gen_dir})
	target_link_libraries(${name} foo unity::framework)

	add_test(NAME ${name} COMMAND ${name})
endfunction()

# tests
if (BUILD_TESTING)
	enable_testing()

	set(test_root_dir ${CMAKE_CURRENT_LIST_DIR}/tests)
	FILE(GLOB subdirs RELATIVE ${test_root_dir} ${test_root_dir}/*)
	FOREACH(subdir ${subdirs})
		IF(IS_DIRECTORY ${test_root_dir}/${subdir})
			add_unittest(${subdir} ${test_root_dir}/${subdir})
		ENDIF()
	ENDFOREACH()
endif()
