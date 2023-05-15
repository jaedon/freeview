# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

# Add all subdirs if the "all" keyword was specified
if(NRDP_TOOLS MATCHES "all")
    set(NRDP_TOOLS "")
    file(GLOB dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
    foreach(dir ${dirs})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
            list(APPEND NRDP_TOOLS ${dir})
        endif()
    endforeach()
endif()

# Iterate through the list passed in the --nrd-tools parameter
foreach(tool ${NRDP_TOOLS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${tool}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${tool}/CMakeLists.txt")
        set(IGNORE_DIRECTORY FALSE)
        add_subdirectory(${tool})
        if(NOT IGNORE_DIRECTORY)
            list(APPEND NRDP_TOOLS_FOUND ${tool})
        endif()
    else()
        message(STATUS "NrdApp tools: Ignoring directory '${tool}'")
    endif()
endforeach()

add_component_option_text(COMPONENT nrd DESCRIPTION "Tools" VALUE "${NRDP_TOOLS_FOUND}")
