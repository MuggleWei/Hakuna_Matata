cmake_minimum_required (VERSION 3.18.6)
project(openssl_c_example)

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

#------------------------------
# dependencies
message("------------------------------")
message("find dependencies")
message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

###############################
# openssl
find_package(OpenSSL)
if (OPENSSL_FOUND)
	message("openssl version: ${OPENSSL_VERSION}")
	message("openssl include dir: ${OPENSSL_INCLUDE_DIR}")
	message("openssl crypto lib: ${OPENSSL_CRYPTO_LIBRARY}")
	message("openssl ssl lib: ${OPENSSL_SSL_LIBRARY}")
else()
	message(FATAL_ERROR "Can't find openssl")
endif()

###############################
# mugglec
find_package(mugglec)
if (mugglec_FOUND)
	message("find mugglec dir: ${mugglec_DIR}")
else()
    message(FATAL_ERROR "failed found mugglec")
endif()

#------------------------------

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

	target_include_directories(${name} PUBLIC
		${PROJECT_BINARY_DIR}
		${OPENSSL_INCLUDE_DIR}
		${folder}
	)
	target_link_libraries(${name}
		mugglec
		${OPENSSL_LIBRARIES})

	target_compile_definitions(${name}
		PUBLIC MUGGLE_HOLD_LOG_MACRO)

	# install
	install(TARGETS ${name}
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib/static
	)
endfunction()

#------------------------------

# examples
message("------------------------------")
set(example_dirs "${CMAKE_CURRENT_LIST_DIR}/example")
FILE(GLOB subfiles RELATIVE ${example_dirs} ${example_dirs}/*)
FOREACH(subfile ${subfiles})
	set(subdir ${example_dirs}/${subfile})
    IF(IS_DIRECTORY ${subdir})
		add_example(${subfile} ${subdir})
    ENDIF()
ENDFOREACH()
