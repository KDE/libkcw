include_directories( .. )

add_library(commoninjector MODULE commoninjectiondll.cpp)
target_link_libraries(commoninjector kcw)

add_library(testplugin MODULE testplugin.cpp)
target_link_libraries(testplugin kcw)

add_library(simpleinjectable SHARED simpleinjectable.cpp)
target_link_libraries(simpleinjectable kcw)
set_target_properties(commoninjector testplugin simpleinjectable PROPERTIES PREFIX "")

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
if(MINGW)
set_target_properties(sharedmemorytest PROPERTIES COMPILE_FLAGS -std=c++0x)
endif()
target_link_libraries(injectortest2 simpleinjectable)
