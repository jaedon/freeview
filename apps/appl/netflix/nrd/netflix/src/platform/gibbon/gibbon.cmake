# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

# Add source files to Gibbon
macro(gibbon_add_sources)
    get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
    foreach(arg ${ARGV})
        if(NOT IS_ABSOLUTE "${arg}")
            set(file "${CURRENT_DIRECTORY}/${arg}")
        else()
            set(file "${arg}")
        endif()
        set_property(GLOBAL APPEND PROPERTY GIBBON_SOURCES ${file})
    endforeach()
endmacro()

macro(gibbon_get_sources variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_SOURCES)
endmacro()

macro(gibbon_remove_sources)
    gibbon_get_sources(temp)
    foreach(arg ${ARGV})
        list(REMOVE_ITEM temp ${arg})
    endforeach()

    set_property(GLOBAL PROPERTY GIBBON_SOURCES ${temp})
endmacro()

macro(gibbon_add_install_libraries)
    get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
    foreach(arg ${ARGV})
        if(NOT IS_ABSOLUTE "${arg}")
            set(file "${CURRENT_DIRECTORY}/${arg}")
        else()
            set(file "${arg}")
        endif()
        set_property(GLOBAL APPEND PROPERTY GIBBON_INSTALL_LIBRARIES ${file})
    endforeach()
endmacro()

# Add include files to Gibbon
macro(gibbon_add_includes)
    get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
    foreach(arg ${ARGV})
        if(NOT IS_ABSOLUTE "${arg}")
            set(file "${CURRENT_DIRECTORY}/${arg}")
        else()
            set(file "${arg}")
        endif()
        set_property(GLOBAL APPEND PROPERTY GIBBON_INCLUDES ${file})
    endforeach()
endmacro()

macro(gibbon_get_includes variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_INCLUDES)
endmacro()

macro(gibbon_get_dependencies variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_DEPENDENCIES)
endmacro()

# Add libraries to link against Gibbon
macro(gibbon_add_libraries)
    foreach(arg ${ARGV})
        set_property(GLOBAL APPEND PROPERTY GIBBON_LIBRARIES ${arg})
    endforeach()
endmacro()

# Add dependencies to Gibbon
macro(gibbon_add_dependencies)
    foreach(arg ${ARGV})
        set_property(GLOBAL APPEND PROPERTY GIBBON_DEPENDENCIES ${arg})
    endforeach()
endmacro()

# Add mocables to be processed by Qt
macro(gibbon_add_mocables)
    get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
    foreach(arg ${ARGV})
            if(NOT IS_ABSOLUTE "${arg}")
            set(file "${CURRENT_DIRECTORY}/${arg}")
        else()
            set(file "${arg}")
        endif()
        set_property(GLOBAL APPEND PROPERTY GIBBON_MOCABLES ${file})
    endforeach()
endmacro()

# Add JavaScript files to Gibbon's JavaScript bridge
macro(gibbon_add_js)
    get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
    foreach(arg ${ARGV})
        if(NOT IS_ABSOLUTE "${arg}")
            set(file "${CURRENT_DIRECTORY}/${arg}")
        else()
            set(file "${arg}")
        endif()
        set_property(GLOBAL APPEND PROPERTY GIBBON_JS ${file})
    endforeach()
endmacro()

macro(gibbon_add_partnerbridge)
    get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
    foreach(arg ${ARGV})
        if(NOT IS_ABSOLUTE "${arg}")
            set(file "${CURRENT_DIRECTORY}/${arg}")
        else()
            set(file "${arg}")
        endif()
        set_property(GLOBAL APPEND PROPERTY GIBBON_PARTNERBRIDGE ${file})
    endforeach()
endmacro()

macro(gibbon_add_platformxml)
    get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
    foreach(arg ${ARGV})
        if(NOT IS_ABSOLUTE "${arg}")
            set(file "${CURRENT_DIRECTORY}/${arg}")
        else()
            set(file "${arg}")
        endif()
        set_property(GLOBAL APPEND PROPERTY GIBBON_PLATFORMXML ${file})
    endforeach()
endmacro()

macro(gibbon_add_keys)
    get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
    foreach(arg ${ARGV})
        if(NOT IS_ABSOLUTE "${arg}")
            set(file "${CURRENT_DIRECTORY}/${arg}")
        else()
            set(file "${arg}")
        endif()
        set_property(GLOBAL APPEND PROPERTY GIBBON_KEYS ${file})
    endforeach()
endmacro()

macro(gibbon_copy_resource source target attributes)
    get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)

    if(NOT IS_ABSOLUTE "${source}")
        set(file "${CURRENT_DIRECTORY}/${source}")
    else()
        set(file "${source}")
    endif()
    copy_resource(gibbon "${file}" "${target}" "${attributes}")
endmacro()

