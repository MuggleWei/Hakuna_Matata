cmake_minimum_required (VERSION 3.18.6)
project(hide_symbol)

################################
# general config
################################

message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

# set compile parameter
if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (${CMAKE_C_COMPILER_ID} MATCHES Clang)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

message("-- c compiler support features: ")
foreach(feature ${CMAKE_C_COMPILE_FEATURES})
	message("support feature: ${feature}")
endforeach()

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# for lsp
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################
# deps
################################
find_package(bar REQUIRED)
if (bar_FOUND)
	message("Found bar: ${bar_DIR}")
else()
	message(FATAL_ERROR "failed found bar")
endif()

find_package(foo REQUIRED)
if (foo_FOUND)
	message("Found foo: ${foo_DIR}")
else()
	message(FATAL_ERROR "failed found foo")
endif()

################################
# project
################################

set(BAZ_VERSION "1.0.0")
set(BAZ_SOVERSION "1")

FILE(GLOB_RECURSE baz_src_c "${CMAKE_CURRENT_LIST_DIR}/baz/*.c")
add_executable(baz ${baz_src_c})
target_include_directories(baz PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(baz PRIVATE bar foo)
set_target_properties(baz PROPERTIES
	LINKER_LANGUAGE C
	DEBUG_POSTFIX d
	VERSION ${BAZ_VERSION}
	SOVERSION ${BAZ_SOVERSION})
set_target_properties(baz
	PROPERTIES
	INSTALL_RPATH "\$ORIGIN/../lib"
	)

# install
include(GNUInstallDirs)
install(TARGETS baz
	EXPORT bazTargets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(IMPORTED_RUNTIME_ARTIFACTS bar)
install(IMPORTED_RUNTIME_ARTIFACTS foo)
