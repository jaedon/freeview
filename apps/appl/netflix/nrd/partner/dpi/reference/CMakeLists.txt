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

# Set this to true to build Single Sign On
set(BUILD_SSO FALSE)
# Set this to true to build the Voice bridge
set(BUILD_VOICE FALSE)
# Set this to true to build the Signup bridge
set(BUILD_SIGNUP FALSE)
# Set OpenMAX
set(BUILD_OPENMAX FALSE)

set(DPI_REFERENCE_VIDEO_RENDERER "auto" CACHE STRING "Video renderer to use in reference DPI.")
set(DPI_REFERENCE_VIDEO_DECODER "auto" CACHE STRING "Video decoder to use in reference DPI.")
set(DPI_REFERENCE_AUDIO_RENDERER "auto" CACHE STRING "Audio renderer to use in reference DPI.")
set(DPI_REFERENCE_AUDIO_DECODER "auto" CACHE STRING "Audio decoder to use in reference DPI.")
set(DPI_REFERENCE_AUDIO_MIXER "auto" CACHE STRING "Audio mixer to use in reference DPI.")
set(DPI_REFERENCE_TTS "auto" CACHE STRING "Text to speech engine to use in reference DPI.")

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
    AudioESPlayerNative.cpp
    VideoESPlayerNative.cpp
    BufferManagerNative.cpp
    ReferenceClockNative.cpp
    AudioDecoder.cpp
    VideoDecoder.cpp
    RendererManager.cpp
    AudioRenderer.cpp
    VideoRenderer.cpp
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
if(IOS)
    list(APPEND SOURCES IOSInterfaces.cpp)
elseif(APPLE)
    list(APPEND SOURCES OSXInterfaces.cpp)
else()
    list(APPEND SOURCES LinuxInterfaces.cpp)
endif()

# Video Decoder
# --------------------------------------------------------------
if (DPI_REFERENCE_VIDEO_DECODER STREQUAL "auto") #default
    if(DPI_NAME STREQUAL "reference-sink")
        set(DPI_REFERENCE_VIDEO_DECODER "sink")
    elseif(GIBBON_PLATFORM STREQUAL "rpi")
        set(DPI_REFERENCE_VIDEO_DECODER "openmax-il")
    else()
        check_include_file(hevc/hevc_dec_api.h VANGUARD_HEVC)
        if(VANGUARD_HEVC)
            set(DPI_REFERENCE_VIDEO_DECODER "vanguard_hevc")
            add_definitions("-DBUILD_VANGUARDHEVC_VIDEO_DECODER")
            #list(APPEND SOURCES x86/VideoDecoderHEVC.cpp)
            list(APPEND SOURCES x86/VideoDecoderDolbyVision.cpp)
            include_directories(${TOOLCHAIN_DIRECTORY}/netflix/include/hevc)
            list(APPEND LIBRARIES hevc-dec-l32g hevc-cmn-l32g vpl-l32g imf svml irc)
        else()
            set(DPI_REFERENCE_VIDEO_DECODER "ffmpeg")
        endif()
    endif()
endif()
if(DPI_REFERENCE_VIDEO_DECODER STREQUAL "sink")
    add_definitions("-DBUILD_SINK_VIDEO_DECODER")
    list(APPEND SOURCES sink/VideoDecoderSink.cpp)
elseif(DPI_REFERENCE_VIDEO_DECODER STREQUAL "openmax-il")
    set(BUILD_OPENMAX TRUE)
    add_definitions("-DBUILD_OPENMAXIL_VIDEO_DECODER")
    list(APPEND SOURCES openmax-il/VideoDecoderOMXIL.cpp)
else()
    add_definitions("-DBUILD_FFMPEG_VIDEO_DECODER")
    set(HAVE_DPI_CONFIGURATION_OPTIONS true PARENT_SCOPE)
    list(APPEND SOURCES x86/AudioDecoderNative.cpp x86/VideoDecoderNative.cpp)
    list(APPEND SOURCES x86/NaluHelper.cpp)
    set(CMAKE_C_FLAGS "-I${TOOLCHAIN_DIRECTORY}/netflix/include/ffmpeg2 ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-I${TOOLCHAIN_DIRECTORY}/netflix/include/ffmpeg2 ${CMAKE_CXX_FLAGS}")
    #include_directories(BEFORE ${TOOLCHAIN_DIRECTORY}/netflix/include/ffmpeg2)
    list(APPEND LIBRARIES
        ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libavformat.a
        ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libavcodec.a
        ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libswscale.a
        ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libswresample.a
        ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libavutil.a)
