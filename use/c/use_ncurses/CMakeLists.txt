cmake_minimum_required (VERSION 3.0.2)
project(ncurses_example)

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
# ncurses
include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindNcurses.cmake)
if (NCURSES_FOUND)
	message("find ncurses include dir: ${NCURSES_INCLUDE_DIR}")
	message("find ncurses lib: ${NCURSES_LIBRARIES}")
else()
	message(FATAL_ERROR "Can't find ncurses")
endif()

###############################
# mugglec
include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindMugglec.cmake)
if (MUGGLEC_FOUND)
	message("find mugglec include dir: ${MUGGLEC_INCLUDE_DIR}")
	message("find mugglec libraries: ${MUGGLEC_LIBRARIES}")
else()
	message(FATAL_ERROR "failed found mugglec")
endif()

#------------------------------

# include cmake
include(${CMAKE_CURRENT_LIST_DIR}/_deps/mugglec-v1.0.0-alpha.3/cmake/muggle_utils.cmake)

# threads
find_package(Threads)

#------------------------------

# functions
function(add_example name folder)
	message("add example ${name} ${folder}")
	
	set(name example_${name})

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
		set_target_properties(${name}
			PROPERTIES
			INSTALL_RPATH "\$ORIGIN/../lib"
		)
	endif(WIN32)

	target_include_directories(${name} PUBLIC
		${PROJECT_BINARY_DIR}
		${MUGGLEC_INCLUDE_DIR}
		${NCURSES_INCLUDE_DIR}
	)

	target_link_libraries(${name}
		${MUGGLEC_LIBRARIES}
		${NCURSES_LIBRARIES}
		${CMAKE_THREAD_LIBS_INIT}
	)

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
SUBDIRLIST(example_dirs ${CMAKE_CURRENT_LIST_DIR}/example)
FOREACH(subdir ${example_dirs})
	add_example(${subdir} ${CMAKE_CURRENT_LIST_DIR}/example/${subdir})
ENDFOREACH()
