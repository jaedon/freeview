# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

# Platform independent way to concatenate an arbitray number of files
# Invoke it with: cmake -DDPKG_QUERY=<path_to_dpkg-query> -DSEARCH_PACKAGE=<package> -DSEARCH_VERSION=<minimum_version_required> -P dpkg.cmake

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if(NOT DPKG_QUERY)
    message(FATAL_ERROR "dpkg.cmake: Path to dpkg-query not supplied.")
endif()

if(NOT SEARCH_PACKAGE)
    message(FATAL_ERROR "dpkg.cmake: Package to search not supplied.")
endif()

message(STATUS "Searching for package ${SEARCH_PACKAGE}")
execute_process(COMMAND ${DPKG_QUERY} -W -f=\${Version} ${SEARCH_PACKAGE}
                RESULT_VARIABLE DPKG_EXIT
                OUTPUT_VARIABLE DPKG_OUTPUT
                ERROR_VARIABLE DPKG_ERROR
)

if(${DPKG_EXIT}) # Non-zero exit code
    message("\nERROR: The package ${SEARCH_PACKAGE} is required to build this tree. Try 'sudo apt-get install ${SEARCH_PACKAGE}'\n\n")
    message(FATAL_ERROR "")
else()
    message(STATUS "Package ${SEARCH_PACKAGE} version ${DPKG_OUTPUT} found")
endif()

if(SEARCH_VERSION)
    string(REPLACE "-" "." SV "${SEARCH_VERSION}")
    string(REPLACE "-" "." PV "${DPKG_OUTPUT}")

    if(PV VERSION_LESS SV AND NOT PV VERSION_EQUAL SV)
        message("\nERROR: At least version ${SV} of the package '${SEARCH_PACKAGE}' is required but you have ${PV}. Try running 'netflix-update'\n\n")
        message(FATAL_ERROR "")
    endif()
endif()