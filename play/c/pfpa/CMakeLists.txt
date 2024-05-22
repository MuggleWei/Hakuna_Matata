cmake_minimum_required (VERSION 3.18.6)
project(parse_pcap_file)

################################
# general config
################################

message("-- use c compiler ${CMAKE_C_COMPILER}")
message("-- use c++ compiler ${CMAKE_CXX_COMPILER}")

# set compile parameter
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

# for LSP
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################
# deps
################################

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)

message("# fetch mugglec")
FetchContent_Declare(
	mugglec
	GIT_REPOSITORY https://github.com/MuggleWei/mugglec.git
	GIT_TAG v1.8.1
	GIT_SHALLOW TRUE
	)
FetchContent_MakeAvailable(mugglec)

message("# fetch pcap")
FetchContent_Declare(
	pcap
	GIT_REPOSITORY https://github.com/the-tcpdump-group/libpcap.git
	GIT_TAG libpcap-1.10.4
	GIT_SHALLOW TRUE
	)
FetchContent_MakeAvailable(pcap)

################################
# configure
################################
# version
file(STRINGS "version.txt" version)

string(REPLACE "-" ";" semver_ext ${version})
list(GET semver_ext 0 semver)
string(REPLACE "." ";" semver_list ${semver})

list(GET semver_list 0 PFPA_VER_MAJOR)
list(GET semver_list 1 PFPA_VER_MINOR)
list(GET semver_list 2 PFPA_VER_PATCH)

set(PFPA_VERSION "${version}")
set(PFPA_SOVERSION "${PFPA_VER_MAJOR}")

# dll
if (BUILD_SHARED_LIBS)
	set(PFPA_USE_DLL ON)
else()
	set(PFPA_USE_DLL OFF)
endif()

# configure
set(src_dir "${CMAKE_CURRENT_LIST_DIR}/pfpa")
set(gen_dir "${CMAKE_BINARY_DIR}/gen/pfpa")
configure_file(
	"${src_dir}/pfpa_config.h.in"
	"${gen_dir}/pfpa_config.h")

################################
# pfpa
################################
file(GLOB src_h ${src_dir}/*.h)
file(GLOB src_c ${src_dir}/*.c)
if (WIN32)
	add_library(pfpa
		${src_h}
		${src_c})
else()
	add_library(pfpa
		${src_c})
endif()
target_link_libraries(pfpa PUBLIC mugglec pcap)
target_include_directories(pfpa PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/gen>
	$<BUILD_INTERFACE:${FETCHCONTENT_BASE_DIR}/mugglec-src>
	$<BUILD_INTERFACE:${FETCHCONTENT_BASE_DIR}/pcap-src>)
add_dependencies(pfpa mugglec pcap)
target_compile_definitions(pfpa PRIVATE
	MUGGLE_HOLD_LOG_MACRO=1)
set_target_properties(pfpa PROPERTIES 
	LINKER_LANGUAGE C
	VERSION ${PFPA_VERSION}
	SOVERSION ${PFPA_SOVERSION}
	DEBUG_POSTFIX d
)

# install
include(GNUInstallDirs)
set(CMAKE_INSTALL_LIBDIR lib)

install(TARGETS pfpa
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
if (APPLE)
	set_target_properties(pfpa
		PROPERTIES
		INSTALL_RPATH "@executable_path/../lib"
		)
else ()
	set_target_properties(pfpa
		PROPERTIES
		INSTALL_RPATH "\$ORIGIN/../lib"
		)
endif()


################################
# function
################################

function(add_app name folder)
	file(GLOB tmp_h ${folder}/*.h)
	file(GLOB tmp_c ${folder}/*.c)
	file(GLOB tmp_cpp ${folder}/*.cpp)
	file(GLOB tmp_cc ${folder}/*.cc)

	if (WIN32)
		add_executable(${name} ${tmp_h} ${tmp_c} ${tmp_cpp} ${tmp_cc})
		set_target_properties(${name}
			PROPERTIES
			FOLDER "app"
			VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
		)
	else()
		add_executable(${name} ${tmp_c} ${tmp_cpp} ${tmp_cc})
	endif(WIN32)

	target_link_libraries(${name} PUBLIC pfpa)
	add_dependencies(${name} pfpa)
	target_compile_definitions(${name} PRIVATE
		MUGGLE_HOLD_LOG_MACRO=1
	)

	install(TARGETS ${name}
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")
	if (APPLE)
		set_target_properties(${name}
			PROPERTIES
			INSTALL_RPATH "@executable_path/../lib"
			)
	else ()
		set_target_properties(${name}
			PROPERTIES
			INSTALL_RPATH "\$ORIGIN/../lib"
			)
	endif()
endfunction()

################################
# examples
################################

set(app_root_dir ${CMAKE_CURRENT_LIST_DIR}/apps)
FILE(GLOB subdirs RELATIVE ${app_root_dir} ${app_root_dir}/*)
FOREACH(subdir ${subdirs})
	IF(IS_DIRECTORY ${app_root_dir}/${subdir})
		add_app(${subdir} ${app_root_dir}/${subdir})
	ENDIF()
ENDFOREACH()
