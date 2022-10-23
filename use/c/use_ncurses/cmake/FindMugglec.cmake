find_path(MUGGLEC_INCLUDE_DIR
	NAMES muggle/c/muggle_c.h
	PATH_SUFFIXES include
	PATHS ${CMAKE_PREFIX_PATH})

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWER_BUILD_TYPE)
if (${LOWER_BUILD_TYPE} STREQUAL "debug")
	message("try find mugglec libs with debug mode")
	find_library(MUGGLEC_LIBRARIES
		NAMES mugglecd
		PATH_SUFFIXES lib lib/static
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH)
else()
	message("try find mugglec libs with release mode")
	find_library(MUGGLEC_LIBRARIES
		NAMES mugglec
		PATH_SUFFIXES lib lib/static
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MUGGLEC REQUIRED_VARS
	MUGGLEC_INCLUDE_DIR
	MUGGLEC_LIBRARIES
)
