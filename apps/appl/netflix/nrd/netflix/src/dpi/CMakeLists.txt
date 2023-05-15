# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required( VERSION 2.8 )

project(nrddpi)

set(DPI_COMMON "${CMAKE_CURRENT_SOURCE_DIR}/common")
add_subdirectory(${DPI_COMMON})

if (TEE_IMPLEMENTATION)
  if(EXISTS "${BUILD_DPI_DIRECTORY}/ReeTeeImp/${TEE_IMPLEMENTATION}" AND IS_DIRECTORY "${BUILD_DPI_DIRECTORY}/ReeTeeImp/${TEE_IMPLEMENTATION}")
    set(REE_TEE_IMPLEMENTATION "${BUILD_DPI_DIRECTORY}/ReeTeeImp/${TEE_IMPLEMENTATION}")
  elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ReeTeeImp/${TEE_IMPLEMENTATION}" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ReeTeeImp/${TEE_IMPLEMENTATION}")
    set(REE_TEE_IMPLEMENTATION "${CMAKE_CURRENT_SOURCE_DIR}/ReeTeeImp/${TEE_IMPLEMENTATION}")
  else()
    message(FATAL_ERROR "Error: TEE implementation ${TEE_IMPLEMENTATION} does not exist!")
  endif()

    add_subdirectory(
		${REE_TEE_IMPLEMENTATION}/Tee ${CMAKE_CURRENT_BINARY_DIR}/ReeTeeImp/${TEE_IMPLEMENTATION}/Tee
	) 

endif()

if(DPI_IMPLEMENTATION)
    set(DPI_NAME "${DPI_IMPLEMENTATION}")

    # Use this macro to print information from your DPI implementation CMake file
    macro(DPI_MESSAGE message)
        message(STATUS "[${DPI_NAME} dpi] ${message}")
    endmacro()

    list(APPEND DPI_SEARCH_NAMES "${DPI_IMPLEMENTATION}")
    string(REPLACE "-" ";" DPI_NAMES "${DPI_IMPLEMENTATION}")
    list(GET DPI_NAMES 0 DPI_IMPLEMENTATION)
    list(APPEND DPI_SEARCH_NAMES "${DPI_IMPLEMENTATION}")

    set(DPI_FOUND FALSE)

    foreach(DPI_IMPLEMENTATION ${DPI_SEARCH_NAMES})
        if(NOT DPI_FOUND)
            if(EXISTS "${BUILD_DPI_DIRECTORY}/${DPI_IMPLEMENTATION}" AND IS_DIRECTORY "${BUILD_DPI_DIRECTORY}/${DPI_IMPLEMENTATION}")
                set(DPI_IMPLEMENTATION "${BUILD_DPI_DIRECTORY}/${DPI_IMPLEMENTATION}")
                set(DPI_FOUND TRUE)
            endif()

            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DPI_IMPLEMENTATION}" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${DPI_IMPLEMENTATION}")
                add_subdirectory(${DPI_IMPLEMENTATION})
                set(DPI_FOUND TRUE)
            else()
                # Absolute path to a DPI directory
                if(EXISTS "${DPI_IMPLEMENTATION}" AND IS_DIRECTORY "${DPI_IMPLEMENTATION}")
                    # Do this to remove the trailing backslash if specified
                    get_filename_component(DPI_DIRNAME "${DPI_IMPLEMENTATION}" ABSOLUTE)
                    # Get the directory name
                    get_filename_component(DPI_DIRNAME "${DPI_DIRNAME}" NAME)

                    DPI_MESSAGE("Adding external DPI from ${DPI_DIRNAME}")
                    add_subdirectory(${DPI_IMPLEMENTATION} ${CMAKE_CURRENT_BINARY_DIR}/${DPI_DIRNAME})
                    set(DPI_FOUND TRUE)
                endif()
            endif()
        endif()
    endforeach()

    if(NOT DPI_FOUND)
        message(FATAL_ERROR "Error: DPI implementation ${DPI_IMPLEMENTATION} does not exist!")
    endif()
endif()

# Bubble up the value of HAVE_DPI_CONFIGURATION_OPTIONS
set(HAVE_DPI_CONFIGURATION_OPTIONS "${HAVE_DPI_CONFIGURATION_OPTIONS}" PARENT_SCOPE)
