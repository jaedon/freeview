# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)
include("CheckIncludeFile")

# DRM
# --------------------------------------------------------------
if(DPI_REFERENCE_DRM STREQUAL "playready1.2")

    set(PLAYREADY_INCLUDE "${TOOLCHAIN_DIRECTORY}/netflix/include/playready1.2")

    set(CMAKE_REQUIRED_INCLUDES_SAVED "${CMAKE_REQUIRED_INCLUDES}")
    set(CMAKE_REQUIRED_DEFINITIONS_SAVED "${CMAKE_REQUIRED_DEFINITIONS}")
    set(CMAKE_REQUIRED_INCLUDES "${PLAYREADY_INCLUDE};${PLAYREADY_INCLUDE}/oem/common/inc;${PLAYREADY_INCLUDE}/oem/netflix/inc;${PLAYREADY_INCLUDE}/oem/netflix/trace")
    set(CMAKE_REQUIRED_DEFINITIONS "-DDRM_BUILD_ARCH=X86 -DDRM_BUILD_PLATFORM=ANSI -DDRM_BUILD_PROFILE=10 -DDRM_BUILD_TYPE=CHECKED -DDRM_NO_FORCE_ALIGN=0 -DDRM_NO_NATIVE_64_TYPES=0 -DDRM_PRECOMPUTE_GTABLE -DDRM_SUPPORT_APP_REVOCATION=0 -DDRM_SUPPORT_BLOCK_HEADER_CACHING=0 -DDRM_SUPPORT_CERTIFICATE_CACHING=0 -DDRM_SUPPORT_CLEANUP=0 -DDRM_SUPPORT_CONTENT_REVOCATION=0 -DDRM_SUPPORT_DATASTORE_PREALLOC=0 -DDRM_SUPPORT_DEVICESTORE=0 -DDRM_SUPPORT_DEVICE_REVOCATION=0 -DDRM_SUPPORT_DLA=1 -DDRM_SUPPORT_DOMAINS=1 -DDRM_SUPPORT_LICENSE_SYNC=0 -DDRM_SUPPORT_LOCKING=0 -DDRM_SUPPORT_MULTI_THREADING=0 -DDRM_SUPPORT_PC_FLAGS=0 -DDRM_SUPPORT_PROFILING=0 -DDRM_SUPPORT_SYMMETRIC_OPTIMIZATIONS=0 -DDRM_SUPPORT_TEST_SETTINGS=0 -DDRM_SUPPORT_TRACING=0 -DDRM_SUPPORT_WMDRMNET=0 -DDRM_TEST_IGNORE_UNIQUE_ID=1 -DDRM_TEST_MEM_ALLOC_CHECK=0 -DDRM_TEST_SUPPORT_NET_ID=0 -DDRM_USE_ASSEMBLY=0 -DNO_DRM_CRT=0 -DTEST_USE_OFFSET_CLOCK=1 -D_M_IX86")
    check_include_file("${PLAYREADY_INCLUDE}/drmmanager.h" HAVE_PLAYREADY_HEADER)

    if (HAVE_PLAYREADY_HEADER)
        add_definitions( -DPLAYREADY )
        if (EXISTS "${PLAYREADY_INCLUDE}/playready.cmake")
            include("${PLAYREADY_INCLUDE}/playready.cmake")
        endif()

        include_directories(${PLAYREADY_INCLUDE})
        include_directories(${PLAYREADY_INCLUDE}/oem/common/inc)
        include_directories(${PLAYREADY_INCLUDE}/oem/netflix/inc)
        include_directories(${PLAYREADY_INCLUDE}/oem/netflix/trace)

        link_directories(${PLAYREADY_LIB})
        target_link_libraries(nrddpi playready-1.2)

        add_custom_command(TARGET nrddpi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}/playready
            COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}/playready/storage
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLAYREADY_INCLUDE}/etc/playready ${HAVE_DPI_DIRECTORY}/playready
        )

    else()
        set(DPI_REFERENCE_DRM "none")
    endif()

    set(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES_SAVED}")
    set(CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS_SAVED}")

