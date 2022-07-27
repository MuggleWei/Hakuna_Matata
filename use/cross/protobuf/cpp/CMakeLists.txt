cmake_minimum_required (VERSION 3.0.2)
project(proto_cpp)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

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

# find protobuf
message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindProtobufBin.cmake)
if (ProtobufBin_FOUND)
	message("protobuf include dir: ${PROTOBUF_INCLUDE_DIR}")
	message("protobuf libs: ${PROTOBUF_LIBS}")
else()
	message(FATAL_ERROR "Can't find protobuf binary")
endif()

# config fetch directory
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

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

set(MUGGLE_BUILD_TRACE OFF CACHE BOOL "")
set(MUGGLE_BUILD_SHARED_LIB ON CACHE BOOL "")
set(MUGGLE_BUILD_STATIC_PIC ON CACHE BOOL "")
set(MUGGLE_BUILD_BENCHMARK OFF CACHE BOOL "")
set(MUGGLE_BUILD_TESTING OFF CACHE BOOL "")
set(MUGGLE_BUILD_EXAMPLE OFF CACHE BOOL "")

add_subdirectory(
	${FETCHCONTENT_BASE_DIR}/mugglec-src
	${FETCHCONTENT_BASE_DIR}/mugglec-build)
set(mugglec mugglec)

# include cmake
include(${FETCHCONTENT_BASE_DIR}/mugglec-src/cmake/muggle_utils.cmake)

#############################
# protobuf message handle
set(msg_handle protobuf_msg_handle)
muggle_add_project(${msg_handle} ${CMAKE_CURRENT_LIST_DIR}/src/msg_handle STATIC)
add_dependencies(${msg_handle} ${mugglec})
target_include_directories(${msg_handle} PUBLIC
	${CMAKE_BINARY_DIR}
	${FETCHCONTENT_BASE_DIR}/mugglec-src
	${PROTOBUF_INCLUDE_DIR})
target_link_libraries(${msg_handle} PUBLIC ${mugglec} ${PROTOBUF_LIBS})
if (WIN32)
	target_compile_definitions(${msg_handle} PUBLIC PROTOBUF_USE_DLLS)
endif()
set_target_properties(${msg_handle} PROPERTIES 
	DEBUG_POSTFIX d
)

install(TARGETS ${msg_handle}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static
)

#############################
# protobuf msg
set(protomsg protomsg)
muggle_add_project(${protomsg} ${CMAKE_BINARY_DIR}/gen/proto STATIC)
target_include_directories(${protomsg} PUBLIC ${CMAKE_BINARY_DIR} ${PROTOBUF_INCLUDE_DIR})
target_link_libraries(${protomsg} PUBLIC ${PROTOBUF_LIBS})
if (WIN32)
	target_compile_definitions(${protomsg} PUBLIC PROTOBUF_USE_DLLS)
endif()
set_target_properties(${protomsg} PROPERTIES 
	DEBUG_POSTFIX d
)

install(TARGETS ${protomsg}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib/static
)

# functions
function(add_example name folder)
	message("add example ${name} ${folder}")
	
	set(name cpp_example_${name})

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

	target_include_directories(${name} PUBLIC
		${PROJECT_BINARY_DIR}
		${FETCHCONTENT_BASE_DIR}/mugglec-src
		${CMAKE_CURRENT_LIST_DIR}/src
	)

	add_dependencies(${name} ${muggle_c} ${msg_handle} ${protomsg})
	target_link_libraries(${name} ${muggle_c} ${msg_handle} ${protomsg})

	target_compile_definitions(${name} PUBLIC MUGGLE_HOLD_LOG_MACRO)

	# install
	install(TARGETS ${name}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
	)
endfunction()

SUBDIRLIST(example_dirs ${CMAKE_CURRENT_LIST_DIR}/example)
FOREACH(subdir ${example_dirs})
	add_example(${subdir} ${CMAKE_CURRENT_LIST_DIR}/example/${subdir})
ENDFOREACH()