endif()

# Video Renderer
# --------------------------------------------------------------
if (DPI_REFERENCE_VIDEO_RENDERER STREQUAL "auto") #default
    if(DPI_NAME STREQUAL "reference-sink")
        set(DPI_REFERENCE_VIDEO_RENDERER "sink")
    elseif(GIBBON_PLATFORM STREQUAL "rpi")
        set(DPI_REFERENCE_VIDEO_RENDERER "openmax-il")
    #elseif(GIBBON_GRAPHICS)
    #    set(DPI_REFERENCE_VIDEO_RENDERER "${GIBBON_GRAPHICS}")
    else()
        set(DPI_REFERENCE_VIDEO_RENDERER "directfb")
    endif()
endif()
if(DPI_REFERENCE_VIDEO_RENDERER STREQUAL "sink")
    add_definitions("-DBUILD_SINK_VIDEO_RENDERER")
    list(APPEND SOURCES sink/VideoRendererSink.cpp)
elseif(DPI_REFERENCE_VIDEO_RENDERER STREQUAL "openmax-il")
    add_definitions("-DBUILD_OPENMAXIL_VIDEO_RENDERER")
    list(APPEND SOURCES sink/VideoRendererSink.cpp)
elseif(DPI_REFERENCE_VIDEO_RENDERER STREQUAL "gles2-glx")
    add_definitions("-DBUILD_OPENGL_VIDEO_RENDERER")
    list(APPEND SOURCES x86/VideoRendererOpenGL.cpp)
    list(APPEND LIBRARIES ${OPENGL_gl_LIBRARY})
elseif(DPI_REFERENCE_VIDEO_RENDERER STREQUAL "gles2-egl")
    add_definitions("-DBUILD_OPENGL_VIDEO_RENDERER")
    list(APPEND SOURCES x86/VideoRendererOpenGL.cpp)
    list(APPEND LIBRARIES ${OPENGL_gl_LIBRARY})
#elseif(DPI_REFERENCE_VIDEO_RENDERER STREQUAL "sdl")
#    check_thirdparty(NAME SDL DESCRIPTION "SDL Video" HEADER SDL/SDL.h LIBRARY SDL)
#if (HAVE_SDL_LIBRARY)
#        add_dpi_plugin(SdlVideo SOURCES VideoRendererSDL.cpp LIBRARIES SDL)
#    endif()
else()
    add_definitions("-DBUILD_DIRECTFB_VIDEO_RENDERER")
    list(APPEND SOURCES x86/VideoRendererDFB.cpp)
    include_directories(${TOOLCHAIN_DIRECTORY}/netflix/include/directfb)
    list(APPEND LIBRARIES directfb direct fusion)
endif()

# Audio Decoder
# --------------------------------------------------------------
if (DPI_REFERENCE_AUDIO_DECODER STREQUAL "auto") #default
    if(DPI_NAME STREQUAL "reference-sink")
        set(DPI_REFERENCE_AUDIO_DECODER "sink")
    elseif(GIBBON_PLATFORM STREQUAL "rpi")
        set(DPI_REFERENCE_AUDIO_DECODER "ffmpeg") #software, for now
    else()
        set(DPI_REFERENCE_AUDIO_DECODER "ffmpeg")
    endif()
endif()
if(DPI_REFERENCE_AUDIO_DECODER STREQUAL "sink")
    add_definitions("-DBUILD_SINK_AUDIO_DECODER")
    list(APPEND SOURCES sink/AudioDecoderSink.cpp)
elseif(DPI_REFERENCE_AUDIO_DECODER STREQUAL "openmax-il")
    set(BUILD_OPENMAX TRUE)
    add_definitions("-DBUILD_OPENMAXIL_AUDIO_DECODER")
    list(APPEND SOURCES openmax-il/AudioDecoderOMXIL.cpp)