elseif(DPI_REFERENCE_DRM STREQUAL "playready2.0-ss")
    set(PLAYREADY_INCLUDE "${TOOLCHAIN_DIRECTORY}/netflix/include/playready2.0-ss")

    set(CMAKE_REQUIRED_INCLUDES_SAVED "${CMAKE_REQUIRED_INCLUDES}")
    set(CMAKE_REQUIRED_DEFINITIONS_SAVED "${CMAKE_REQUIRED_DEFINITIONS}")
    set(CMAKE_REQUIRED_INCLUDES "${PLAYREADY_INCLUDE};${PLAYREADY_INCLUDE}/oem/common/inc;${PLAYREADY_INCLUDE}/oem/netflix/inc;${PLAYREADY_INCLUDE}/oem/netflix/trace")
    set(CMAKE_REQUIRED_DEFINITIONS "-DDRM_BUILD_ARCH=X86 -DDRM_BUILD_PLATFORM=ANSI -DDRM_BUILD_PROFILE=10 -DDRM_BUILD_TYPE=CHECKED -DDRM_NO_FORCE_ALIGN=0 -DDRM_NO_NATIVE_64_TYPES=0 -DDRM_PRECOMPUTE_GTABLE -DDRM_SUPPORT_APP_REVOCATION=0 -DDRM_SUPPORT_BLOCK_HEADER_CACHING=0 -DDRM_SUPPORT_CERTIFICATE_CACHING=0 -DDRM_SUPPORT_CLEANUP=0 -DDRM_SUPPORT_CONTENT_REVOCATION=0 -DDRM_SUPPORT_DATASTORE_PREALLOC=0 -DDRM_SUPPORT_DEVICESTORE=0 -DDRM_SUPPORT_DEVICE_REVOCATION=0 -DDRM_SUPPORT_DLA=1 -DDRM_SUPPORT_DOMAINS=1 -DDRM_SUPPORT_LICENSE_SYNC=0 -DDRM_SUPPORT_LOCKING=0 -DDRM_SUPPORT_MULTI_THREADING=0 -DDRM_SUPPORT_PC_FLAGS=0 -DDRM_SUPPORT_PROFILING=0 -DDRM_SUPPORT_SYMMETRIC_OPTIMIZATIONS=0 -DDRM_SUPPORT_TEST_SETTINGS=0 -DDRM_SUPPORT_TRACING=0 -DDRM_SUPPORT_WMDRMNET=0 -DDRM_TEST_IGNORE_UNIQUE_ID=1 -DDRM_TEST_MEM_ALLOC_CHECK=0 -DDRM_TEST_SUPPORT_NET_ID=0 -DDRM_USE_ASSEMBLY=0 -DNO_DRM_CRT=0 -DTEST_USE_OFFSET_CLOCK=1 -D_M_IX86")

    #check_include_file("${PLAYREADY_INCLUDE}/drmmanager.h" HAVE_PLAYREADY_HEADER)

#if (HAVE_PLAYREADY_HEADER)
        add_definitions( -DPLAYREADY )
        add_definitions( -DPLAYREADY2 )
        add_definitions( -DPLAYREADY2TEE )
        if (EXISTS "${PLAYREADY_INCLUDE}/playready.cmake")
            include("${PLAYREADY_INCLUDE}/playready.cmake")
        endif()

        include_directories(${PLAYREADY_INCLUDE})
        include_directories(${PLAYREADY_INCLUDE}/oem/common/inc)
        include_directories(${PLAYREADY_INCLUDE}/oem/netflix/inc)
        include_directories(${PLAYREADY_INCLUDE}/oem/netflix/trace)

        link_directories(${PLAYREADY_LIB})
        target_link_libraries(nrddpi playready-2.0-ss)

        add_custom_command(TARGET nrddpi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}/playready
            COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}/playready/storage
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLAYREADY_INCLUDE}/etc/playready ${HAVE_DPI_DIRECTORY}/playready
        )

    #else()
    #    set(DPI_REFERENCE_DRM "none")
    #endif()

    set(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES_SAVED}")
    set(CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS_SAVED}")

