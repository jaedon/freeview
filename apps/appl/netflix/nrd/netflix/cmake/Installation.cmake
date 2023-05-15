# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

include(CMakeParseArguments)

# Installation
# ------------------------------------------------------------------------------

# Creates forwading headers in the build directory
macro(PROCESS_HEADERS)
    cmake_parse_arguments(PH "" "TARGET;COMPONENT" "PUBLIC_HEADERS;PRIVATE_HEADERS" ${ARGN})

    if(NOT PH_TARGET)
        message(FATAL_ERROR "process_headers: missing TARGET parameter")
    endif()

    if(NOT PH_COMPONENT)
        message(FATAL_ERROR "process_headers: missing COMPONENT parameter")
    endif()

    # Copy all headers except those ending in _p.h
    foreach(header ${PH_PUBLIC_HEADERS})
        if(header MATCHES "^.*_p.h")
            list(REMOVE_ITEM ${PH_PUBLIC_HEADERS} ${header})
            list(APPEND ${PH_PRIVATE_HEADERS} ${header})
        elseif(NOT header MATCHES "^.*config.h")
            if(IS_ABSOLUTE ${header})
                file(RELATIVE_PATH header ${CMAKE_CURRENT_SOURCE_DIR} ${header})
            endif()

            if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${header})
                message(FATAL_ERROR "\nThe public header \"${header}\" (${CMAKE_CURRENT_SOURCE_DIR}/${header}) does not exist!\n")
            endif()

            file(RELATIVE_PATH INCLUDE_FILE ${${PH_COMPONENT}_BINARY_ROOT_DIR}/include/${PH_TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/${header})
            get_filename_component(rheader "${header}" NAME)
            configure_file_if_different(${NRDP_ROOT_DIR}/cmake/include.h.in ${${PH_COMPONENT}_BINARY_ROOT_DIR}/include/${PH_TARGET}/${rheader})
        endif()
    endforeach()

    foreach(header ${PH_PRIVATE_HEADERS})
        if(IS_ABSOLUTE ${header})
            file(RELATIVE_PATH header ${CMAKE_CURRENT_SOURCE_DIR} ${header})
        endif()

        if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${header})
            message(FATAL_ERROR "\nThe private header \"${header}\" (${CMAKE_CURRENT_SOURCE_DIR}/${header}) does not exist!\n")
        endif()

        file(RELATIVE_PATH INCLUDE_FILE ${${PH_COMPONENT}_BINARY_ROOT_DIR}/include/${PH_TARGET}/private ${CMAKE_CURRENT_SOURCE_DIR}/${header})

        get_filename_component(rheader "${header}" NAME)
        configure_file_if_different(${NRDP_ROOT_DIR}/cmake/include.h.in ${${PH_COMPONENT}_BINARY_ROOT_DIR}/include/${PH_TARGET}/private/${rheader})
    endforeach()
endmacro()

#
# Creates an installation target and an install-like structure on the build directory
# We also set the runtime rpath to be the installation and toolchain directories (if we are building dynamic libraries and the platform supports that)
macro(CREATE_INSTALL_TARGET) # target description public_headers)
    cmake_parse_arguments(CIT "" "TARGET;DESCRIPTION;COMPONENT" "BINARIES;RESOURCES;LIBRARIES;PUBLIC_HEADERS;PRIVATE_HEADERS;JS_EXPORTS" ${ARGN})

    if(NOT CIT_TARGET)
        message(FATAL_ERROR "create_install_target: missing TARGET parameter")
    endif()
    if(NOT CIT_DESCRIPTION)
        message(FATAL_ERROR "create_install_target: missing DESCRIPTION parameter")
    endif()
    if(NOT CIT_COMPONENT)
        message(FATAL_ERROR "create_install_target: missing COMPONENT parameter")
    endif()
    string(TOLOWER "${CIT_COMPONENT}" CIT_COMPONENT_LOWERCASE)

    process_headers(TARGET "${CIT_TARGET}"
                    COMPONENT "${CIT_COMPONENT}"
                    PUBLIC_HEADERS "${CIT_PUBLIC_HEADERS}"
                    PRIVATE_HEADERS "${CIT_PRIVATE_HEADERS}"
    )
    set_target_properties(${CIT_TARGET} PROPERTIES
        PUBLIC_HEADER "${CIT_PUBLIC_HEADERS}"
        ARCHIVE_OUTPUT_DIRECTORY "${${CIT_COMPONENT}_BINARY_ROOT_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${${CIT_COMPONENT}_BINARY_ROOT_DIR}/lib"
        INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )

    install(CODE "message(\"Installing ${CIT_DESCRIPTION}...\")")

    if(NOT ${NRD_EXPORT_NAME})
        set(NRD_EXPORT_NAME "${CIT_COMPONENT_LOWERCASE}")
    endif()

    if(CIT_JS_EXPORTS)
        foreach(FILE ${CIT_JS_EXPORTS})
            file(INSTALL ${FILE} DESTINATION "${${CIT_COMPONENT}_BINARY_ROOT_DIR}/js/")
        endforeach()
    endif()

    if(BUILD_INSTALL_TARGET)
        install(
            TARGETS ${CIT_TARGET}
            COMPONENT ${CIT_COMPONENT_LOWERCASE}
            EXPORT "netflix"
            LIBRARY DESTINATION lib COMPONENT Libraries
            ARCHIVE DESTINATION lib COMPONENT Libraries
            RUNTIME DESTINATION bin COMPONENT Executable
            PUBLIC_HEADER DESTINATION include/${CIT_TARGET}/ COMPONENT Headers
            CONFIGURATIONS Debug|Release
        )

        if(CIT_RESOURCES)
            install(
                DIRECTORY "${CIT_RESOURCES}"
                DESTINATION bin
                COMPONENT ${CIT_COMPONENT_LOWERCASE}
                CONFIGURATIONS Debug|Release
            )
        endif()

        if(CIT_LIBRARIES)
            install(
                FILES ${CIT_LIBRARIES}
                DESTINATION bin
                COMPONENT ${CIT_COMPONENT_LOWERCASE}
                CONFIGURATIONS Debug|Release
            )
        endif()

        # Export targets
        install(
            EXPORT "netflix"
            DESTINATION lib/cmake
        )

        set(CMAKE_EXPORT_PATH "${${CIT_COMPONENT}_BINARY_ROOT_DIR}/lib/cmake/${NRD_EXPORT_NAME}.cmake")
        file(MAKE_DIRECTORY ${${CIT_COMPONENT}_BINARY_ROOT_DIR}/lib/cmake)
        if(${CIT_TARGET} MATCHES "nrdbase")
            file(REMOVE ${CMAKE_EXPORT_PATH})
        endif()
        export(TARGETS ${CIT_TARGET} APPEND FILE "${CMAKE_EXPORT_PATH}")

        install(
            DIRECTORY ${${CIT_COMPONENT}_BINARY_ROOT_DIR}/lib/cmake
            DESTINATION ${CMAKE_INSTALL_PREFIX}
        )
    endif()
endmacro()