else()
    add_definitions("-DBUILD_FFMPEG_AUDIO_DECODER")
    list(APPEND SOURCES x86/AudioDecoderNative.cpp)
    set(CMAKE_C_FLAGS "-I${TOOLCHAIN_DIRECTORY}/netflix/include/ffmpeg2 ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-I${TOOLCHAIN_DIRECTORY}/netflix/include/ffmpeg2 ${CMAKE_CXX_FLAGS}")
    #include_directories(BEFORE ${TOOLCHAIN_DIRECTORY}/netflix/include/ffmpeg2)
    list(APPEND LIBRARIES
        ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libavcodec.a
        ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libswresample.a
        ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libavutil.a
    )
    if(EXISTS ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libx264.a) # rpi :(
        list(APPEND LIBRARIES ${TOOLCHAIN_DIRECTORY}/netflix/lib/ffmpeg2/libx264.a)
    endif()
    if(EXISTS ${TOOLCHAIN_DIRECTORY}/netflix/lib/dcv_sub.a) # Dolby library
        add_library(ddplus STATIC IMPORTED)
        set_property(TARGET ddplus PROPERTY IMPORTED_LOCATION ${TOOLCHAIN_DIRECTORY}/netflix/lib/dcv_sub.a)
        list(APPEND LIBRARIES ddplus)
        add_definitions("-DHAVE_DDPLUS=1")
    endif()
endif()

# Audio Renderer
# --------------------------------------------------------------
if (DPI_REFERENCE_AUDIO_RENDERER STREQUAL "auto") #default
    if(DPI_NAME STREQUAL "reference-sink")
        set(DPI_REFERENCE_AUDIO_RENDERER "sink")
    elseif(GIBBON_PLATFORM STREQUAL "rpi")
        set(DPI_REFERENCE_AUDIO_RENDERER "openmax-il")
    else()
        set(DPI_REFERENCE_AUDIO_RENDERER "portaudio")
    endif()
endif()
if(DPI_REFERENCE_AUDIO_RENDERER STREQUAL "sink")
    add_definitions("-DBUILD_SINK_AUDIO_RENDERER")
    list(APPEND SOURCES sink/AudioRendererSink.cpp)
elseif(DPI_REFERENCE_AUDIO_RENDERER STREQUAL "openmax-il")
    set(BUILD_OPENMAX TRUE)
    add_definitions("-DBUILD_OPENMAXIL_AUDIO_RENDERER")
    list(APPEND SOURCES openmax-il/AudioRendererOMXIL.cpp)
else()
    add_definitions("-DBUILD_PORTAUDIO_AUDIO_RENDERER")
    list(APPEND SOURCES x86/AudioRendererPA.cpp)
    list(APPEND LIBRARIES portaudio)
    if(NOT APPLE) # PortAudio on Linux requires ALSA
        find_package(ALSA REQUIRED)
        string(REGEX REPLACE "/alsa$" "" ALSA_INCLUDE_DIRS "${ALSA_INCLUDE_DIRS}")
        list(APPEND LIBRARIES ${ALSA_LIBRARIES})

        add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/include/alsa"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${ALSA_INCLUDE_DIRS}/alsa" "${CMAKE_BINARY_DIR}/include/alsa"
        )

        add_custom_target(referencedpi_alsa_links DEPENDS "${CMAKE_BINARY_DIR}/include/alsa")
        list(APPEND DEPENDENCIES "referencedpi_alsa_links")
    endif()
endif()

#Audio mixer
if (DPI_REFERENCE_AUDIO_MIXER STREQUAL "auto") #default
    if(DPI_NAME STREQUAL "reference-sink")
        set(DPI_REFERENCE_AUDIO_MIXER "none")
    elseif(GIBBON_PLATFORM STREQUAL "rpi")
        set(DPI_REFERENCE_AUDIO_MIXER "none") #implement #####
    elseif(APPLE)
        set(DPI_REFERENCE_AUDIO_MIXER "openal")
    else()
        set(DPI_REFERENCE_AUDIO_MIXER "alsa")
    endif()
endif()
if(DPI_REFERENCE_AUDIO_MIXER STREQUAL "none")
    #nothing...
