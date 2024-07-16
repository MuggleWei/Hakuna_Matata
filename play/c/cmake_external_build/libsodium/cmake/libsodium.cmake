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

if (MSVC)
	if (${sodium_build_type} STREQUAL "debug")
		if (BUILD_SHARED_LIBS)
			set(sodium_configure "DynDebug")
		else()
			set(sodium_configure "StaticDebug")
		endif()
		set(sodium_type_dir_name Debug)
	else()
		if (BUILD_SHARED_LIBS)
			set(sodium_configure "DynRelease")
		else()
			set(sodium_configure "StaticRelease")
		endif()
		set(sodium_type_dir_name Release)
	endif()

	if ((MSVC_VERSION GREATER_EQUAL 1920) AND (MSVC_VERSION LESS_EQUAL 1929))
		set(sodium_vs vs2019)
		set(sodium_vc v142)
	elseif ((MSVC_VERSION GREATER_EQUAL 1910) AND (MSVC_VERSION LESS_EQUAL 1919))
		set(sodium_vs vs2017)
		set(sodium_vc v141)
	elseif (MSVC_VERSION EQUAL 1900)
		set(sodium_vs vs2015)
		set(sodium_vc v140)
	else ()
		message(FATAL_ERROR "unsupport visual studio version")
	endif()

	if (BUILD_SHARED_LIBS)
		set(sodium_dir_name dynamic)
	else()
		set(sodium_dir_name static)
	endif()

	string(REPLACE "/" "\\" WIN_FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR})

	if (BUILD_SHARED_LIBS)
		ExternalProject_Add(libsodium
			SOURCE_DIR        "${FETCHCONTENT_BASE_DIR}/libsodium-src"
			BINARY_DIR        "${FETCHCONTENT_BASE_DIR}/libsodium-build"
			CONFIGURE_COMMAND ""
			BUILD_COMMAND     msbuild /m /v:n /p:Configuration=${sodium_configure} /p:Platform=x64 ${WIN_FETCHCONTENT_BASE_DIR}\\libsodium-src\\builds\\msvc\\${sodium_vs}\\libsodium.sln
			COMMAND ${CMAKE_COMMAND} -E copy ${WIN_FETCHCONTENT_BASE_DIR}\\libsodium-src\\bin\\x64\\${sodium_type_dir_name}\\${sodium_vc}\\${sodium_dir_name}\\libsodium.dll ${CMAKE_INSTALL_PREFIX}\\bin\\libsodium.dll
			COMMAND ${CMAKE_COMMAND} -E copy ${WIN_FETCHCONTENT_BASE_DIR}\\libsodium-src\\bin\\x64\\${sodium_type_dir_name}\\${sodium_vc}\\${sodium_dir_name}\\libsodium.lib ${CMAKE_INSTALL_PREFIX}\\lib\\libsodium.lib
			# install headers
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${WIN_FETCHCONTENT_BASE_DIR}\\libsodium-src\\src\\libsodium\\include ${CMAKE_INSTALL_PREFIX}\\include
			# remove private & make files
			COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_INSTALL_PREFIX}\\include\\sodium\\private
			COMMAND ${CMAKE_COMMAND} -E remove  ${CMAKE_INSTALL_PREFIX}\\include\\sodium\\version.h.in
			COMMAND ${CMAKE_COMMAND} -E remove  ${CMAKE_INSTALL_PREFIX}\\include\\Makefile.in
			COMMAND ${CMAKE_COMMAND} -E remove  ${CMAKE_INSTALL_PREFIX}\\include\\Makefile.am
			INSTALL_COMMAND   ""
		)
	else()
		ExternalProject_Add(libsodium
			SOURCE_DIR        "${FETCHCONTENT_BASE_DIR}/libsodium-src"
			BINARY_DIR        "${FETCHCONTENT_BASE_DIR}/libsodium-build"
			CONFIGURE_COMMAND ""
			BUILD_COMMAND     msbuild /m /v:n /p:Configuration=${sodium_configure} /p:Platform=x64 ${WIN_FETCHCONTENT_BASE_DIR}\\libsodium-src\\builds\\msvc\\${sodium_vs}\\libsodium.sln
			COMMAND ${CMAKE_COMMAND} -E copy ${WIN_FETCHCONTENT_BASE_DIR}\\libsodium-src\\bin\\x64\\${sodium_type_dir_name}\\${sodium_vc}\\${sodium_dir_name}\\libsodium.lib ${CMAKE_INSTALL_PREFIX}\\lib\\libsodium.lib
			# install headers
			COMMAND ${CMAKE_COMMAND} -E copy_directory ${WIN_FETCHCONTENT_BASE_DIR}\\libsodium-src\\src\\libsodium\\include ${CMAKE_INSTALL_PREFIX}\\include
			# remove private & make files
			COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_INSTALL_PREFIX}\\include\\sodium\\private
			COMMAND ${CMAKE_COMMAND} -E remove  ${CMAKE_INSTALL_PREFIX}\\include\\sodium\\version.h.in
			COMMAND ${CMAKE_COMMAND} -E remove  ${CMAKE_INSTALL_PREFIX}\\include\\Makefile.in
			COMMAND ${CMAKE_COMMAND} -E remove  ${CMAKE_INSTALL_PREFIX}\\include\\Makefile.am
			INSTALL_COMMAND   ""
		)
	endif()
else()
	if (${sodium_build_type} STREQUAL "debug")
		set(sodium_configure_debug "--enable-debug")
	else()
		set(sodium_configure_debug "")
	endif()

	if (BUILD_SHARED_LIBS)
		set(sodium_configure_shared "--enable-shared=yes")
		set(sodium_configure_static "--enable-static=no")
		set(sodium_configure_pic "")
		set(sodium_configure_pie "")
	else()
		set(sodium_configure_shared "--enable-shared=no")
		set(sodium_configure_static "--enable-static=yes")
		set(sodium_configure_pic "--with-pic")
		set(sodium_configure_pie "--disable-pie")
	endif()

	ExternalProject_Add(libsodium
		SOURCE_DIR        "${FETCHCONTENT_BASE_DIR}/libsodium-src"
		BINARY_DIR        "${FETCHCONTENT_BASE_DIR}/libsodium-build"
		CONFIGURE_COMMAND ${FETCHCONTENT_BASE_DIR}/libsodium-src/configure --prefix=${CMAKE_INSTALL_PREFIX} ${sodium_configure_debug} ${sodium_configure_shared} ${sodium_configure_static} ${sodium_configure_pic} ${sodium_configure_pie}
		BUILD_COMMAND     ${MAKE}
		INSTALL_COMMAND   make install
	)
endif()
