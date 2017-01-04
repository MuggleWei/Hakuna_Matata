cmake_minimum_required (VERSION 2.8.9)
project (JustCode)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# platform macro
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	add_definitions(-DMG_PLATFORM_UNIX=1)
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
	add_definitions(-DMG_PLATFORM_OSX=1)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	add_definitions(-DMG_PLATFORM_WINDOWS=1)
endif()

include_directories(
    inc/
)

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# timer
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_timer
    inc/timer.h
)
else()
set(HEADERS_timer)
endif()
set(SOURCES_timer
    run_test/run_test_timer.cpp
)
add_executable(timer
    ${HEADERS_timer}
    ${SOURCES_timer}
)
# end timer

# memory_aligned
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_memory_aligned
    inc/memory_aligned.h
)
else()
set(HEADERS_memory_aligned)
endif()
set(SOURCES_memory_aligned
    src/memory_aligned.c
    run_test/run_test_memory_aligned.c
)
add_executable(memory_aligned
    ${HEADERS_memory_aligned}
    ${SOURCES_memory_aligned}
)
# end memory_aligned

# memory_pool
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_memory_pool
    inc/memory_pool.h
)
else()
set(HEADERS_memory_pool)
endif()
set(SOURCES_memory_pool
	src/memory_pool.c
    run_test/run_test_memory_pool.cpp
)
add_executable(memory_pool
    ${HEADERS_memory_pool}
    ${SOURCES_memory_pool}
)
# end memory_pool

# list_memory_pool
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_list_memory_pool
    inc/list_memory_pool.h
)
else()
set(HEADERS_list_memory_pool)
endif()
set(SOURCES_list_memory_pool
    run_test/run_test_list_memory_pool.cpp
)
add_executable(list_memory_pool
    ${HEADERS_list_memory_pool}
    ${SOURCES_list_memory_pool}
)
# end list_memory_pool

# list_memory_pool_use_index
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_list_memory_pool_use_index
    inc/list_memory_pool_use_index.h
)
else()
set(HEADERS_list_memory_pool_use_index)
endif()
set(SOURCES_list_memory_pool_use_index
    run_test/run_test_list_memory_pool_use_index.cpp
)
add_executable(list_memory_pool_use_index
    ${HEADERS_list_memory_pool_use_index}
    ${SOURCES_list_memory_pool_use_index}
)
# end list_memory_pool_use_index

# ptr_hide_data
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_ptr_hide_data
    inc/memory_aligned.h
    inc/ptr_hide_data.h
)
else()
set(HEADERS_ptr_hide_data)
endif()
set(SOURCES_ptr_hide_data
    src/memory_aligned.c
    run_test/run_test_ptr_hide_data.c
)
add_executable(ptr_hide_data
    ${HEADERS_ptr_hide_data}
    ${SOURCES_ptr_hide_data}
)
# end ptr_hide_data

# v_table_exploer.cpp
add_executable(v_table_exploer
    src/v_table_exploer.cpp
)
# end ptr_hide_data

# memory_check
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_memory_check
    inc/memory_check.h
)
else()
set(HEADERS_memory_check)
endif()
set(SOURCES_memory_check
    run_test/run_test_memory_check.cpp
)
add_executable(memory_check
    ${HEADERS_memory_check}
    ${SOURCES_memory_check}
)
# end memory_check

# delegate
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_delegate
    inc/delegate.h
)
else()
set(HEADERS_delegate)
endif()
set(SOURCES_delegate
    run_test/run_test_delegate.cpp
)
add_executable(delegate
    ${HEADERS_delegate}
    ${SOURCES_delegate}
)
# end delegate

# ref_count
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_ref_count
    inc/ref_count.h
)
else()
set(HEADERS_ref_count)
endif()
set(SOURCES_ref_count
    run_test/run_test_ref_count.cpp
)
add_executable(ref_count
    ${HEADERS_ref_count}
    ${SOURCES_ref_count}
)
# end ref_count

# enum string
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_enum_string
    inc/enum_string.h
)
else()
set(HEADERS_enum_string)
endif()
set(SOURCES_enum_string
	src/enum_string.c
    run_test/run_test_enum_string.c
)
add_executable(enum_string
    ${HEADERS_enum_string}
    ${SOURCES_enum_string}
)
# end enum sting 

# enum string struct
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_enum_string_struct
    inc/enum_string_struct.h
)
else()
set(HEADERS_enum_string_struct)
endif()
set(SOURCES_enum_string_struct
	src/enum_string_struct.cpp
    run_test/run_test_enum_string_struct.cpp
)
add_executable(enum_string_struct
    ${HEADERS_enum_string_struct}
    ${SOURCES_enum_string_struct}
)
# end enum string struct

# enum command utils
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_cmd_utils
    inc/cmd_utils.h
	inc/base.h
	inc/str_utils.h
)
else()
set(HEADERS_cmd_utils)
endif()
set(SOURCES_cmd_utils
	src/cmd_utils.c
	src/base.c
	src/str_utils.c
    run_test/run_test_cmd_utils.c
)
add_executable(command_utils
    ${HEADERS_cmd_utils}
    ${SOURCES_cmd_utils}
)
set_target_properties(command_utils PROPERTIES 
	COMPILE_DEFINITIONS MG_USE_LIB)
# end command utils

# adapter
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
add_executable(adapter
	inc/adapter.h
	src/adapter.cpp
	run_test/run_test_adapter.cpp
)
else()
set(adapter
	src/adapter.cpp
	run_test/run_test_adapter.cpp
)
endif()
# end adapter

# singleton
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
add_executable(singleton
	inc/singleton.h
	inc/scope_lock.h
	run_test/run_test_singleton.cpp
)
else()
add_executable(singleton
	run_test/run_test_singleton.cpp
)
endif()
# end singleton