cmake_minimum_required (VERSION 3.18.6)
project(use_odbc)

################################
# general config
################################
include(${CMAKE_CURRENT_LIST_DIR}/cmake/general.cmake)

################################
# find odbc
################################
find_package(ODBC)
if (ODBC_FOUND)
	message("Found ODBC include dir: ${ODBC_INCLUDE_DIRS}")
	message("Found ODBC libraries: ${ODBC_LIBRARIES}")
	message("Found ODBC config: ${ODBC_CONFIG}")
else()
	message(FATAL_ERROR "failed found odbc")
endif()

#######################################
# examples
#######################################
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
	target_include_directories(${name} PUBLIC ${folder} ${ODBC_INCLUDE_DIRS})
	target_link_libraries(${name} PUBLIC ODBC::ODBC)
endfunction()

set(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/example)
FILE(GLOB subdirs RELATIVE ${example_root_dir} ${example_root_dir}/*)
FOREACH(subdir ${subdirs})
	IF(IS_DIRECTORY ${example_root_dir}/${subdir})
		add_example(${subdir} ${example_root_dir}/${subdir})
	ENDIF()
ENDFOREACH()
