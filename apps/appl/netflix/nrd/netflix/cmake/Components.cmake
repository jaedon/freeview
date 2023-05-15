# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

include(CMakeParseArguments)

# Components handling
# ------------------------------------------------------------------------------
macro(ADD_COMPONENT)
    cmake_parse_arguments(AC "" "NAME;VERSION;LOCATION" "" ${ARGN})

    if(NOT AC_NAME)
        message(FATAL_ERROR "add_component: missing NAME parameter")
    endif()

    # See if the component already exists
    get_property(COMPONENTS GLOBAL PROPERTY NRDP_COMPONENTS)
    set(COMPONENT_STRING "${AC_NAME}|${AC_VERSION}|${AC_LOCATION}")

    list(FIND COMPONENTS ${COMPONENT_STRING} FOUND)
    if(FOUND EQUAL -1)
        set_property(GLOBAL APPEND PROPERTY NRDP_COMPONENTS "${COMPONENT_STRING}")
    endif()
endmacro()


macro(PRINT_COMPONENTS prefix)
    get_property(COMPONENTS GLOBAL PROPERTY NRDP_COMPONENTS)
    foreach(COMPONENT_STRING ${COMPONENTS})
        string(REPLACE "|" ";" COMPONENT ${COMPONENT_STRING})
        list(GET COMPONENT 0 COMPONENT_NAME)
        list(GET COMPONENT 1 COMPONENT_VERSION)
        list(GET COMPONENT 2 COMPONENT_LOCATION)

        set(COMPONENT_TEXT "${prefix}${COMPONENT_NAME}")
        if(COMPONENT_VERSION)
            set(COMPONENT_TEXT "${COMPONENT_TEXT} ${COMPONENT_VERSION}")
        endif()

        if(COMPONENT_LOCATION)
            set(COMPONENT_TEXT "${COMPONENT_TEXT} [${COMPONENT_LOCATION}]")
        endif()

        message("${COMPONENT_TEXT}")
    endforeach()
endmacro()


# Per-component options handling
# ------------------------------------------------------------------------------
macro(ADD_COMPONENT_OPTION variable description default component)

    option(${variable} ${description} ${default})

    get_property(ACO_VALUES GLOBAL PROPERTY NRDP_COMPONENTS_${component})
    string(REGEX REPLACE "^\\[.*\\][ ]*" "" new_description "${description}")
    set(VALUE_STRING "${new_description}|${${variable}}")

    list(FIND ACO_VALUES ${VALUE_STRING} FOUND)
    if(FOUND EQUAL -1)
        set_property(GLOBAL APPEND PROPERTY NRDP_COMPONENTS_${component} "${VALUE_STRING}")
    endif()
endmacro()

# Adds free text to any component, without generating an option
macro(ADD_COMPONENT_OPTION_TEXT)
    cmake_parse_arguments(ACO "" "COMPONENT;DESCRIPTION" "VALUE" ${ARGN})

    if(NOT ACO_COMPONENT)
        message(FATAL_ERROR "add_component_option: missing COMPONENT parameter")
    endif()

    if(NOT ACO_DESCRIPTION)
        message(FATAL_ERROR "add_component_option: missing DESCRIPTION parameter")
    endif()

    if(NOT ACO_VALUE)
        set(ACO_VALUE OFF)
    endif()

    string(REPLACE ";" ", " ACO_VALUE "${ACO_VALUE}")

    get_property(ACO_VALUES GLOBAL PROPERTY NRDP_COMPONENTS_${ACO_COMPONENT})
    set(VALUE_STRING "${ACO_DESCRIPTION}|${ACO_VALUE}")

    list(FIND ACO_VALUES ${VALUE_STRING} FOUND)
    if(FOUND EQUAL -1)
        set_property(GLOBAL APPEND PROPERTY NRDP_COMPONENTS_${ACO_COMPONENT} "${VALUE_STRING}")
    endif()
endmacro()


macro(PRINT_COMPONENT_OPTIONS component prefix)
    get_property(OPTIONS GLOBAL PROPERTY NRDP_COMPONENTS_${component})
    foreach(OPTION_STRING ${OPTIONS})
        string(REPLACE "|" ";" OPTION ${OPTION_STRING})
        list(GET OPTION 0 OPTION_DESCRIPTION)
        list(GET OPTION 1 OPTION_VALUE)
        string(REGEX REPLACE "^\\[.*\\] " "" OPTION_DESCRIPTION "${OPTION_DESCRIPTION}")

        if(${OPTION_VALUE} STREQUAL "ON" OR ${OPTION_VALUE} STREQUAL "1")
            set(OPTION_VALUE "Yes")
        endif()
        if(${OPTION_VALUE} STREQUAL "OFF" OR ${OPTION_VALUE} STREQUAL "0")
            set(OPTION_VALUE "No")
        endif()

        set(OPTION_TEXT "${prefix}${OPTION_DESCRIPTION}: ${OPTION_VALUE}")

        message("${OPTION_TEXT}")
    endforeach()
endmacro()