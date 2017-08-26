if (WIN32)
	cmake_minimum_required(VERSION 3.8.2)
else()
	cmake_minimum_required(VERSION 3.0.2)
endif()
project(use_libevent)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# include cmake
include(${CMAKE_CURRENT_LIST_DIR}/cmake/muggle_utils.cmake)

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# libevent
set(USE_LIBEVENT_WITH_SOURCE ON CACHE BOOL "use libevent with source, if it OFF, try to find libevent binary in system")

if (USE_LIBEVENT_WITH_SOURCE)
	include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindLibeventSrc.cmake)
	if(LibeventSrc_FOUND)
	
		SET(EVENT__DISABLE_OPENSSL ON CACHE BOOL "close libevent openssl")				# libevent close openssl in windows
		SET(EVENT__BUILD_SHARED_LIBRARIES ON CACHE BOOL "build libevent as shared")		# libevent build as static lib		
		SET(EVENT__DISABLE_SAMPLES ON CACHE BOOL "close libevent example")				# libevent close example
		SET(EVENT__DISABLE_TESTS ON CACHE BOOL "close libevent test")					# libevent close test
		SET(EVENT__DISABLE_BENCHMARK ON CACHE BOOL "close libevent benchmark")			# libevent close benchmark
		SET(EVENT__DISABLE_THREAD_SUPPORT OFF CACHE BOOL "open libevent thread")		# libevent open thread
		
		add_subdirectory(${LIBEVENT_SOURCE_DIR})
		
		include_directories(
			${LIBEVENT_INCLUDE_DIR}
			${libevent_BINARY_DIR}/include	# for event2/event-config.h
		)
		
	else()
		message(FATAL_ERROR "Can't find libevent source")
	endif()
else()
	include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindLibeventBin.cmake)
	if (LibeventBin_FOUND)
		include_directories(${LIBEVENT_INCLUDE_DIR})
	else()
		message(FATAL_ERROR "Can't find libevent binary")
	endif()
endif()

# include directories
include_directories(
	${CMAKE_CURRENT_LIST_DIR}/src
)

# add exe dependent on libevent
function(add_le_project name folder)
	muggle_add_project(${name} ${folder} EXE)
	target_link_libraries(${name}
		event_core
		event_extra
	)
	if (USE_LIBEVENT_WITH_SOURCE)
		add_dependencies(${name}
			event_core
			event_extra
		)
	endif()
	
	if (MSVC)
		set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)")
	endif()
endfunction()

# add projects
add_le_project(simple_client ${CMAKE_CURRENT_LIST_DIR}/src/simple_client)
add_le_project(echo_server ${CMAKE_CURRENT_LIST_DIR}/src/echo_server)
add_le_project(client ${CMAKE_CURRENT_LIST_DIR}/src/client)
add_le_project(time_server ${CMAKE_CURRENT_LIST_DIR}/src/time_server)

# add post compile event - copy libevent
if (MSVC AND USE_LIBEVENT_WITH_SOURCE)
	add_custom_target(FAKE
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${libevent_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/bin
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${libevent_BINARY_DIR}/lib ${CMAKE_BINARY_DIR}/lib
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${libevent_BINARY_DIR}/include ${CMAKE_BINARY_DIR}/include
	)
	add_dependencies(FAKE
		event
		event_core
		event_extra
	)
endif()