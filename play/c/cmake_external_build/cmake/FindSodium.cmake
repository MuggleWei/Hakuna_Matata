INCLUDE(FindPackageHandleStandardArgs)

if (MSVC)
	set(sodium_search_name libsodium)
else()
	set(sodium_search_name sodium)
endif()

FIND_PATH(SODIUM_INCLUDE_DIR
	NAMES sodium.h
	PATH_SUFFIXES include
	PATHS ${sodium_search_path})
FIND_LIBRARY(SODIUM_LIBRARIES
	NAMES ${sodium_search_name}
	PATH_SUFFIXES lib
	PATHS ${sodium_search_path}
	NO_CMAKE_SYSTEM_PATH
	NO_CMAKE_ENVIRONMENT_PATH
	NO_SYSTEM_ENVIRONMENT_PATH)
if (MSVC AND BUILD_SHARED_LIBS)
	FIND_PATH(SODIUM_RUNTIMES
		NAMES ${sodium_search_name}.dll
		PATH_SUFFIXES bin
		PATHS ${sodium_search_path}
		NO_CMAKE_SYSTEM_PATH
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH)
	find_package_handle_standard_args(SODIUM DEFAULT_MSG
		SODIUM_INCLUDE_DIR
		SODIUM_LIBRARIES
		SODIUM_RUNTIMES)
else ()
	find_package_handle_standard_args(SODIUM DEFAULT_MSG
		SODIUM_INCLUDE_DIR
		SODIUM_LIBRARIES)
endif()

if (SODIUM_FOUND)
	message("# Found sodium")
	message("# sodium include dir: ${SODIUM_INCLUDE_DIR}")
	message("# sodium libraries: ${SODIUM_LIBRARIES}")
	if (MSVC AND BUILD_SHARED_LIBS)
		message("# sodium runtimes: ${SODIUM_RUNTIMES}")
	endif()
else()
	message(FATAL_ERROR "failed found sodium")
endif()

if (BUILD_SHARED_LIBS)
	add_library(sodium SHARED IMPORTED)
else()
	add_library(sodium STATIC IMPORTED)
endif()
if (MSVC)
	if (BUILD_SHARED_LIBS)
		set_target_properties(sodium PROPERTIES
			IMPORTED_IMPLIB "${SODIUM_LIBRARIES}"
			IMPORTED_LOCATION "${SODIUM_RUNTIMES}"
			INTERFACE_INCLUDE_DIRECTORIES "${SODIUM_INCLUDE_DIR}")
	else ()
		set_target_properties(sodium PROPERTIES
			IMPORTED_IMPLIB "${SODIUM_LIBRARIES}"
			IMPORTED_LOCATION "${SODIUM_LIBRARIES}"
			INTERFACE_INCLUDE_DIRECTORIES "${SODIUM_INCLUDE_DIR}")
	endif ()
else()
	set_target_properties(sodium PROPERTIES
		IMPORTED_IMPLIB "${SODIUM_LIBRARIES}"
		IMPORTED_LOCATION "${SODIUM_LIBRARIES}"
		INTERFACE_INCLUDE_DIRECTORIES "${SODIUM_INCLUDE_DIR}")
endif()
