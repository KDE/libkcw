include_directories( .. )

add_library(commoninjector MODULE commoninjectiondll.cpp)
target_link_libraries(commoninjector kcw)

add_library(testplugin MODULE testplugin.cpp)
target_link_libraries(testplugin kcw)

add_library(simpleinjectable SHARED simpleinjectable.cpp)
target_link_libraries(simpleinjectable kcw)

set(kcw_TESTS
    debugtest
    sharedmemorytest
    injectortest
    injectortest2
    pluginloadertest
    notifiertest
)
foreach(_test ${kcw_TESTS})
    add_executable(${_test} ${_test}.cpp)
    target_link_libraries(${_test} kcw)
    add_test(${_test} ${_test})
endforeach(_test ${kcw_TESTS})
target_link_libraries(injectortest2 simpleinjectable)
