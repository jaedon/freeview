# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.


# Post build command to create a symbolic link to the DPI directory if the DPI creates it
# Invoke it with: cmake -DSOURCE=<absolute path to the DPI directory> -DTARGET=<absolute path to target directory> -P dpilink.cmake
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# Create the data directory
file(MAKE_DIRECTORY ${TARGET}/data)

# Link or copy the DPI directory
if(EXISTS "${SOURCE}" AND IS_DIRECTORY "${SOURCE}")
    if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
        execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different ${SOURCE} ${TARGET}/data/dpi RESULT_VARIABLE ret)
    else()
        execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink ${SOURCE} ${TARGET}/data/dpi RESULT_VARIABLE ret)
    endif()

    if(NOT ret EQUAL 0)
        if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
            message(FATAL_ERROR "Write error when trying to copy ${SOURCE} to ${TARGET}/data/dpi")
        else()
            message(FATAL_ERROR "Write error when trying to link ${SOURCE} to ${TARGET}/data/dpi")
        endif()
    endif()
endif()
