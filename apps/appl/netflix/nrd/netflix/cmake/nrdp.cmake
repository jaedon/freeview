# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

# Master CMake include file - Includes all the files for the common build infrastructure

get_filename_component(NRDP_MACROS_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
message(STATUS "Including NRDP Macros from ${NRDP_MACROS_DIRECTORY}")

# If we don't have CMakeParseArguments (old CMake or no access the the modules directory) include our own
include(CMakeParseArguments OPTIONAL RESULT_VARIABLE HAS_PARSE_ARGUMENTS)
if(NOT HAS_PARSE_ARGUMENTS)
    message(STATUS "Using NRDP CMake modules directory ${NRDP_MACROS_DIRECTORY}/modules")
    list(APPEND CMAKE_MODULE_PATH "${NRDP_MACROS_DIRECTORY}/modules")
endif()

include("${NRDP_MACROS_DIRECTORY}/nrdp_macros.cmake")

include("${NRDP_MACROS_DIRECTORY}/Toolchain.cmake")
include("${NRDP_MACROS_DIRECTORY}/FindNrdModule.cmake")
include("${NRDP_MACROS_DIRECTORY}/Installation.cmake")
include("${NRDP_MACROS_DIRECTORY}/Resources.cmake")
include("${NRDP_MACROS_DIRECTORY}/Components.cmake")

if("${CMAKE_VERSION}" VERSION_GREATER "2.8.5")
    # Cotire - Copyright (c) 2012 Sascha Kratky
    # https://github.com/sakra/cotire
    include("${NRDP_MACROS_DIRECTORY}/cotire.cmake")
endif()
