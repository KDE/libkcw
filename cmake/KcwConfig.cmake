# Config file for kcw
# It defines the following variables
# KCW_INCLUDE_DIRS  - include directories for kcw
# KCW_LIBRARIES     - libraries to link against

get_filename_component(KCW_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET kcw)
    include(${KCW_CMAKE_DIR}/KcwTargets.cmake)
endif()

set(KCW_INCLUDE_DIRS ${KCW_CMAKE_DIR}/../../../include/kcw)
set(KCW_LIBRARIES kcw)
