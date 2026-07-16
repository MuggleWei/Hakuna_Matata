cmake_minimum_required (VERSION 3.18.6)
project(use_simd)

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
else()
    if (ANDROID)
        message("-- cmake run cross compiling, building for android")
    else()
        message(FATAL_ERROR "-- building for unsupport platform")
    endif()
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

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################
# configure
################################
if ((${CMAKE_C_COMPILER_ID} STREQUAL "GNU") OR
	(${CMAKE_C_COMPILER_ID} STREQUAL "Clang"))
	set(SIMD_SSE4_FLAGS "-msse4.2")
	set(SIMD_AVX2_FLAGS "-mavx2")
	set(SIMD_FMA_FLAGS "-mfma")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
	set(SIMD_SSE4_FLAGS "/arch:SSE4.2")
	set(SIMD_AVX2_FLAGS "/arch:AVX2")
else()
	message("ignore check simd")
endif()

include(CheckCSourceCompiles)

set(CMAKE_REQUIRED_FLAGS "${SIMD_SSE4_FLAGS}")
check_c_source_compiles("
    #include <smmintrin.h>
    int main() {
        __m128i a = _mm_setzero_si128();
        __m128i b = _mm_minpos_epu16(a);
        return 0;
    }
	" SIMD_HAS_SSE4)
unset(CMAKE_REQUIRED_FLAGS)

if (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
	set(CMAKE_REQUIRED_FLAGS "${SIMD_AVX2_FLAGS}")
else()
	set(CMAKE_REQUIRED_FLAGS "${SIMD_AVX2_FLAGS} ${SIMD_FMA_FLAGS}")
endif()
check_c_source_compiles("
	#include <immintrin.h>
	int main() {
		__m256i a = _mm256_setzero_si256();
		__m256 b = _mm256_set1_ps(1.0f);
		__m256 c = _mm256_set1_ps(2.0f);
		__m256 d = _mm256_fmadd_ps(b, c, b);
		return 0;
	}
	" SIMD_HAS_AVX2)
unset(CMAKE_REQUIRED_FLAGS)

set(CMAKE_REQUIRED_FLAGS "")
check_c_source_compiles("
    #include <arm_neon.h>
    int main() {
		float32x4_t a = vdupq_n_f32(1.0f);
		float32x4_t b = vdupq_n_f32(2.0f);
		float32x4_t c = vmlaq_f32(a, b, a);
        return 0;
    }
	" SIMD_HAS_NEON)
unset(CMAKE_REQUIRED_FLAGS)

set(src_dir "${CMAKE_CURRENT_LIST_DIR}/src")
set(common_dir "${src_dir}/common")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
configure_file(
	"${common_dir}/config.h.in"
	"${generated_dir}/common/config.h")

################################
# deps
################################
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

# set mugglec compile options
set(MUGGLE_BUILD_TRACE OFF CACHE BOOL "")
set(MUGGLE_BUILD_SHARED_LIB ON CACHE BOOL "")
set(MUGGLE_BUILD_STATIC_PIC ON CACHE BOOL "")
set(MUGGLE_BUILD_BENCHMARK OFF CACHE BOOL "")
set(MUGGLE_BUILD_TESTING OFF CACHE BOOL "")
set(MUGGLE_BUILD_EXAMPLE OFF CACHE BOOL "")

message("# fetch mugglec ")
FetchContent_Declare(
	mugglec
	GIT_REPOSITORY https://github.com/MuggleWei/mugglec.git
	GIT_TAG v2.5.0
	GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(mugglec)

set(BUILD_SIMD_TESTING OFF CACHE BOOL "")
if (BUILD_SIMD_TESTING)
	message("# fetch unity test")
	FetchContent_Declare(
		unity
		GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
		GIT_TAG v2.5.2
		GIT_SHALLOW TRUE
		)
	FetchContent_MakeAvailable(unity)
endif()

################################
# output options
################################

# 尽可能让指令交错执行, 提升ILP(instruction-Level Parallelism)
set(INCREASE_SIMD_ILP OFF CACHE BOOL "")

message("--------------------------------")
message("Options")
message("# c compiler flags: ${CMAKE_C_FLAGS}")
message("# cxx compiler flags: ${CMAKE_CXX_FLAGS}")
message("# CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("# BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
message("# BUILD_SIMD_TESTING: ${BUILD_SIMD_TESTING}")
message("# SIMD_HAS_SSE4: ${SIMD_HAS_SSE4}")
message("# SIMD_HAS_AVX2: ${SIMD_HAS_AVX2}")
message("# SIMD_HAS_NEON: ${SIMD_HAS_NEON}")
message("# INCREASE_SIMD_ILP: ${INCREASE_SIMD_ILP}")
message("--------------------------------")

################################
# common library
################################
FILE(GLOB_RECURSE common_core_h "${common_dir}/*.h")
FILE(GLOB_RECURSE common_core_c "${common_dir}/*.c")
list(FILTER common_core_c EXCLUDE REGEX "${common_dir}/simd/*")

add_library(common_core OBJECT
	"${common_core_c}"
	"${common_core_h}"
	"${generated_dir}/common/config.h")
target_include_directories(common_core PRIVATE
	"${src_dir}"
	"${generated_dir}")
target_link_libraries(common_core PRIVATE mugglec)

if (SIMD_HAS_SSE4)
	FILE(GLOB_RECURSE common_sse4_h "${common_dir}/simd/*_sse4.h")
	FILE(GLOB_RECURSE common_sse4_c "${common_dir}/simd/*_sse4.c")
	add_library(common_sse4 OBJECT
		"${common_sse4_c}"
		"${common_sse4_h}")
	target_compile_options(common_sse4 PRIVATE ${SIMD_SSE4_FLAGS})
	target_include_directories(common_sse4 PRIVATE
		"${src_dir}"
		"${generated_dir}")
	target_link_libraries(common_sse4 PRIVATE mugglec)
endif()

if (SIMD_HAS_AVX2)
	FILE(GLOB_RECURSE common_avx2_h "${common_dir}/simd/*_avx2.h")
	FILE(GLOB_RECURSE common_avx2_c "${common_dir}/simd/*_avx2.c")
	add_library(common_avx2 OBJECT
		"${common_avx2_c}"
		"${common_avx2_h}")
	target_compile_options(common_avx2 PRIVATE ${SIMD_AVX2_FLAGS})
	if (NOT ${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
		target_compile_options(common_avx2 PRIVATE ${SIMD_FMA_FLAGS})
	endif()
	target_include_directories(common_avx2 PRIVATE
		"${src_dir}"
		"${generated_dir}")
	target_link_libraries(common_avx2 PRIVATE mugglec)
endif()

if (SIMD_HAS_NEON)
	FILE(GLOB_RECURSE common_neon_h "${common_dir}/simd/*_neon.h")
	FILE(GLOB_RECURSE common_neon_c "${common_dir}/simd/*_neon.c")
	add_library(common_neon OBJECT
		"${common_neon_c}"
		"${common_neon_h}")
	# NEON now without any flags
	#target_compile_options(common_sse4 PRIVATE ${SIMD_NEON_FLAGS})
	target_include_directories(common_neon PRIVATE
		"${src_dir}"
		"${generated_dir}")
	target_link_libraries(common_neon PRIVATE mugglec)
endif()

add_library(common)
target_sources(common PUBLIC $<TARGET_OBJECTS:common_core>)
if (SIMD_HAS_SSE4)
	target_sources(common PUBLIC $<TARGET_OBJECTS:common_sse4>)
endif()
if (SIMD_HAS_AVX2)
	target_sources(common PUBLIC $<TARGET_OBJECTS:common_avx2>)
endif()
if (SIMD_HAS_NEON)
	target_sources(common PUBLIC $<TARGET_OBJECTS:common_neon>)
endif()
target_include_directories(common PUBLIC
	$<BUILD_INTERFACE:${src_dir}>
	$<BUILD_INTERFACE:${generated_dir}>
	$<INSTALL_INTERFACE:include>)
target_link_libraries(common PUBLIC mugglec)

# install
include(GNUInstallDirs)
install(TARGETS common
	EXPORT mugglecTargets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(DIRECTORY "${common_dir}" "${generated_dir}/common"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h")

################################
# function
################################
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
    target_include_directories(${name} PUBLIC ${folder} ${generated_dir})
	target_link_libraries(${name} PUBLIC mugglec common)

	if ((${CMAKE_C_COMPILER_ID} STREQUAL "GNU") OR
		(${CMAKE_C_COMPILER_ID} STREQUAL "Clang"))
		target_compile_options(${name} PUBLIC -save-temps=obj)
	endif()
endfunction()

function(add_unittest name folder)
    message("add unittest ${name} ${folder}")

    set(name test_${name})

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
    target_include_directories(${name} PUBLIC ${folder} ${generated_dir})
	target_link_libraries(${name} PUBLIC mugglec common  unity::framework)

	if ((${CMAKE_C_COMPILER_ID} STREQUAL "GNU") OR
		(${CMAKE_C_COMPILER_ID} STREQUAL "Clang"))
		target_compile_options(${name} PUBLIC -save-temps=obj)
	endif()

	add_test(NAME ${name} COMMAND ${name})
endfunction()

# tut
set(tut_root_dir ${CMAKE_CURRENT_LIST_DIR}/tut)
FILE(GLOB subdirs RELATIVE ${tut_root_dir} ${tut_root_dir}/*)
FOREACH(subdir ${subdirs})
	IF(IS_DIRECTORY ${tut_root_dir}/${subdir})
		add_example(${subdir} ${tut_root_dir}/${subdir})
	ENDIF()
ENDFOREACH()

# examples
set(example_root_dir ${CMAKE_CURRENT_LIST_DIR}/examples)
FILE(GLOB subdirs RELATIVE ${example_root_dir} ${example_root_dir}/*)
FOREACH(subdir ${subdirs})
	IF(IS_DIRECTORY ${example_root_dir}/${subdir})
		add_example(${subdir} ${example_root_dir}/${subdir})
	ENDIF()
ENDFOREACH()

# tests
if (BUILD_SIMD_TESTING)
	enable_testing()

	set(test_root_dir ${CMAKE_CURRENT_LIST_DIR}/tests)
	FILE(GLOB subdirs RELATIVE ${test_root_dir} ${test_root_dir}/*)
	FOREACH(subdir ${subdirs})
		IF(IS_DIRECTORY ${test_root_dir}/${subdir})
			add_unittest(${subdir} ${test_root_dir}/${subdir})
		ENDIF()
	ENDFOREACH()
endif()
