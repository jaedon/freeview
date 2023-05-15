# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.12.2)
    cmake_policy(SET CMP0045 OLD)
    cmake_policy(SET CMP0046 OLD)
endif()

set(NRDP_APPLICATION "auto" CACHE STRING "[-nrd:+application:LIST] Sets the nrds to build.")
if(NRDP_APPLICATION MATCHES "auto")
    if(BUILD_NRDSYSTEM MATCHES "linux")
        set(NRDP_APPLICATION "gibbon")
    endif()
endif()

if(NRDP_APPLICATION)
    macro(CREATE_DATA_DIRECTORY target)
        get_target_property(target_directory ${target} RUNTIME_OUTPUT_DIRECTORY)
        if(NOT target_directory)
            set(target_directory "${CMAKE_CURRENT_BINARY_DIR}")
        endif()

        set(APP_DATA_DIR "${target_directory}/data")
        set(RESOURCES_DIR "${NRDP_ROOT_DIR}/resources")
        set(SCRIPTS_DIR "${NRDP_ROOT_DIR}/scripts")

        # Certificates
        copy_resource(${target} ${RESOURCES_DIR}/etc/certs/ui_ca.pem ${APP_DATA_DIR}/etc/certs/ui_ca.pem)

        # Configuration
        copy_resource(${target} ${RESOURCES_DIR}/configuration/config.xml ${APP_DATA_DIR}/etc/conf/config.xml)
        copy_resource(${target} ${RESOURCES_DIR}/configuration/common.xml ${APP_DATA_DIR}/etc/conf/common.xml)

        # DPI Directory
        get_property(HAVE_DPI_DIRECTORY GLOBAL PROPERTY HAVE_DPI_DIRECTORY)
        if(HAVE_DPI_DIRECTORY)
            file(RELATIVE_PATH relative_dpi_dir "${APP_DATA_DIR}" "${HAVE_DPI_DIRECTORY}")
            add_custom_command(
                OUTPUT "${APP_DATA_DIR}/dpi"
                COMMENT "Linking DPI internal directory"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${APP_DATA_DIR}"
                COMMAND ${CMAKE_COMMAND} -E create_symlink "${relative_dpi_dir}" ${APP_DATA_DIR}/dpi
                DEPENDS nrddpi
            )
            add_resource(${target} "${APP_DATA_DIR}/dpi")
        endif()
    endmacro()

    # Iterate through the list passed in the --with-application parameter
    foreach(app ${NRDP_APPLICATION})
        if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${app}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${app}/CMakeLists.txt")
        add_subdirectory(${app} "${CMAKE_CURRENT_BINARY_DIR}/${app}")
        else()
            message(" ")
            message("WARNING! - Ignoring specified application '${app}': The directory does not exist or doesn't contain a valid CMake file")
            message(" ")
        endif()
    endforeach()

    add_component_option_text(COMPONENT nrd DESCRIPTION "Applications" VALUE ${NRDP_APPLICATION})
endif()
