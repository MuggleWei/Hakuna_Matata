cmake_minimum_required (VERSION 3.18.6)
project(use_wolfssl)

################################
# general config
################################

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
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

# build flags
message("# BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("# BUILD_TESTING: ${BUILD_TESTING}")

################################
# dependencies
################################

include(FetchContent)

# set deps dir
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

# wolfssl
set(WOLFSSL_TLS13 ON CACHE BOOL "")
set(WOLFSSL_DTLS ON CACHE BOOL "")
set(WOLFSSL_DTLS13 ON CACHE BOOL "")

message("# Fetch wolfssl")
FetchContent_Declare(
	wolfssl
	GIT_REPOSITORY https://github.com/wolfSSL/wolfssl.git
	GIT_TAG v5.4.0-stable
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(wolfssl)

# mugglec
message("# Fetch mugglec")
FetchContent_Declare(
	mugglec
	GIT_REPOSITORY https://github.com/MuggleWei/mugglec.git
	GIT_TAG v1.2.4
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(mugglec)

################################
# examples
################################

include(GNUInstallDirs)

# functions
function(add_example name folder)
	message("add example ${name} ${folder}")
	
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
		if (APPLE)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "@executable_path/../lib")
		elseif (UNIX)
			set_target_properties(${name}
				PROPERTIES
				INSTALL_RPATH "\$ORIGIN/../lib")
		endif()
	endif(WIN32)

	target_include_directories(${name} PUBLIC
		${PROJECT_BINARY_DIR}
		${FETCHCONTENT_BASE_DIR}/mugglec-src
		${FETCHCONTENT_BASE_DIR}/wolfssl-src
	)
	target_link_libraries(${name} mugglec wolfssl)

	add_dependencies(${name} mugglec wolfssl)

	target_compile_definitions(${name} PUBLIC
		MUGGLE_HOLD_LOG_MACRO
		WOLFSSL_DTLS
		WOLFSSL_TLS13
		WOLFSSL_DTLS13)

	install(TARGETS ${name} 
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endfunction()

#------------------------------

# examples
set(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/example)
FILE(GLOB subdirs RELATIVE ${example_root_dir} ${example_root_dir}/*)
FOREACH(subdir ${subdirs})
	IF(IS_DIRECTORY ${example_root_dir}/${subdir})
		add_example(${subdir} ${example_root_dir}/${subdir})
	ENDIF()
ENDFOREACH()

#------------------------------

# copy config and certs
if (WIN32)
	add_custom_target(COPY_CONFIG ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/certs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/certs)
else()
	add_custom_target(COPY_CONFIG ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/certs ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/certs)
endif()
