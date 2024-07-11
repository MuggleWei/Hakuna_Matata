# download libsodium
message("-- fetch libsodium")
FetchContent_Declare(
	libsodium
	GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
	GIT_TAG 1.0.20-RELEASE
	GIT_SHALLOW TRUE
)
FetchContent_Populate(libsodium)

# build libsodium
string(TOLOWER ${CMAKE_BUILD_TYPE} sodium_build_type)
if ("${sodium_build_type}" STREQUAL "")
	message(FATAL_ERROR "oop!!! CMAKE_BUILD_TYPE is empty!!!")
endif()

if (${sodium_build_type} STREQUAL "debug")
	set(sodium_configure_debug "--enable-debug")
else()
	set(sodium_configure_debug "")
endif()

if (BUILD_SHARED_LIBS)
	set(sodium_configure_shared "--enable-shared=yes")
	set(sodium_configure_static "--enable-static=no")
else()
	set(sodium_configure_shared "--enable-shared=no")
	set(sodium_configure_static "--enable-static=yes")
endif()

if (MSVC)
else()
	set(sodium_configure_command "")
endif()

ExternalProject_Add(libsodium
	SOURCE_DIR        "${FETCHCONTENT_BASE_DIR}/libsodium-src"
	BINARY_DIR        "${FETCHCONTENT_BASE_DIR}/libsodium-build"
	CONFIGURE_COMMAND ${FETCHCONTENT_BASE_DIR}/libsodium-src/configure --prefix=${CMAKE_INSTALL_PREFIX}/deps ${sodium_configure_debug} ${sodium_configure_shared} ${sodium_configure_static}
	BUILD_COMMAND     ${MAKE}
	INSTALL_COMMAND   make install
)
