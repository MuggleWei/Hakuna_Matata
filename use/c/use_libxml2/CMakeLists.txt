cmake_minimum_required (VERSION 3.0.2)
project(use_libxml2)

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

# deps
find_package(libxml2)
if (NOT libxml2_FOUND)
	message(FATAL_ERR "failed found libxml2")
endif()

find_package(mugglec)
if (NOT mugglec_FOUND)
	message(FATAL_ERR "failed found mugglec")
endif()

# functions
function(add_example name folder)
	message("add example ${name} ${folder}")
	
	set(name example_${name})

	file(GLOB_RECURSE tmp_h ${folder}/*.h)
	file(GLOB_RECURSE tmp_c ${folder}/*.c)
	file(GLOB_RECURSE tmp_cpp ${folder}/*.cpp)
	file(GLOB_RECURSE tmp_cc ${folder}/*.cc)

	if (MSVC OR MINGW)
		add_executable(${name} ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		set_target_properties(${name}
			PROPERTIES
			FOLDER "example"
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
		endif()
	endif()
	target_compile_definitions(${name} PUBLIC MUGGLE_HOLD_LOG_MACRO)
	target_include_directories(${name} PUBLIC ${folder} ${LIBXML2_INCLUDE_DIR})
	target_link_libraries(${name} mugglec xml2)
endfunction()

# examples
set(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/example)
FILE(GLOB subdirs RELATIVE ${example_root_dir} ${example_root_dir}/*)
FOREACH(subdir ${subdirs})
	IF(IS_DIRECTORY ${example_root_dir}/${subdir})
		add_example(${subdir} ${example_root_dir}/${subdir})
	ENDIF()
ENDFOREACH()

# copy resources
set(res_src_dir ${CMAKE_CURRENT_LIST_DIR}/etc)
set(res_dst_dir ${CMAKE_BINARY_DIR}/etc)
set(dst_files)
FILE(GLOB_RECURSE res_files "${res_src_dir}/*")
FOREACH(src_file ${res_files})
	file(RELATIVE_PATH rel_path ${res_src_dir} ${src_file})
	set(dst_file "${res_dst_dir}/${rel_path}")
	add_custom_command(
		OUTPUT "${dst_file}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${src_file}" "${dst_file}"
		DEPENDS "${src_file}"
		COMMENT "copy ${src_file} to ${dst_file}"
	)
	list(APPEND dst_files "${dst_file}")
ENDFOREACH()
add_custom_target(COPY_RES ALL DEPENDS ${dst_files})
