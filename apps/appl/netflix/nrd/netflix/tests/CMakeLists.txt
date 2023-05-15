# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

# Add all subdirs if the "all" keyword was specified
if(NRDP_TESTS STREQUAL "all")
    set(NRDP_TESTS "")
    file(GLOB dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
    foreach(dir ${dirs})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
            list(APPEND NRDP_TESTS ${dir})
        endif()
    endforeach()
endif()

# Iterate through the list passed in the --nrd-tests parameter
foreach(test ${NRDP_TESTS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${test}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test}/CMakeLists.txt")
        set(IGNORE_DIRECTORY FALSE)
        add_subdirectory(${test})
        if(NOT IGNORE_DIRECTORY)
            list(APPEND NRDP_TESTS_FOUND ${test})
        endif()
    else()
        message(STATUS "NrdApp test: Ignoring directory '${test}'")
    endif()
endforeach()

add_component_option_text(COMPONENT nrd DESCRIPTION "Tests" VALUE "${NRDP_TESTS_FOUND}")
