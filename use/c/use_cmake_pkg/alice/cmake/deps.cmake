include("${CMAKE_CURRENT_LIST_DIR}/FindFoo.cmake")

install(DIRECTORY
	"${FOO_INCLUDE_DIR}/"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	FILES_MATCHING
	PATTERN "*.h")
install(IMPORTED_RUNTIME_ARTIFACTS foo
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

# fix symbol links not install
if (NOT WIN32)
	get_filename_component(FOO_LIB_DIR "${FOO_LIBRARIES}" DIRECTORY)
	install(DIRECTORY
		"${FOO_LIB_DIR}/"
		DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		FILES_MATCHING
		PATTERN "*.so*")
endif()

install(FILES
	"${CMAKE_CURRENT_LIST_DIR}/FindFoo.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/alice")