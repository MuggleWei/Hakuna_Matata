string(TOLOWER ${CMAKE_BUILD_TYPE} deps_type)
if (${deps_type} STREQUAL "debug")
	set(foo_postfix "d")
else()
	set(foo_postfix "")
endif()

include(FindPackageHandleStandardArgs)

FIND_PATH(FOO_INCLUDE_DIR
	NAMES foo/foo.h
	PATH_SUFFIXES include
	PATHS ${CMAKE_PREFIX_PATH})
FIND_LIBRARY(FOO_LIBRARIES
	NAMES foo${deps_lib_postfix}
	PATH_SUFFIXES lib
	NO_CMAKE_SYSTEM_PATH
	NO_CMAKE_ENVIRONMENT_PATH
	NO_SYSTEM_ENVIRONMENT_PATH)
find_package_handle_standard_args(FOO DEFAULT_MSG
	FOO_INCLUDE_DIR
	FOO_LIBRARIES)
if (FOO_FOUND)
	message("# Found foo")
	message("# foo include dir: ${FOO_INCLUDE_DIR}")
	message("# foo libraries: ${FOO_LIBRARIES}")
else()
	message(FATAL_ERROR "failed found foo")
endif()

add_library(foo SHARED IMPORTED)
set_target_properties(foo PROPERTIES
	IMPORTED_LOCATION "${FOO_LIBRARIES}"
	INTERFACE_INCLUDE_DIRECTORIES "${FOO_INCLUDE_DIR}")
