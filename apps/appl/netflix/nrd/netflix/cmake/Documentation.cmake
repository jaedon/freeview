# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)
find_package(Doxygen)
include(CMakeParseArguments)

macro(ADD_DOCUMENTATION name)
    get_property(DOXYGEN_FOUND GLOBAL PROPERTY DOXYGEN_FOUND)

    if(NOT DOXYGEN_FOUND)
        add_custom_target(${name}_documentation COMMENT "Documentation not available, please install Doxygen and run configure again")
    else()
        cmake_parse_arguments(ADX "EXTRACT_ALL" "MAINPAGES_DIRECTORY;EXAMPLES;TITLE" "INPUT;EXCLUDE_PATTERNS" ${ARGN})
        set(ADX_NAME "${name}")

        set(DOXYGEN_CONFIG ${NRDP_ROOT_DIR}/doxygen)
        set(DOXYGEN_TARGET ${NRDP_BINARY_ROOT_DIR}/doxygen)

        set(DOXYGEN_PROJECT_NUMBER "${CPACK_PACKAGE_VERSION}")
        set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Documentation")

        set(DOXYGEN_HTML_HEADER "${DOXYGEN_CONFIG}/netflix-doxygen-header.html")
        set(DOXYGEN_HTML_FOOTER "${DOXYGEN_CONFIG}/netflix-doxygen-footer.html")
        set(DOXYGEN_HTML_STYLESHEET "${DOXYGEN_CONFIG}/netflix-doxygen.css")

        configure_file_if_different(${DOXYGEN_CONFIG}/reference.html.in ${DOXYGEN_OUTPUT_DIRECTORY}/${name}-reference.html)

        if(NOT ADX_MAINPAGES_DIRECTORY)
            set(ADX_MAINPAGES_DIRECTORY "${DOXYGEN_CONFIG}/mainpages")
        endif()
        list(APPEND ADX_INPUT "${ADX_MAINPAGES_DIRECTORY}/${name}-reference.mainpage")
        string(REPLACE ";" " " ADX_INPUT "${ADX_INPUT}")

        string(REPLACE ";" " " ADX_EXCLUDE_PATTERNS "${ADX_EXCLUDE_PATTERNS}")

        set(DOXYGEN_INPUT "${ADX_INPUT}" )
        set(DOXYGEN_EXCLUDE_PATTERNS "${ADX_EXCLUDE_PATTERNS}")
        set(DOXYGEN_HTML_OUTPUT "${name}-reference")
        set(DOXYGEN_EXAMPLE_PATH "${ADX_EXAMPLES}")

        if(ADX_EXTRACT_ALL)
            set(DOXYGEN_EXTRACT_ALL "Yes")
        else()
            set(DOXYGEN_EXTRACT_ALL "No")
        endif()

        configure_file_if_different(${DOXYGEN_CONFIG}/doxyfile.in ${DOXYGEN_TARGET}/${name}-reference.config)

        add_custom_target(
            ${name}_documentation
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_TARGET}/${name}-reference.config
            COMMAND ${CMAKE_COMMAND} -E copy ${DOXYGEN_CONFIG}/netflix-logo-sm.png "${DOXYGEN_OUTPUT_DIRECTORY}/${name}-reference"
            COMMENT "Creating ${name} reference documentation..."
            WORKING_DIRECTORY ${DOXYGEN_CONFIG}
        )

        add_dependencies(documentation ${name}_documentation)
    endif()
endmacro()

# -------------------------------------------------------------------------------------------------------------------------------------

if(NOT DOXYGEN_FOUND)
    message( STATUS " Doxygen was not found. You will not be able to generate local Doxygen DPI reference docs. These are available on the NRD Portal." )
endif()

message(STATUS " ")
message(STATUS "-------------------------------------------------------------")
message(STATUS " Documentation")

if(DOXYGEN_FOUND)
    message(STATUS " - Doxygen: ${DOXYGEN_EXECUTABLE}")
else()
    message(STATUS " - Doxygen NOT found. Documentation building won't work.")
endif()
if(DOXYGEN_DOT_FOUND)
    message(STATUS " - Dot: ${DOXYGEN_DOT_EXECUTABLE}")
    set(DOXYGEN_HAVE_DOT "YES")
else()
    message(STATUS " - Dot not found")
endif()

message(STATUS "-------------------------------------------------------------")
message(STATUS " ")

# Common configuration

set_property(GLOBAL PROPERTY DOXYGEN_FOUND ${DOXYGEN_FOUND})

if(DOXYGEN_FOUND)
    add_custom_target(documentation COMMENT "Building documentation" WORKING_DIRECTORY ${DOXYGEN_CONFIG})
else()
    add_custom_target(documentation COMMENT "Documentation not available, please install Doxygen and run configure again")
endif()