elseif(DPI_REFERENCE_DRM STREQUAL "playready2.5tee")
    set(PLAYREADY_INCLUDE "${TOOLCHAIN_DIRECTORY}/netflix/include/playready2.5-tee")

    set(CMAKE_REQUIRED_INCLUDES_SAVED "${CMAKE_REQUIRED_INCLUDES}")
    set(CMAKE_REQUIRED_DEFINITIONS_SAVED "${CMAKE_REQUIRED_DEFINITIONS}")
    set(CMAKE_REQUIRED_INCLUDES "${PLAYREADY_INCLUDE};${PLAYREADY_INCLUDE}/oem/inc;${PLAYREADY_INCLUDE}/oem/ansi/inc;${PLAYREADY_INCLUDE}/oem/common/inc;${PLAYREADY_INCLUDE}/oem/netflix/inc;${PLAYREADY_INCLUDE}/oem/netflix/trace")

    add_definitions( -DPLAYREADY )
    add_definitions( -DPLAYREADY2 )
    add_definitions( -DPLAYREADY2TEE )
    add_definitions( -DPLAYREADY2_5 )
    add_definitions( -DPLAYREADY2_5_TEE )
    if (EXISTS "${PLAYREADY_INCLUDE}/playready.cmake")
        include("${PLAYREADY_INCLUDE}/playready.cmake")
    endif()
    add_definitions(${CMAKE_REQUIRED_DEFINITIONS})

    include_directories(${PLAYREADY_INCLUDE})
    include_directories(${PLAYREADY_INCLUDE}/oem/inc)
    include_directories(${PLAYREADY_INCLUDE}/oem/ansi/inc)
    include_directories(${PLAYREADY_INCLUDE}/oem/common/inc)
    include_directories(${PLAYREADY_INCLUDE}/oem/netflix/inc)
    include_directories(${PLAYREADY_INCLUDE}/oem/netflix/trace)

    link_directories(${PLAYREADY_LIB})
    add_library(playready-2.5-tee STATIC IMPORTED)
    set_property(TARGET playready-2.5-tee PROPERTY IMPORTED_LOCATION ${TOOLCHAIN_DIRECTORY}/netflix/lib/libplayready-2.5-tee.a)
    set_property(TARGET playready-2.5-tee PROPERTY IMPORTED_LINK_DEPENDENT_LIBRARIES "nrddpi")

    target_link_libraries(nrddpi playready-2.5-tee)

    add_custom_command(TARGET nrddpi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}/playready
        COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}/playready/storage
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLAYREADY_INCLUDE}/etc/playready ${HAVE_DPI_DIRECTORY}/playready
    )

    set(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES_SAVED}")
    set(CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS_SAVED}")

elseif(DPI_REFERENCE_DRM STREQUAL "playready2.5-ss-tee")
#    set(PLAYREADY_INCLUDE "${TOOLCHAIN_DIRECTORY}/netflix/include/playready2.5-ss-tee")
    set(PLAYREADY_INCLUDE "${DRM_INCLUDE_DIRECTORY}")

    set(CMAKE_REQUIRED_INCLUDES_SAVED "${CMAKE_REQUIRED_INCLUDES}")
    set(CMAKE_REQUIRED_DEFINITIONS_SAVED "${CMAKE_REQUIRED_DEFINITIONS}")
    set(CMAKE_REQUIRED_INCLUDES "${PLAYREADY_INCLUDE}")
