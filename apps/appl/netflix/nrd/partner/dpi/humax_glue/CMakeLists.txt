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
set(DPI_REFERENCE_TTS "none" CACHE STRING "Unsupported Text to speech engine")

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
    TextToSpeechHumax.cpp
    NativeBridge.cpp
    DeviceLibNative.cpp
    FileSystem.cpp
    VideoPipelineCapability.cpp
    ESManagerNative.cpp
    ESPlayerNative.cpp
    PlaybackGroupNative.cpp
    SampleWriterNative.cpp
    AudioMixerSink.cpp
	AudioMixerOutputBrcm.cpp
	AudioMixerOutputThread.cpp
    AudioESPlayerNative.cpp
    VideoESPlayerNative.cpp
    BufferManagerNative.cpp
    ReferenceClockNative.cpp
    AudioDecoder.cpp
    VideoDecoder.cpp
    AVController.cpp
    AVControllerEaseThread.cpp
    AVStatusManager.cpp
    AudioDecoderNative.cpp
    VideoDecoderNative.cpp
    DeviceThread.cpp
    DecoderLock.cpp
    DecoderManager.cpp
    RendererManager.cpp
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

# Application manager (application-manager)
# --------------------------------------------------------------
if(GIBBON_PLATFORM STREQUAL "humax")
    add_definitions("-DNRDP_APPLICATION_MANAGER")
    add_component_option_text(COMPONENT nrddpi DESCRIPTION "Building with Application Manager support" VALUE "ON")
else()
    add_component_option_text(COMPONENT nrddpi DESCRIPTION "Building with Application Manager support" VALUE "OFF")
endif()

# Network Interfaces
# --------------------------------------------------------------
list(APPEND SOURCES LinuxInterfaces.cpp)

# Video Decoder
# --------------------------------------------------------------


# Video Renderer
# --------------------------------------------------------------


# Audio Decoder
# --------------------------------------------------------------
add_definitions("-DHAVE_DDPLUS=1")

# Audio Renderer
# --------------------------------------------------------------


#Audio mixer
# --------------------------------------------------------------
list(APPEND SOURCES
     ${DPI_COMMON}/AudioMixerSoftware/AudioMixerSoftware.cpp
     ${DPI_COMMON}/AudioMixerSoftware/AudioMixerThread.cpp
     ${DPI_COMMON}/AudioMixerSoftware/AudioMixerDecoderVorbis.cpp
	 AudioMixerOutputBrcm.cpp AudioMixerOutputThread.cpp	 
    )
list(APPEND LIBRARIES vorbisidec)
add_definitions("-DAUDIO_MIXER_SOFTWARE=1")
add_definitions("-DAUDIO_MIXER_OUTPUT_BRCM=1")

#Text to speech
# --------------------------------------------------------------
if (DPI_REFERENCE_TTS STREQUAL "auto") #default
    if(DPI_NAME STREQUAL "reference-sink")
        set(DPI_REFERENCE_TTS "none")
    elseif (APPLE)
        set(DPI_REFERENCE_TTS "osx")
    else()
        set(DPI_REFERENCE_TTS "ivona")
    endif()
endif()
if (DPI_REFERENCE_TTS STREQUAL "osx")
    list(APPEND SOURCES
         tts/TextToSpeechOSX.mm
    )
    add_definitions("-DTEXTTOSPEECH_REFERENCE_OSX")
elseif (DPI_REFERENCE_TTS STREQUAL "ivona")
    set(CMAKE_REQUIRED_INCLUDES ${TOOLCHAIN_DIRECTORY}/netflix/include/ivona)

    check_include_file_cxx(ivona_tts.h HAVE_IVONA)

    unset(CMAKE_REQUIRED_INCLUDES)

    if (HAVE_IVONA)
        list(APPEND SOURCES
            tts/TextToSpeechIvona.cpp
        )
        include_directories(SYSTEM ${TOOLCHAIN_DIRECTORY}/netflix/include/ivona)
        list(APPEND LIBRARIES tts_engine )
        add_definitions("-DTEXTTOSPEECH_REFERENCE_IVONA")
        add_definitions("-DTEXTTOSPEECH_REFERENCE_IVONA_HAVE_LIB")
    else()
        #set(DPI_REFERENCE_TTS "none")
        list(APPEND SOURCES
            tts/TextToSpeechIvona.cpp
        )
        include_directories(SYSTEM ${TOOLCHAIN_DIRECTORY}/netflix/include/ivona)
        #list(APPEND LIBRARIES tts_engine )
        add_definitions("-DTEXTTOSPEECH_REFERENCE_IVONA")
        add_definitions("-DTEXTTOSPEECH_REFERENCE_IVONA_NO_LIB")
    endif()
endif()

if(DEFINED DPI_REFERENCE_DESKTOP AND EXISTS /lib/i386-linux-gnu/libc.so.6)
    list(APPEND LIBRARIES /lib/i386-linux-gnu/libc.so.6)
    if(EXISTS /lib/i386-linux-gnu/ld-linux.so.2)
        list(APPEND LIBRARIES /lib/i386-linux-gnu/ld-linux.so.2)
    endif()
 endif()

if(NOT DPI_REFERENCE_DRM STREQUAL "none")
    # Create the DPI directory if we have DRM configured
#    set_property(GLOBAL PROPERTY HAVE_DPI_DIRECTORY ${HAVE_DPI_DIRECTORY})
    list(APPEND SOURCES PlayReadyDrmSystem.cpp PlayReadyDrmSession.cpp)
endif()

add_library(nrddpi ${SOURCES})
target_link_libraries(nrddpi nrdtee nrdbase nrd z ${LIBRARIES})

if(DEFINED DEPENDENCIES AND NOT DEPENDENCIES STREQUAL "")
    add_dependencies(nrddpi ${DEPENDENCIES})
endif()

# Ivona
# --------------------------------------------------------------
if(DPI_REFERENCE_TTS STREQUAL "ivona")
    add_custom_command(TARGET nrddpi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}
#	HUMAX fix. (following line causes build error. reason : {toolcahin}/netflix/share/ivona does not exist.)
#        COMMAND ${CMAKE_COMMAND} -E create_symlink "${TOOLCHAIN_DIRECTORY}/netflix/share/ivona" "${HAVE_DPI_DIRECTORY}/ivona"
    )
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

#For Test Tool
foreach(tool ${NRDP_TOOLS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${tool}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${tool}/CMakeLists.txt")
        set(IGNORE_DIRECTORY FALSE)
        add_subdirectory(${tool})
        if(NOT IGNORE_DIRECTORY)
            list(APPEND NRDP_TOOLS_FOUND ${tool})
        endif()
    else()
        message(STATUS "Nrdp tools: Ignoring directory '${tool}'")
    endif()
endforeach()

add_component_option_text(COMPONENT nrdp DESCRIPTION "Tools" VALUE "${NRDP_TOOLS_FOUND}")

