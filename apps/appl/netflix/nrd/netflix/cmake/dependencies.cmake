# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

# Platform independent way to concatenate an arbitray number of files
# Invoke it with: cmake -DFILENAME=<path_to_dependencies_file> -DCXX=<compiler> -DDPKG_QUERY=<path_to_dpkg-query> -DSEARCH_PACKAGE=<package> -DSEARCH_VERSION=<minimum_version_required> -P dpkg.cmake

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

find_program(DPKG_QUERY dpkg-query)
if(NOT DPKG_QUERY)
    message(FATAL_ERROR "dependencies.cmake: dpkg-query not found.")
endif()

if(NOT FILENAME)
    message(FATAL_ERROR "dependencies.cmake: no dependencies file passed.")
endif()

if(NOT EXISTS ${FILENAME})
    message(FATAL_ERROR "dependencies.cmake: dependencies file '${FILENAME}' does not exist.")
endif()

#set(CMAKE_CXX_COMPILER "/usr/local/i686-netflix-linux-gnu-4.2/icecream/bin/i686-netflix-linux-gnu-g++")

set(DEP "")

macro(SEARCH_PACKAGE name version)
    if(version)
    else()
        message(STATUS "Searching for package ${name}")
    endif()

    execute_process(COMMAND ${DPKG_QUERY} -W -f=\${Version} ${name}
                    RESULT_VARIABLE DPKG_EXIT
                    OUTPUT_VARIABLE DPKG_OUTPUT
                    ERROR_VARIABLE DPKG_ERROR
    )

    if(${DPKG_EXIT}) # Non-zero exit code
        message("\n=============================================================================================")
        message("ERROR: The package ${name} is required to build this tree.\n       Try 'sudo apt-get install ${name}'")
        message("=============================================================================================")
        message(FATAL_ERROR "")
    else()
        message(STATUS "Package ${name} version ${DPKG_OUTPUT} found")
    endif()

    if(version)
        string(REPLACE "-" "." SV "${version}")
        string(REPLACE "-" "." PV "${DPKG_OUTPUT}")

        if(PV VERSION_LESS SV AND NOT PV VERSION_EQUAL SV)
            message("\n=============================================================================================")
            message("ERROR: At least version ${SV} of the package '${name}' is required but you have ${PV}.\n       Try running 'netflix-update'")
            message("=============================================================================================")
            message(FATAL_ERROR "")
        else()
            message(STATUS "Package ${name} version ${PV} is at least the required ${SV}")
            set(DEP "${DEP}\n${name} ${PV}")
        endif()
    else()
        set(DEP "${DEP}\n${name}")
    endif()
endmacro()

macro(SEARCH_COMPILER name mode)
    if("${mode}" STREQUAL "CN")
        message(STATUS "Checking if the compiler is the disallowed ${name}")
    elseif("${mode}" STREQUAL "CY")
        message(STATUS "Checking if the compiler is not ${name}")
    else()
        message(STATUS "Checking if the compiler is ${name} (${mode})")
    endif()

    string(FIND "${CXX}" "${name}" result)

    if(result GREATER -1)
        if("${mode}" STREQUAL "CN")
            message("\n=============================================================================================")
            message("ERROR: You can't build this tree with the '${name}' toolchain.\n       Run configure with no toolchain parameter to build with the default")
            message("=============================================================================================")
            message(FATAL_ERROR "")
        endif()
    else()
        if("${mode}" STREQUAL "CY")
            message("\n=============================================================================================")
            message("ERROR: You have to build this tree with the '${name}' toolchain.\n       ${CXX} is not supported.")
            message("=============================================================================================")
            message(FATAL_ERROR "")
        endif()
    endif()

endmacro()


file(READ ${FILENAME} dependencies)
string(REGEX REPLACE ";" "\\\\;" dependencies "${dependencies}")
string(REGEX REPLACE "\n" ";" dependencies "${dependencies}")

foreach(dep ${dependencies})
    if(NOT dep MATCHES "^#")
        string(REPLACE ":" ";" dep ${dep})
        list(GET dep 0 command)
        list(GET dep 1 package)
        list(LENGTH dep items)
        if(items GREATER 2)
            list(GET dep 2 version)
        else()
            set(version "")
        endif()

        if(command STREQUAL "P")
            search_package("${package}" "${version}")
        elseif(command STREQUAL "CN")
            search_compiler("${package}" "CN")
        elseif(command STREQUAL "CY")
            search_compiler("${package}" "CY")
        else()
            message(FATAL_ERROR "Unknown command ${command}")
        endif()
    endif()
endforeach()

file(WRITE dependencies.dep "${DEP}")


