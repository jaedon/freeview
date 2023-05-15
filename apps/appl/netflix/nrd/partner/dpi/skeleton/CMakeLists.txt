# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

# reference REE/TEE interface implementation
add_subdirectory(${REE_TEE_IMPLEMENTATION} ${CMAKE_BINARY_DIR}/reeteebin)

include("CheckCSourceCompiles")
include("CheckIncludeFile")
include("CheckIncludeFileCXX")
include("CheckLibraryExists")

include_directories(${DPI_COMMON})
include_directories(${DPI_COMMON}/ies)
include_directories(${DPI_COMMON}/NTBA)
include_directories(${REE_TEE_IMPLEMENTATION})

add_definitions(-D__STDC_CONSTANT_MACROS)

set(HAVE_DPI_DIRECTORY "${NRDP_BINARY_ROOT_DIR}/lib/dpi")
set(HAVE_DPI_CONFIGURATION_OPTIONS true PARENT_SCOPE)

# We need to pass absolute library paths to override the linker search directories, that's how CMake works
get_environment_variable(TOOLCHAIN_DIRECTORY "Toolchain location" "")
if(TOOLCHAIN_DIRECTORY MATCHES "i686")
    set(DPI_REFERENCE_DESKTOP ON)
endif()

# Common
# --------------------------------------------------------------
set(SOURCES
    MediaUtils.cpp
    ESPlayerConstants.cpp
    ManufSS.cpp
    DeviceLibNative.cpp
    FileSystem.cpp
    VideoPipelineCapability.cpp
    ESManagerNative.cpp
    ESPlayerNative.cpp
    PlaybackGroupNative.cpp
    SampleWriterNative.cpp
    AudioMixerSink.cpp
    AudioESPlayerNative.cpp
    VideoESPlayerNative.cpp
    BufferManagerNative.cpp
    DeviceThread.cpp
    SSOBridge.cpp
    SignupBridge.cpp
    CurlUtils.cpp
    SystemValues.cpp
)
if(BUILD_QA)
    list(APPEND SOURCES QADeviceBridge.cpp)
endif()

list(APPEND SOURCES ${DPI_COMMON_FILES})
add_definitions(${DPI_COMMON_DEFINITIONS})
include_directories(${DPI_COMMON_INCLUDE_DIRECTORIES})

list(APPEND SOURCES ${REE_TEE_IMPLEMENTATION_FILES})
add_definitions(${REE_TEE_IMPLEMENTATION_DEFINITIONS})
include_directories(${REE_TEE_IMPLEMENTATION_INCLUDE_DIRECTORIES})

# Application manager
# --------------------------------------------------------------
if(GIBBON_PLATFORM STREQUAL "application-manager")
    add_definitions("-DNRDP_APPLICATION_MANAGER")
    add_component_option_text(COMPONENT nrddpi DESCRIPTION "Building with Application Manager support" VALUE "ON")
else()
    add_component_option_text(COMPONENT nrddpi DESCRIPTION "Building with Application Manager support" VALUE "OFF")
endif()

# Network Interfaces
# --------------------------------------------------------------
list(APPEND SOURCES LinuxInterfaces.cpp)


if(DEFINED DPI_REFERENCE_DESKTOP AND EXISTS /lib/i386-linux-gnu/libc.so.6)
    list(APPEND LIBRARIES /lib/i386-linux-gnu/libc.so.6)
    if(EXISTS /lib/i386-linux-gnu/ld-linux.so.2)
        list(APPEND LIBRARIES /lib/i386-linux-gnu/ld-linux.so.2)
    endif()
 endif()

if(NOT DPI_REFERENCE_DRM STREQUAL "none")
    # Create the DPI directory if we have DRM configured
    set_property(GLOBAL PROPERTY HAVE_DPI_DIRECTORY ${HAVE_DPI_DIRECTORY})
    list(APPEND SOURCES PlayReadyDrmSystem.cpp PlayReadyDrmSession.cpp)
endif()

add_library(nrddpi ${SOURCES})
target_link_libraries(nrddpi nrdtee nrdbase nrd z ${LIBRARIES})

if(DEFINED DEPENDENCIES AND NOT DEPENDENCIES STREQUAL "")
    add_dependencies(nrddpi ${DEPENDENCIES})
endif()

# DRM
# --------------------------------------------------------------
include(drm.cmake)

create_install_target(
    TARGET nrddpi
    DESCRIPTION "Reference DPI"
    COMPONENT "NRDP"
    PUBLIC_HEADERS ""
)

add_documentation("referencedpi" INPUT ${CMAKE_CURRENT_SOURCE_DIR}/ TITLE "NRDP Sample DPI Reference" MAINPAGES_DIRECTORY "${NRDP_ROOT_DIR}/doxygen/mainpages")

# Status information
# --------------------------------------------------------------
add_component_option_text(COMPONENT nrddpi DESCRIPTION "Implementation" VALUE "${DPI_NAME}")

message(" ")
message("=============================================================")
message(" Reference DPI configuration:")
message("-------------------------------------------------------------")

print_component_options(nrddpi "  - ")

message(" ")
message("  - DRM library: ${DPI_REFERENCE_DRM}")
message("=============================================================")
message(" ")


