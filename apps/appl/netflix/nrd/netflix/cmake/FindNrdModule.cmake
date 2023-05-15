# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)
include(CMakeParseArguments)

#
# Helper macros
#
macro(FIND_NRD_MODULE_VERSION name file)
    file(STRINGS "${file}" VERSION_H REGEX "^#define ${name}_VERSION_MAJOR")
    string(REGEX MATCH "[0-9]+$" ${name}_VERSION_MAJOR "${VERSION_H}")
    file(STRINGS "${file}" VERSION_H REGEX "^#define ${name}_VERSION_MINOR")
    string(REGEX MATCH "[0-9]+$" ${name}_VERSION_MINOR "${VERSION_H}")
    file(STRINGS "${file}" VERSION_H REGEX "^#define ${name}_VERSION_PATCH")
    string(REGEX MATCH "[0-9]+$" ${name}_VERSION_PATCH "${VERSION_H}")
    if(NOT ${name}_VERSION_PATCH)
        set(${name}_VERSION_PATCH "0")
    endif()
    if(${name}_VERSION_MAJOR AND ${name}_VERSION_MINOR)
        set(${name}_FOUND TRUE)
    endif()
endmacro()

macro(FIND_LIST_OF_FILES name directory files version)
    foreach(file ${files})
        if(EXISTS "${DIRECTORY}/${file}")
            if(${version})
                set(${name}_FOUND TRUE)
            else()
                find_nrd_module_version(${name} "${DIRECTORY}/${file}")
            endif()
            if(${name}_FOUND)
                break()
            endif()
        else()
            set(${name}_FOUND FALSE)
            break()
        endif()
    endforeach()
endmacro()

#
# Finds a NRD module, either in source or binary form
#
# find_nrd_module(<package_name> [REQUIRED]
#                 DIRECTORIES directory1 [directory2 ...]
#                 [BINARY_FILES file1 [file2 ...]]
#                 [SOURCE_FILES file1 [file2 ...]])
#
# DIRECTORIES is a list of directories where to search for the files
# BINARY_FILES: list of files to search to identify the directory as a binary package
# SOURCE_FILES: list of files to search to identify the directory as a source package
#
# <package_name>_FOUND will be set to "source", "binary" or FALSE depending on which package was found
# Either <package_name>_BINARY or <package_name>_SOURCE will be set to true, depending on which package was found
# <package_name>_DIRECTORY will be set to the directory where <package> was found

macro(FIND_NRD_MODULE name)
    if(NOT ${name}_FOUND)
        cmake_parse_arguments(FIND_${name} "REQUIRED;NO_VERSION" "BASE_DIRECTORY" "DIRECTORIES;BINARY_FILES;SOURCE_FILES" ${ARGN})

        set(${name}_FOUND FALSE)
        string(TOLOWER ${name} ${name}_lowercase)

        foreach(DIRECTORY ${FIND_${name}_DIRECTORIES})
            get_filename_component(DIRECTORY "${DIRECTORY}" ABSOLUTE)
            message(STATUS "Looking for ${name} in ${DIRECTORY}")

            set(${name}_DIRECTORY "${DIRECTORY}")

            if(FIND_${name}_BINARY_FILES)
                find_list_of_files(${name} "${DIRECTORY}" "${FIND_${name}_BINARY_FILES}" ${FIND_${name}_NO_VERSION})

                if(${name}_FOUND)
                    set(${name} "binary")
                    set(${name}_SOURCE FALSE)
                    set(${name}_BINARY TRUE)
                    break()
                endif()
            endif()

            if(FIND_${name}_SOURCE_FILES)
                find_list_of_files(${name} "${DIRECTORY}" "${FIND_${name}_SOURCE_FILES}" ${FIND_${name}_NO_VERSION})

                if(${name}_FOUND)
                    set(${name} "source")
                    set(${name}_SOURCE TRUE)
                    set(${name}_BINARY FALSE)
                    break()
                endif()
            endif()
        endforeach()

        if(NOT ${name}_FOUND AND FIND_${name}_REQUIRED)
            message(FATAL_ERROR "\n${name} was not found!\n")
        endif()

        set(FIND_${name}_MESSAGE "Looking for ${name} in ${${name}_DIRECTORY} - Found ${${name}} build")
        if(${name}_VERSION_MAJOR)
            set(${name}_VERSION "${${name}_VERSION_MAJOR}.${${name}_VERSION_MINOR}")
            set(FIND_${name}_MESSAGE "${FIND_${name}_MESSAGE} (${${name}_VERSION})")
        endif()

        if(${name}_FOUND)
            message(STATUS "${FIND_${name}_MESSAGE}")
        else()
            message(STATUS "Looking for ${name} in ${${name}_DIRECTORY} - Not found")
        endif()

        if(${name}_VERSION_MAJOR AND ${name}_VERSION_MINOR)
            set(${name}_VERSION_STRING "${${name}_VERSION_MAJOR}.${${name}_VERSION_MINOR}.${${name}_VERSION_PATCH}")
        endif()

        if(${name})
            set(${name}_VERSION_STRING "${${name}_VERSION_STRING} (${${name}})")
        endif()

        if(FIND_${name}_BASE_DIRECTORY AND ${name}_FOUND)
            if(${name}_SOURCE)
                set(${name}_BINARY_ROOT_DIR ${FIND_${name}_BASE_DIRECTORY}/${${name}_lowercase})
                set(${name}_ROOT_DIR ${${name}_DIRECTORY})

                # Check if we have already added the directory
                get_property(FindNrdModule_dirs GLOBAL PROPERTY FindNrdModule_dirs)
                list(FIND FindNrdModule_dirs "${${name}_DIRECTORY}" result)
                if(result EQUAL -1)
                    set_property(GLOBAL APPEND PROPERTY FindNrdModule_dirs "${${name}_DIRECTORY}")
                    add_subdirectory("${${name}_DIRECTORY}" "${${name}_BINARY_ROOT_DIR}")
                endif()
            else()
                set(abs_${name}_path "${${name}_DIRECTORY}/")
                if(IS_ABSOLUTE ${abs_${name}_path})
                    file(RELATIVE_PATH abs_${name}_path ${FIND_${name}_BASE_DIRECTORY} ${abs_${name}_path})
                endif()
                set(abs_${name}_path ${FIND_${name}_BASE_DIRECTORY}/${abs_${name}_path})
                set(${name}_BINARY_ROOT_DIR "${abs_${name}_path}")
                include(${abs_${name}_path}/lib/cmake/${${name}_lowercase}.cmake)
                set(${name}_DIRECTORY "${abs_${name}_path}")
                set(${name}_ROOT_DIR ${${name}_DIRECTORY})
            endif()
        endif()
    endif()
endmacro()
