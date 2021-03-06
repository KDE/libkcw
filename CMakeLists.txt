project(libkcw)

cmake_minimum_required(VERSION 2.8)

option(BUILD_TESTS "build unittests" ON)
option(BUILD_DOCS "build documentation" OFF)

add_definitions(-DUNICODE)

set(KCW_MAJOR_VERSION 0)
set(KCW_MINOR_VERSION 1)
set(KCW_PATCH_VERSION 0)

set(KCW_VERSION "${KCW_MAJOR_VERSION}.${KCW_MINOR_VERSION}.${KCW_PATCH_VERSION}")

if(MSVC)
    add_definitions(/Zc:wchar_t-)
endif()
configure_file(cmake/KcwConfigVersion.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/KcwConfigVersion.cmake" @ONLY)

# Install the KcwConfig.cmake and KcwConfigVersion.cmake
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/KcwConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KcwConfigVersion.cmake"
  DESTINATION lib/cmake/kcw COMPONENT dev)

if(BUILD_TESTS)
    enable_testing()
endif()

set(kcw_HDRS
    kcwapp.h
    kcwautomutex.h
    kcwdebug.h
    kcweventloop.h
    kcwinjector.h
    kcwnotifier.h
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
    kcwnotifier.cpp
    kcwpluginloader.cpp
    kcwprocess.cpp
    kcwthread.cpp
    kcwthreadrep.cpp
)

add_library(kcw STATIC ${kcw_SRCS})

install(TARGETS kcw EXPORT KcwTargets
                    RUNTIME DESTINATION bin
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
	endif()
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html DESTINATION doc/kcw COMPONENT doc)
endif()

if(BUILD_TESTS)
add_subdirectory(tests)
endif()

export(TARGETS kcw FILE "${CMAKE_CURRENT_BINARY_DIR}/KcwTargets.cmake")
export(PACKAGE kcw)

install(EXPORT KcwTargets DESTINATION lib/cmake/kcw COMPONENT devel)
