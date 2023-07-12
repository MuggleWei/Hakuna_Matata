cmake_minimum_required (VERSION 3.0.2)
project(mugglecpp_template)

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

# set fetch content
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

###################################
# mugglec & mugglecpp
FetchContent_Declare(
	mugglecpp
	GIT_REPOSITORY https://github.com/MuggleWei/mugglecpp.git
	GIT_TAG v1.0.6
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(mugglecpp)

include(${FETCHCONTENT_BASE_DIR}/mugglec-src/cmake/muggle_utils.cmake)

###################################
# functions
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
		set_target_properties(${name}
			PROPERTIES
			INSTALL_RPATH "\$ORIGIN/../lib"
		)
	endif(WIN32)
	add_dependencies(${name} mugglecpp)
	target_include_directories(${name} PUBLIC
		${folder}
		${FETCHCONTENT_BASE_DIR}/mugglecpp-src
		${FETCHCONTENT_BASE_DIR}/mugglecpp-build/generated
	)
	target_link_libraries(${name} mugglecpp)

	target_compile_definitions(${name} PUBLIC MUGGLE_HOLD_LOG_MACRO)

	install(TARGETS ${name} RUNTIME DESTINATION bin)
endfunction()

SUBDIRLIST(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/example)
FOREACH(subdir ${example_root_dir})
	add_example(${subdir} ${CMAKE_CURRENT_LIST_DIR}/example/${subdir})
ENDFOREACH()
