include_directories( .. )

add_library(commoninjector MODULE commoninjectiondll.cpp)
target_link_libraries(commoninjector kcw)

add_library(testplugin MODULE testplugin.cpp)
target_link_libraries(testplugin kcw)

set(kcw_TESTS
    debugtest
    sharedmemorytest
    injectortest
    pluginloadertest
    notifiertest
)
foreach(_test ${kcw_TESTS})
    add_executable(${_test} ${_test}.cpp)
    target_link_libraries(${_test} kcw)
    add_test(${_test} ${_test})
endforeach(_test ${kcw_TESTS})
