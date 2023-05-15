# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

project(nrd)

include_directories(Core)
include_directories(${NRDP_3RDPARTY_DIRECTORY}/mongoose)

include_directories(Configuration Core Dpi Logging MediaControl NBP QA Utilities)

#=====================================================================================================

set(SOURCES
    Configuration/ConfigurationKeys.cpp
    Configuration/DiskStore.cpp
    Configuration/NrdWriteLimiter.cpp
    Configuration/SecureStore.cpp
    Configuration/SystemData.cpp
    Configuration/SystemDataKey.cpp

    Core/AppLog.cpp
    Core/AppThread.cpp
    Core/EventSourceEventConnection.cpp
    Core/NrdApplication.cpp
    Core/NrdConfiguration.cpp
    Core/NrdDiskCache.cpp

    Dpi/IDeviceError.cpp

    Logging/InstrumentationThread.cpp
    Logging/NrdLog.cpp

    MediaControl/AppendableMediaRequest.cpp
    MediaControl/BufferedMediaRequest.cpp
    MediaControl/JsBridgeMediaRequest2.cpp
    MediaControl/LocalConfigParameter.cpp
    MediaControl/McMediaBufferPool.cpp
    MediaControl/MediaRequestManager.cpp
    MediaControl/MediaSourceAppendable.cpp
    MediaControl/NrdpMedia.cpp
    MediaControl/NrdpMediaEventThread.cpp
    MediaControl/NrdpMediaSource.cpp
    MediaControl/NrdpMediaSourceBuffer.cpp
    MediaControl/PlaybackDeviceError.cpp
    MediaControl/PlayerState.cpp
    MediaControl/StreamingConfigParameter.cpp
    MediaControl/StreamingPersistentStore.cpp
    MediaControl/DrmManager.cpp

    NBP/EventConnection.cpp
    NBP/NfObject.cpp
    NBP/Request.cpp
    NBP/Response.cpp
    NBP/bridge/NetworkBridge.cpp
    NBP/bridge/SystemBridge.cpp
    NBP/bridge/AudioBridge.cpp
    NBP/bridge/DeviceBridge.cpp
    NBP/bridge/DrmSystemBridge.cpp
    NBP/bridge/InstrumentationBridge.cpp
    NBP/bridge/LogBridge.cpp
    NBP/bridge/MediaRequestBridge.cpp
    NBP/bridge/MediaSourcePlayerBridge.cpp
    NBP/bridge/MediaSourcePlayerFactoryBridge.cpp
    NBP/bridge/NrdpBridge.cpp
    NBP/bridge/StorageBridge.cpp
    NBP/bridge/TextToSpeechBridge.cpp
    NBP/bridge/TransportReporter.cpp
    NBP/bridge/WebCryptoBridge.cpp
    NBP/bridge/WebSocketBridge.cpp

    StreamParser/MediaSourceStreamParser.cpp
    StreamParser/MediaSourceStreamParserContext.cpp

    Utilities/NrdIpConfigurationListener.cpp
    Utilities/NrdIpConnectivityManager.cpp
)

set(NRDP_JS_FILES
    # this must always be first
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/nrdp.js

    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/audio.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/device.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/drmsystem.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/instrumentation.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/log.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/mediarequest.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/mediasourceplayer.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/storage.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/texttospeech.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/webcrypto.js
    ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/websocket.js
)

add_component_option(NRDP_WEBSERVER_ENABLED "[-nrd-webserver] Enable webserver for nrd" ON nrd)
add_component_option(NRDP_HAS_SOCKETIZER "[-socketizer] Build with socketizer support" OFF nrd)
add_component_option(NRDP_MDX_ENABLED "[-mdx] Build with mdx enabled" ON nrd)

set(NRDP_LIBRARIES nrdbase nrdnet nrdase expat ssl crypto curl cares z)

if (NRDP_HAS_PARTNER_INTEGRATION)
  set(PUBLIC_HEADERS ${PUBLIC_HEADERS} ../integration/IntegrationWriter.h ../integration/IntegrationWriterErrorCodesInternal.h)
  set(NRDP_JS_FILES ${NRDP_JS_FILES} ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/tile.js)
  set(SOURCES ${SOURCES} NBP/bridge/TileBridge.cpp ../integration/IntegrationWriter.cpp ../integration/Buffer.cpp)
endif ()

