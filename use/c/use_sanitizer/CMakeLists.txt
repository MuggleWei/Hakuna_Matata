cmake_minimum_required (VERSION 3.18.0)
project(use_sanitizer)

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
	endif(WIN32)
	target_include_directories(${name} PUBLIC ${folder})

	install(TARGETS ${name} RUNTIME DESTINATION bin)
endfunction()

# examples - Undefined Behavior
set(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/example)
FILE(GLOB san_dirs RELATIVE ${example_root_dir} ${example_root_dir}/*)
FOREACH(san_dir ${san_dirs})
	set(san_root_dir ${example_root_dir}/${san_dir})
	FILE(GLOB subdirs RELATIVE ${san_root_dir} ${san_root_dir}/*)

	FOREACH(subdir ${subdirs})
		add_example(${subdir} ${san_root_dir}/${subdir})
		if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
			(CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
			if (${san_dir} STREQUAL "ubsan")
				target_compile_options(example_${subdir} PRIVATE -fsanitize=undefined)
				target_link_options(example_${subdir} PRIVATE -fsanitize=undefined)
			elseif (${san_dir} STREQUAL "leak")
				target_compile_options(example_${subdir} PRIVATE -fsanitize=leak)
				target_link_options(example_${subdir} PRIVATE -fsanitize=leak)
			elseif (${san_dir} STREQUAL "address")
				target_compile_options(example_${subdir} PRIVATE -fsanitize=address)
				target_link_options(example_${subdir} PRIVATE -fsanitize=address)
			elseif (${san_dir} STREQUAL "memory")
				if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
					target_compile_options(example_${subdir} PRIVATE -fsanitize=memory)
					target_link_options(example_${subdir} PRIVATE -fsanitize=memory)
				else()
					message(WARNING "only Clang support -fsantitize=memory")
				endif()
			endif()
		endif()
	ENDFOREACH()
ENDFOREACH()
