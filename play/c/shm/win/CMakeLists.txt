cmake_minimum_required (VERSION 3.17.6)
project(win_shm)

################################
# general config
################################

# print compiler
message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

# set compile parameter
if (NOT CMAKE_CROSSCOMPILING)
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

    # NOTE: by default, without -march=native
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
else()
    if (ANDROID)
        message("-- cmake run cross compiling, building for android")
    else()
        message(FATAL_ERROR "-- building for unsupport platform")
    endif()
endif()

# set standard and print features
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

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# function
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
    endif(WIN32)
    target_include_directories(${name} PUBLIC
        ${folder}
    )
endfunction()

# examples
set(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/example)
FILE(GLOB subdirs RELATIVE ${example_root_dir} ${example_root_dir}/*)
FOREACH(subdir ${subdirs})
    IF(IS_DIRECTORY ${example_root_dir}/${subdir})
        add_example(${subdir} ${example_root_dir}/${subdir})
    ENDIF()
ENDFOREACH()