if (NRDP_HAS_ESPLAYER)
    include_directories(PlaybackDevice)

    set(SOURCES ${SOURCES}
        PlaybackDevice/DeviceConstants.cpp
        PlaybackDevice/EventQueue.cpp
        PlaybackDevice/SampleAttributes.cpp
        PlaybackDevice/Demultiplexers/Mp4Demultiplexer.cpp
        PlaybackDevice/Demultiplexers/StreamParser.cpp
        PlaybackDevice/MediaSourcePlaybackDevice.cpp
        PlaybackDevice/MediaSourceESPlayerCallback.cpp
      )

    if (NRDP_TOOLS STREQUAL "none") # do not enable this when tool option is specified
        if (NRDP_MCSIM_DUMP)
            add_definitions(-DENABLE_MCSIM_INPUT_DUMP)
            include_directories(../../tools/MediaControlSim/)
            set(SOURCES ${SOURCES}
                ../../tools/MediaControlSim/MCSimInputFileMaker.cpp
              )
        endif ()
    endif ()

endif ()

if (BUILD_QA)
    set(NRDP_JS_FILES ${NRDP_JS_FILES} ${CMAKE_CURRENT_LIST_DIR}/QA/QABridge.js)
    list(APPEND SOURCES QA/QATextToSpeechBridge.cpp)
endif ()

if (NRDP_ASE)
  set(SOURCES ${SOURCES}
    MediaControl/ASPlayer.cpp
    MediaControl/AdaptiveStreamingPlayer.cpp
    MediaControl/EventThread.cpp
    MediaControl/McMediaSink.cpp
    MediaControl/MediaFileCache.cpp
    MediaControl/PumpingThread.cpp
    NBP/bridge/PlayerBridge.cpp
    Utilities/ManifestUtilities.cpp
    )

  set(NRDP_JS_FILES ${NRDP_JS_FILES} ${CMAKE_CURRENT_LIST_DIR}/NBP/bridge/player.js)

  if (NRDP_HAS_ESPLAYER)
    set(SOURCES ${SOURCES} PlaybackDevice/PlaybackDevice.cpp PlaybackDevice/ESPlayerCallback.cpp)
  endif ()

  if (BUILD_QA)
    set(SOURCES ${SOURCES} QA/QALocationMonitorBridge.cpp)
  endif ()
endif ()

if (NRDP_HAS_SOCKETIZER)
    add_subdirectory(Socketizer)
endif ()

if (NRDP_HAS_ESPLAYER)
  set(NRDP_LIBRARIES ${NRDP_LIBRARIES} nrddemux)
endif ()

# Public headers
set(PUBLIC_HEADERS
  ${PUBLIC_HEADERS}
    ${NRDP_3RDPARTY_DIRECTORY}/mongoose/mongoose.h
    ${NRDP_BINARY_ROOT_DIR}/include/nrd/config.h
    Configuration/ConfigurationKeys.h
    Configuration/NrdWriteLimiter.h
    Configuration/SystemDataKey.h
    Core/AppLog.h
    Core/AppThread.h
    Core/ApplicationEvent.h
    Core/NrdApplication.h
    Core/NrdConfiguration.h
    Core/NrdDiskCache.h
    Dpi/IAudioMixer.h
    Dpi/IAudioMixerOutput.h
    Dpi/IBufferManager.h
    Dpi/IDeviceError.h
    Dpi/IDeviceLib.h
    Dpi/IDrmSession.h
    Dpi/IDrmSystem.h
    Dpi/IElementaryStreamPlayer.h
    Dpi/IPlaybackDevice.h
    Dpi/ISecureStop.h
    Dpi/ISystem.h
    Dpi/ITextToSpeech.h
    Dpi/IWebCrypto.h
    Logging/NrdLog.h
    Logging/IMediaPipelineReporter.h
    MediaControl/AppendableMediaRequest.h
    MediaControl/BufferedMediaRequest.h
    MediaControl/DrmManager.h
    MediaControl/IAdaptiveStreamingPlayer.h
    MediaControl/IJsBridgeMediaRequestListener.h
    MediaControl/IMediaSourcePlayer.h
    MediaControl/JsBridgeMediaRequest2.h
    MediaControl/LocalConfigParameter.h
    MediaControl/MediaRequestManager.h
    MediaControl/MediaRequestManagerListener.h
    MediaControl/MediaSourceAppendable.h
    MediaControl/NrdpMediaSource.h
    MediaControl/NrdpMediaSourceBuffer.h
    MediaControl/Rect.h
    MediaControl/StreamingConfigParameter.h
    MediaControl/Ticks.h
    NBP/EventConnection.h
    NBP/NfObject.h
    NBP/Request.h
    NBP/Response.h
    PlaybackDevice/MediaSourcePlaybackDevice.h
    PlaybackDevice/SampleAttributes.h
    PlaybackDevice/StreamAttributes.h
    StreamParser/MediaSourceStreamParser.h
)

set(PRIVATE_HEADERS
    Configuration/SecureStore.h
)


