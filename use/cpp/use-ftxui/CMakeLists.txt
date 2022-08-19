cmake_minimum_required (VERSION 3.0.2)
project(use_ftxui)

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

# ========================
# dependencies
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

# ------------------------
# mugglec
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
set(MUGGLE_BUILD_SHARED_LIB OFF CACHE BOOL "")
set(MUGGLE_BUILD_STATIC_PIC ON CACHE BOOL "")
set(MUGGLE_BUILD_BENCHMARK OFF CACHE BOOL "")
set(MUGGLE_BUILD_TESTING OFF CACHE BOOL "")
set(MUGGLE_BUILD_EXAMPLE OFF CACHE BOOL "")

# add mugglec to build
add_subdirectory(
	${FETCHCONTENT_BASE_DIR}/mugglec-src
	${FETCHCONTENT_BASE_DIR}/mugglec-build)

# ------------------------
# ftxui
configure_file(
	${CMAKE_CURRENT_LIST_DIR}/cmake/ftxui.cmake.in
	${FETCHCONTENT_BASE_DIR}/ftxui-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/ftxui-download)
if (result)
	message(FATAL_ERROR "cmake step for ftxui failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${FETCHCONTENT_BASE_DIR}/ftxui-download)
if (result)
	message(FATAL_ERROR "build step for ftxui failed: ${result}")
endif()

# set ftxui compile options
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "")
set(FTXUI_BUILD_TESTS OFF CACHE BOOL "")

# add ftxui to build
add_subdirectory(
	${FETCHCONTENT_BASE_DIR}/ftxui-src
	${FETCHCONTENT_BASE_DIR}/ftxui-build)

# ========================
# functions
function(add_app name folder)
	message("add application ${name} ${folder}")

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
	target_include_directories(${name}
		PUBLIC ${FETCHCONTENT_BASE_DIR}/mugglec-src
		PUBLIC ${FETCHCONTENT_BASE_DIR}/ftxui-src/include
		PUBLIC ${CMAKE_CURRENT_LIST_DIR}/apps/${name}
	)
	add_dependencies(${name}
		mugglec
		ftxui::screen
		ftxui::dom
		ftxui::component)
	target_link_libraries(${name}
		PRIVATE mugglec
		PRIVATE ftxui::screen
		PRIVATE ftxui::dom
		PRIVATE ftxui::component)
	target_compile_definitions(${name} PUBLIC MUGGLE_HOLD_LOG_MACRO)
endfunction()

# ========================
# apps
SUBDIRLIST(app_root_dir ${CMAKE_CURRENT_LIST_DIR}/apps)
FOREACH(subdir ${app_root_dir})
	add_app(${subdir} ${CMAKE_CURRENT_LIST_DIR}/apps/${subdir})
ENDFOREACH()
