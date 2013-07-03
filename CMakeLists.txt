project(libkcw)

cmake_minimum_required(VERSION 2.8)

option(BUILD_TESTS "build unittests" ON)
option(BUILD_DOCS "build documentation" OFF)

add_definitions(-DUNICODE)

set(KCW_MAJOR_VERSION 0)
set(KCW_MINOR_VERSION 1)
set(KCW_PATCH_VERSION 0)

if(BUILD_TESTS)
    enable_testing()
endif(BUILD_TESTS)

set(kcw_HDRS
    kcwapp.h
    kcwdebug.h
    kcweventloop.h
    kcwinjector.h
    kcwpluginloader.h
    kcwprocess.h
    kcwsharedmemory.h
    kcwthread.h
    kcwthreadrep.h
)

set(kcw_SRCS
    kcwapp.cpp
    kcwdebug.cpp
    kcweventloop.cpp
    kcwinjector.cpp
    kcwpluginloader.cpp
    kcwprocess.cpp
    kcwthread.cpp
    kcwthreadrep.cpp
)

add_library(kcw STATIC ${kcw_SRCS})

install(TARGETS kcw RUNTIME DESTINATION bin
                    LIBRARY DESTINATION lib
                    ARCHIVE DESTINATION lib COMPONENT devel)

install(FILES ${kcw_HDRS} DESTINATION include/kcw COMPONENT devel)

if(BUILD_DOCS)
	# add a target to generate API documentation with Doxygen
	find_package(Doxygen)
	if(DOXYGEN_FOUND)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
		add_custom_target(doc ALL
			${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
	endif(DOXYGEN_FOUND)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html DESTINATION doc/kcw COMPONENT doc)
endif(BUILD_DOCS)

if(BUILD_TESTS)
add_subdirectory(tests)
endif(BUILD_TESTS)
