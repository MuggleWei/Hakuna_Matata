FIND_PATH(SQLITE_INCLUDE_DIR
	NAMES sqlite3.h
	PATH_SUFFIXES include
	PATHS ${CMAKE_PREFIX_PATH})

if (WIN32)
	FIND_LIBRARY(SQLITE_LIBS
		NAMES libsqlite3
		PATH_SUFFIXES lib
		PATHS ${CMAKE_PREFIX_PATH}
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH)

	FIND_PATH(SQLITE_BIN_DIR
		NAMES sqlite3.exe
		PATH_SUFFIXES bin
		PATHS ${CMAKE_PREFIX_PATH}
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH)
else()
	FIND_LIBRARY(SQLITE_LIBS
		NAMES sqlite3
		PATH_SUFFIXES lib
		PATHS ${CMAKE_PREFIX_PATH}
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH)

	FIND_PATH(SQLITE_BIN_DIR
		NAMES sqlite3
		PATH_SUFFIXES bin
		PATHS ${CMAKE_PREFIX_PATH}
		NO_CMAKE_ENVIRONMENT_PATH
		NO_SYSTEM_ENVIRONMENT_PATH)
endif()

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SqliteBin DEFAULT_MSG
    SQLITE_INCLUDE_DIR
	SQLITE_LIBS
	SQLITE_BIN_DIR)