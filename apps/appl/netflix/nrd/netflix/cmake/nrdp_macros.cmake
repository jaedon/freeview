# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

# General macros
# ------------------------------------------------------------------------------

# Checks if a list contains a value
macro(LIST_CONTAINS var value)
   set(${var})
   foreach(value2 ${ARGN})
       if(${value} STREQUAL ${value2})
          set(${var} TRUE)
       endif()
   endforeach()
endmacro()

# Appends a value to a variable
macro(APPEND variable value)
    set(${variable} "${${variable}}${value}")
endmacro()

# Accept environment variables
macro(GET_ENVIRONMENT_VARIABLE name description value)
    if(NOT ${name}_set)
        set(ENV_VALUE $ENV{${name}})
        if(ENV_VALUE)
            set(${name} "${ENV_VALUE}" CACHE STRING description FORCE)
        else()
            set(${name} "${value}" CACHE STRING description FORCE)
        endif()
    endif()
    set(${name}_set TRUE CACHE INTERNAL "True when ${name} was set at configure time" FORCE)
endmacro()


# Add a directory to the build system only if it exists
macro(ADD_OPTIONAL_SUBDIRECTORY directory)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${directory}")
        add_subdirectory("${directory}")
    endif()
endmacro()


# Create a configure file only if it's going to be different
macro(CONFIGURE_FILE_IF_DIFFERENT input output)
    configure_file("${input}" "${output}.gen")
    if(EXISTS "${output}")
        file(READ "${output}" cfif_output)
    else()
        set(cfif_output "")
    endif()
    file(READ "${output}.gen" cfif_output_gen)

    if(NOT "${cfif_output}" STREQUAL "${cfif_output_gen}")
        file(WRITE "${output}" "${cfif_output_gen}")
    endif()
endmacro()



# Deprecated old macros
macro(BUILDTOOL name source libraries dependencies)
    message("WARNING! - Ignoring old/deprecated tool ${name}")
    set(IGNORE_DIRECTORY TRUE PARENT_SCOPE)
endmacro()

macro(BUILDTEST name source libraries dependencies)
    message("WARNING! - Ignoring old/deprecated test ${name}")
    set(IGNORE_DIRECTORY TRUE PARENT_SCOPE)
endmacro()
