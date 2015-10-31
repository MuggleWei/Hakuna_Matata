cmake_minimum_required (VERSION 2.8.9)
project (JustCode)

include_directories(
	inc/
)

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
	run_test/unit_test_memory_aligned.c
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
	run_test/unit_test_memory_pool.cpp
)
add_executable(memory_pool
	${HEADERS_memory_pool}
	${SOURCES_memory_pool}
)
# end memory_pool

# memory_pool_use_index
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_memory_pool
	inc/memory_pool_use_index.h
)
else()
set(HEADERS_memory_pool_use_index)
endif()
set(SOURCES_memory_pool_use_index
	run_test/unit_test_memory_pool_use_index.cpp
)
add_executable(memory_pool_use_index
	${HEADERS_memory_pool_use_index}
	${SOURCES_memory_pool_use_index}
)
# end memory_pool_use_index

# ptr_hide_data
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
set(HEADERS_memory_aligned
	inc/memory_aligned.h
	inc/ptr_hide_data.h
)
else()
set(HEADERS_ptr_hide_data)
endif()
set(SOURCES_ptr_hide_data
	src/memory_aligned.c
	run_test/unit_test_ptr_hide_data.c
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