# Property getters
macro(gibbon_get_sources variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_SOURCES)
endmacro()

macro(gibbon_get_install_libraries variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_INSTALL_LIBRARIES)
endmacro()

macro(gibbon_get_libraries variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_LIBRARIES)
endmacro()

macro(gibbon_get_js variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_JS)
endmacro()

macro(gibbon_get_partnerbridge variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_PARTNERBRIDGE)
endmacro()

macro(gibbon_get_platformxml variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_PLATFORMXML)
endmacro()

macro(gibbon_get_keys variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_KEYS)
endmacro()

macro(gibbon_get_mocables variable)
    get_property(${variable} GLOBAL PROPERTY GIBBON_MOCABLES)
endmacro()

macro(gibbon_set_variable variable value)
    set(${variable} "${value}")
endmacro()

macro(gibbon_set_oem_config value)
    if(NOT IS_ABSOLUTE "${value}")
        get_filename_component(CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_FILE} PATH)
        gibbon_set_variable(GIBBON_OEM_XML "${CURRENT_DIRECTORY}/${value}")
    else()
        gibbon_set_variable(GIBBON_OEM_XML "${value}")
    endif()
endmacro()

# Convenience function for setting the input driver
macro(gibbon_set_input value)
    gibbon_set_variable(GIBBON_INPUT "${value}")
endmacro()

# Convenience function for setting the event loop (core)
macro(gibbon_set_eventloop value)
    gibbon_set_variable(GIBBON_EVENTLOOP "${value}")
endmacro()

# Convenience function for setting the graphics driver
macro(gibbon_set_graphics value)
    gibbon_set_variable(GIBBON_GRAPHICS "${value}")
endmacro()

# Convenience function for setting the network driver
macro(gibbon_set_network value)
    gibbon_set_variable(GIBBON_NETWORK "${value}")
endmacro()

# Convenience function for setting the script engine
macro(gibbon_set_scriptengine value)
    gibbon_set_variable(GIBBON_SCRIPTENGINE "${value}")
endmacro()

# See if a directory is valid
macro(gibbon_try_directory variable file directory)
    if(NOT ${${variable}})
        #message(STATUS "Looking for ${file} in ${directory}")
        include("${directory}/${file}" OPTIONAL RESULT_VARIABLE ${variable})
        if(NOT ${variable})
            set(${variable} FALSE)
        else()
            set(${variable} TRUE)
            set(GTD_DIRECTORY "${directory}")
            #message(STATUS "Found!")
        endif()
    endif()
endmacro()


# Search for CMakeLists in all possible Gibbon directories
macro(gibbon_add_directory)
    cmake_parse_arguments(GAB "" "NAME;PREFIX;FILE;VARIABLE" "" ${ARGN})

    if(NOT GAB_FILE)
        set(GAB_FILE "gibbon_${GAB_PREFIX}.cmake")
    endif()

    string(REPLACE "-" ";" GAB_FILE_LIST "${GAB_NAME}")
    list(GET GAB_FILE_LIST 0 GAB_NAME_SHORT)

    if("${GAB_NAME}" STREQUAL "" OR "${GAB_NAME}" STREQUAL "none" OR "${GAB_NAME}" STREQUAL "platform")
        set(GAB_FOUND TRUE) # So we don't include anything
    else()
        set(GAB_FOUND FALSE)
    endif()

    gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${GAB_NAME_SHORT}")
    gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${GAB_NAME}")
    if(NOT "${BUILD_GIBBON_DIRECTORY}" STREQUAL "")
        gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${BUILD_GIBBON_DIRECTORY}/${GAB_PREFIX}/${GAB_NAME_SHORT}")
        gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${BUILD_GIBBON_DIRECTORY}/${GAB_PREFIX}/${GAB_NAME}")
        gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${BUILD_GIBBON_DIRECTORY}/${GAB_NAME_SHORT}")
        gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${BUILD_GIBBON_DIRECTORY}/${GAB_NAME}")
        gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${BUILD_GIBBON_DIRECTORY}")
    endif()

    gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/${GAB_PREFIX}/${GAB_NAME}")
    gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/${GAB_PREFIX}/${GAB_NAME_SHORT}")
    gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/${GAB_NAME_SHORT}")
    gibbon_try_directory(GAB_FOUND "${GAB_FILE}" "${CMAKE_CURRENT_SOURCE_DIR}/${GAB_NAME}")

    if(NOT GAB_FOUND)
        message("****************** ERROR ****************")
        message("Can't find ${GAB_PREFIX} (${GAB_NAME})")
        message("****************** ERROR ****************")
        message(FATAL_ERROR "")
    else()
        if(NOT GAB_VARIABLE STREQUAL "")
            set(${GAB_VARIABLE} "${GTD_DIRECTORY}")
        endif()
    endif()
endmacro()