elseif(DPI_REFERENCE_AUDIO_MIXER STREQUAL "openal")
    # ffmpeg and alsa or openal audio mixer
    list(APPEND SOURCES x86/AudioMixerNative.cpp)
    add_definitions("-DHAVE_OPENAL=1")
    add_definitions("-DAUDIO_MIXER_NATIVE=1")
    find_package(OpenAL)
    list(APPEND LIBRARIES ${OPENAL_LIBRARY})
elseif(DPI_REFERENCE_AUDIO_MIXER STREQUAL "audioqueue")
    list(APPEND SOURCES
        ${DPI_COMMON}/AudioMixerSoftware/AudioMixerSoftware.cpp
        ${DPI_COMMON}/AudioMixerSoftware/AudioMixerThread.cpp
        ${DPI_COMMON}/AudioMixerSoftware/AudioMixerDecoderVorbis.cpp
        x86/AudioMixerOutputAudioQueue.cpp
    )
    add_definitions("-DAUDIO_MIXER_OUTPUT_AUDIOQUEUE=1")
    add_definitions("-DAUDIO_MIXER_SOFTWARE=1")
    list(APPEND LIBRARIES vorbisidec ogg)
else()
    list(APPEND SOURCES
        ${DPI_COMMON}/AudioMixerSoftware/AudioMixerSoftware.cpp
        ${DPI_COMMON}/AudioMixerSoftware/AudioMixerThread.cpp
        ${DPI_COMMON}/AudioMixerSoftware/AudioMixerDecoderVorbis.cpp
        x86/AudioMixerOutputALSA.cpp
    )
    add_definitions("-DAUDIO_MIXER_OUTPUT_ALSA=1")
    add_definitions("-DAUDIO_MIXER_SOFTWARE=1")
    list(APPEND LIBRARIES vorbisidec ogg)
endif()

#Text to speech
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
    else()
        set(DPI_REFERENCE_TTS "none")
    endif()
endif()

if(DEFINED DPI_REFERENCE_DESKTOP AND EXISTS /lib/i386-linux-gnu/libc.so.6)
    list(APPEND LIBRARIES /lib/i386-linux-gnu/libc.so.6)
    if(EXISTS /lib/i386-linux-gnu/ld-linux.so.2)
        list(APPEND LIBRARIES /lib/i386-linux-gnu/ld-linux.so.2)
    endif()
 endif()

if(BUILD_OPENMAX)
    add_definitions("-DOMX_SKIP64BIT")
    include_directories(${TOOLCHAIN_DIRECTORY}/netflix/include/omx)
    list(APPEND LIBRARIES openmaxil)
    list(APPEND SOURCES openmax-il/ComponentOMXIL.cpp)
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

# Ivona
# --------------------------------------------------------------
if(DPI_REFERENCE_TTS STREQUAL "ivona")
    add_custom_command(TARGET nrddpi POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${HAVE_DPI_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${TOOLCHAIN_DIRECTORY}/netflix/share/ivona" "${HAVE_DPI_DIRECTORY}/ivona"
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
add_component_option_text(COMPONENT nrddpi DESCRIPTION "Audio Renderer" VALUE "${DPI_REFERENCE_AUDIO_RENDERER}")
add_component_option_text(COMPONENT nrddpi DESCRIPTION "Audio Decoder" VALUE "${DPI_REFERENCE_AUDIO_DECODER}")
add_component_option_text(COMPONENT nrddpi DESCRIPTION "Audio Mixer" VALUE "${DPI_REFERENCE_AUDIO_MIXER}")
add_component_option_text(COMPONENT nrddpi DESCRIPTION "Video Decoder" VALUE "${DPI_REFERENCE_VIDEO_DECODER}")
add_component_option_text(COMPONENT nrddpi DESCRIPTION "Video Renderer" VALUE "${DPI_REFERENCE_VIDEO_RENDERER}")
add_component_option_text(COMPONENT nrddpi DESCRIPTION "Text To Speech Engine" VALUE "${DPI_REFERENCE_TTS}")

message(" ")
message("=============================================================")
message(" Reference DPI configuration:")
message("-------------------------------------------------------------")

print_component_options(nrddpi "  - ")

message(" ")
message("  - DRM library: ${DPI_REFERENCE_DRM}")
message("=============================================================")
message(" ")


