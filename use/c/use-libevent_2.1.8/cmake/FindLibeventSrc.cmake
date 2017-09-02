if (NOT LIBEVENT_SEARCH_PATH)
	SET(LIBEVENT_SEARCH_PATH "${CMAKE_CURRENT_LIST_DIR}/../thirdparty/libevent")
endif()

message(${LIBEVENT_SEARCH_PATH})

FIND_PATH(LIBEVENT_SOURCE_DIR
    NAMES CMakeLists.txt event.c
    PATHS ${LIBEVENT_SEARCH_PATH})

FIND_PATH(LIBEVENT_INCLUDE_DIR
    NAMES event.h
	PATH_SUFFIXES include
    HINTS ${LIBEVENT_SOURCE_DIR}
    PATHS ${LIBEVENT_SEARCH_PATH})
	
INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibeventSrc DEFAULT_MSG
    LIBEVENT_SOURCE_DIR
    LIBEVENT_INCLUDE_DIR)