#    set(CMAKE_REQUIRED_INCLUDES "${PLAYREADY_INCLUDE};${PLAYREADY_INCLUDE}/oem/inc;${PLAYREADY_INCLUDE}/oem/ansi/inc;${PLAYREADY_INCLUDE}/oem/common/inc;${PLAYREADY_INCLUDE}/oem/netflix/inc;${PLAYREADY_INCLUDE}/oem/netflix/trace")
    add_definitions( -DPLAYREADY )
    add_definitions( -DPLAYREADY2 )
    add_definitions( -DPLAYREADY2TEE )
    add_definitions( -DPLAYREADY2_5 )
    add_definitions( -DPLAYREADY2_5_TEE )
    add_definitions( -DPLAYREADY2_5_TEE_LDL )
    add_definitions( -DDRM_BUILD_PROFILE=900 )
    add_definitions( -DTARGET_LITTLE_ENDIAN=1 )
    add_definitions( -DTARGET_SUPPORTS_UNALIGNED_DWORD_POINTERS=1 )
    add_definitions( -DLINUX_BUILD )
    add_definitions( -DHUMAX_PLATFORM_BASE )
    add_definitions( -DDRM_ACTIVATION_PLATFORM=0 )
    add_definitions( -DDRM_SUPPORT_LOCKING=0 )
    add_definitions( -DDRM_SUPPORT_MULTI_THREADING=1 )
    add_definitions( -DDRM_SUPPORT_ECCPROFILING=1 )
    add_definitions( -DDRM_SUPPORT_INLINEDWORDCPY=1 )
    add_definitions( -DDRM_SUPPORT_DATASTORE_PREALLOC=1 )
    add_definitions( -DDRM_SUPPORT_REACTIVATION=0 )
    add_definitions( -DDRM_SUPPORT_NATIVE_64BIT_TYPES=1 )
    add_definitions( -DDRM_SUPPORT_FORCE_ALIGN=1 )
    add_definitions( -DDRM_SUPPORT_ASSEMBLY=0 )
    add_definitions( -DDRM_SUPPORT_PRECOMPUTE_GTABLE=1 )
    add_definitions( -DDRM_SUPPORT_SECUREOEMHAL=0 )
    add_definitions( -DDRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY=0 )
    add_definitions( -DDRM_SUPPORT_TRACING=0 )
    add_definitions( -DDRM_SUPPORT_PRIVATE_KEY_CACHE=0 )
    add_definitions( -DDRM_SUPPORT_EMBEDDED_CERTS=0 )
    add_definitions( -DDRM_SUPPORT_DEVICE_SIGNING_KEY=0 )
    add_definitions( -DDRM_SUPPORT_NONVAULTSIGNING=1 )
    add_definitions( -DDRM_USE_IOCTL_HAL_GET_DEVICE_INFO=0 )
    add_definitions( -D_DATASTORE_WRITE_THRU=1 )
    add_definitions( -D_ADDLICENSE_WRITE_THRU=0 )
    add_definitions( -DDRM_HDS_COPY_BUFFER_SIZE=32768 )
    add_definitions( -DDRM_TEST_SUPPORT_NET_IO=0 )
    add_definitions( -DDRM_TEST_SUPPORT_ACTIVATION=0 )
    add_definitions( -DUSE_PK_NAMESPACES=0 )
    add_definitions( -DDRM_INCLUDE_PK_NAMESPACE_USING_STATEMENT=1 )
    add_definitions( -DDRM_KEYFILE_VERSION=3 )
    add_definitions( -DDRM_TOOLS_PRND_TRANSMITTER_USES_UPNP=0 )
    add_definitions( -DDBG=1 )
    add_definitions( -DPLAY_READY_25 )
    add_definitions( -DNETFLIX_EXT )

#    if (EXISTS "${PLAYREADY_INCLUDE}/playready.cmake")
#        include("${PLAYREADY_INCLUDE}/playready.cmake")
#    endif()

#    add_definitions(${CMAKE_REQUIRED_DEFINITIONS})

    include_directories(${PLAYREADY_INCLUDE})
#    include_directories(${PLAYREADY_INCLUDE}/oem/inc)
#    include_directories(${PLAYREADY_INCLUDE}/oem/ansi/inc)
#    include_directories(${PLAYREADY_INCLUDE}/oem/common/inc) 
#    include_directories(${PLAYREADY_INCLUDE}/oem/netflix/inc)
#    include_directories(${PLAYREADY_INCLUDE}/oem/netflix/trace)

#    link_directories(${PLAYREADY_LIB})
#    add_library(playready-2.5-ss-tee STATIC IMPORTED)
#    set_property(TARGET playready-2.5-ss-tee PROPERTY IMPORTED_LOCATION ${TOOLCHAIN_DIRECTORY}/netflix/lib/libplayready-2.5-ss-tee.a)
#    set_property(TARGET playready-2.5-ss-tee PROPERTY IMPORTED_LINK_DEPENDENT_LIBRARIES "nrddpi")


#    target_link_libraries(nrddpi playready-2.5-ss-tee)

#    add_custom_command(TARGET nrddpi POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}/playready
#        COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}/playready/storage
#        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLAYREADY_INCLUDE}/etc/playready ${HAVE_DPI_DIRECTORY}/playready
#    )

    set(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES_SAVED}")
    set(CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS_SAVED}")

else()
    if(NOT DPI_REFERENCE_DRM STREQUAL "none")
        message(FATAL_ERROR "Invalid DRM implementation '${DPI_REFERENCE_DRM}'. Possible values are 'playready1.2', 'playready2.0-ss', 'playready2.5tee', 'playready2.5-ss-tee")
    else()
        set(DPI_REFERENCE_DRM "none")
    endif()
endif()

