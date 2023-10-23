cmake_minimum_required(VERSION 3.18.6)
project(pat)

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

find_package(alice 1 REQUIRED)
if (alice_FOUND)
	message("found alice: ${alice_DIR}")
else()
	message(FATAL_ERROR "Failed found alice")
endif()

include(${alice_DIR}/FindFoo.cmake)

add_executable(pat pat/pat.c)
target_link_libraries(pat alice)

# install
if (APPLE)
	set_target_properties(pat
		PROPERTIES
		INSTALL_RPATH "@executable_path/../lib"
	)
elseif (UNIX)
	set_target_properties(pat
		PROPERTIES
		INSTALL_RPATH "\$ORIGIN/../lib"
	)
endif()
install(TARGETS pat
	RUNTIME DESTINATION bin)