if (NRDP_HAS_ESPLAYER)
    set(PUBLIC_HEADERS ${PUBLIC_HEADERS}
        PlaybackDevice/DeviceConstants.h
    )
endif ()

set(PUBLIC_HEADERS ${PUBLIC_HEADERS}
    MediaControl/IAdaptiveStreamingPlayer.h
    MediaControl/McMediaBufferPool.h
)

# Documentation
foreach(HEADER ${PUBLIC_HEADERS})
    if (HEADER MATCHES "Dpi/")
        list(APPEND DPI_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}")
    else()
        list(APPEND NRD_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${HEADER}")
    endif ()
endforeach()

add_documentation("nrd" INPUT ${NRD_HEADERS} TITLE "NRDP SDK API Reference")
add_documentation("nrddpi" INPUT ${DPI_HEADERS} TITLE "NRDP SDK DPI Reference")

file(GLOB_RECURSE HEADERS *.h)
set(SOURCES ${SOURCES} ${HEADERS})
set(NRD_MDX_LIBS "")
if (NRDP_MDX_ENABLED)
    set(NRDP_JS_FILES ${NRDP_JS_FILES} ${CMAKE_CURRENT_LIST_DIR}/mdx/resources/js/nrdpmdx.js)
    include(${NRDP_3RDPARTY_DIRECTORY}/upnp_nflx/upnp_nflx.cmake)
    set_property(SOURCE ${NRDP_3RDPARTY_DIRECTORY}/mongoose_mdx/mongoose_mdx.c PROPERTY COMPILE_DEFINITIONS NO_POPEN NO_SSL NO_SSL_DL NO_CGI NO_SSI BUFSIZ=4096)
    add_library(mdx_3rdparty STATIC ${NRDP_3RDPARTY_DIRECTORY}/mongoose_mdx/mongoose_mdx.c ${NETFLIX_UPNP_SOURCES})
    create_install_target(TARGET mdx_3rdparty DESCRIPTION "mdx 3rdparty libraries" COMPONENT "NRDP")

    set(NRD_MDX_LIBS mdx_3rdparty)
    set(SOURCES
        ${SOURCES}
        mdx/Mdx.cpp
        mdx/MdxCryptContext.cpp
        mdx/MdxInternal.cpp
        mdx/MdxRegistry.cpp
        mdx/MdxUtil.cpp
        mdx/MdxGuard.cpp
        mdx/MdxMongooseNFLX.cpp
        mdx/MdxHttpClient.cpp
        mdx/MdxTarget.cpp
        mdx/MdxController.cpp
        mdx/MdxConfiguration.cpp
        mdx/MdxLog.cpp
        mdx/MdxNetworkManager.cpp
        mdx/MdxWebSocketConnections.cpp
        mdx/WebSocketMdx.cpp
        mdx/WebSocketEcho.cpp
        mdx/MdxBridge.cpp
        mdx/MdxError.cpp)

   set(PUBLIC_HEADERS
       ${PUBLIC_HEADERS}
       mdx/Mdx.h
       mdx/IMdx.h
       mdx/MdxBridge.h
       mdx/MdxError.h
       mdx/MdxConfiguration.h
       mdx/MdxVersion.h)

    include_directories(${NRDP_3RDPARTY_DIRECTORY}/mongoose_mdx)
endif ()

# Do not concatenate here. Pass on the raw list of files instead
set_property(GLOBAL APPEND PROPERTY JS_EXPORTS ${NRDP_JS_FILES})

add_library(nrd ${SOURCES} ${HEADERS})
target_link_libraries(nrd ${NRD_MDX_LIBS})

create_install_target(
    TARGET nrd
    DESCRIPTION "NRDP library"
    COMPONENT "NRDP"
    PUBLIC_HEADERS ${PUBLIC_HEADERS}
    PRIVATE_HEADERS ${PRIVATE_HEADERS}
)

# -----------------------------------------------------------------------------------------------------------
# valgrind

    set(VALGRIND_TEST "
    #include </usr/include/valgrind/memcheck.h>
    int main()
    {
        VALGRIND_DO_CHANGED_LEAK_CHECK;
        return 0;
    }
    ")
    check_c_source_compiles("${VALGRIND_TEST}" NRDP_HAS_VALGRIND)
# -----------------------------------------------------------------------------------------------------------

if (NRDP_WEBSERVER_ENABLED)
  set(NRDP_LIBRARIES mongoose ${NRDP_LIBRARIES})
endif ()

target_link_libraries(nrd ${NRDP_LIBRARIES})

set(NRDP_HAS_SOCKETIZER "${NRDP_HAS_SOCKETIZER}" PARENT_SCOPE)

configure_file_if_different(config.h.in ${NRDP_BINARY_ROOT_DIR}/include/nrd/config.h)

