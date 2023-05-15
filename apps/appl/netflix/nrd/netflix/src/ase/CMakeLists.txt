# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)
project(ase)

include_directories(common hrm)
if(NRDP_ASE)
  include_directories(streamingmanager heuristics/netflix)

  set(GROUP_ASE "Streaming engine options")
  set(ASE_HEURISTICS "netflix" CACHE STRING "Heuristics implementation to use")

  add_subdirectory(heuristics/${ASE_HEURISTICS})
endif()

# ----------------------------------------------------------------------------------
# COMMON / HRMpp
# ----------------------------------------------------------------------------------
set(SOURCES
  common/AseBuffer.cpp
  common/AseCommonUtil.cpp
  common/AseTimeStamp.cpp
  common/ByteRange.cpp
  common/DebugLogging.cpp
  common/HeapBuffer.cpp
  common/HttpAttempt.cpp
  common/HttpPipeliningSupportDetector.cpp
  common/IAseConfigParameter.cpp
  common/StreamersLog.cpp

  hrm/HttpRequestManager.cpp
  hrm/AverageResponseTime.cpp
  hrm/AverageThroughput.cpp
  hrm/BaseTrackDownloader2.cpp
  hrm/ByteRangeCache.cpp
  hrm/ByteRangeDownloader.cpp
  hrm/ByteRangeDownloaderStates.cpp
  hrm/ByteRanges.cpp
  hrm/EventsPerSecond.cpp
  hrm/McplTrackDownloader2.cpp
  hrm/MediaRequestTask.cpp
  hrm/ScorTrackDownloader2.cpp
  hrm/UrlRedirectMap.cpp
  hrm/UrlRequestTask2.cpp

  # fixme: these files should get moved to common?
  streamingmanager/BoxReader.cpp
  streamingmanager/EditList.cpp
  streamingmanager/IMediaFile.cpp
  streamingmanager/MoofBoxReader.cpp
  streamingmanager/MoovBoxReader.cpp
  )

set(HEADERS
  # fixme: these files should get moved to common?
  streamingmanager/Reader.h
  streamingmanager/BoxReader.h
  streamingmanager/MoofBoxReader.h
  streamingmanager/MoovBoxReader.h
  )

file(GLOB_RECURSE COMMON_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/common/*.h)
set(HEADERS ${HEADERS} ${COMMON_HEADERS})

file(GLOB_RECURSE HRM_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/hrm/*.h)
set(HEADERS ${HEADERS} ${HRM_HEADERS})

set(LIB_DEPS cares nrdnet_http nrdnet)

# Public headers
set(PUBLIC_HEADERS
  common/AseBuffer.h
  common/AseDataBuffer.h
  common/AseCommonDataTypes.h
  common/AseCommonUtil.h
  common/AseTimeStamp.h
  common/ByteRange.h
  common/DebugLogging.h
  common/HttpAttempt.h
  common/IAseConfigParameter.h
  common/BitrateRange.h
  common/IMediaBufferPool.h
  common/IPersistentStore.h
  common/HttpPipeliningSupportDetector.h
  common/HeapBuffer.h

  hrm/AverageThroughput.h
  hrm/ByteRanges.h
  hrm/HttpRequestManager.h
  hrm/IMediaConsumer.h
  hrm/IThroughputMonitor.h
  hrm/ITransportReporter.h
  hrm/UrlRequestTask.h
  hrm/MediaRequestTask.h

  # fixme: these files should get moved to common?
  streamingmanager/BoxReader.h
  streamingmanager/EditList.h
  streamingmanager/MoovBoxReader.h
  streamingmanager/MoofBoxReader.h
  )

# ----------------------------------------------------------------------------------
# ASE
# ----------------------------------------------------------------------------------
if(NRDP_ASE)

  set(SOURCES
    ${SOURCES}
    streamingmanager/DashIsoMediaFile.cpp
    streamingmanager/EWMAThroughput.cpp
    streamingmanager/FreeBoxReader.cpp
    streamingmanager/FtypBoxReader.cpp
    streamingmanager/IsoMediaFile.cpp
    streamingmanager/IsoMediaFileRequest.cpp
    streamingmanager/Manifest.cpp
    streamingmanager/MediaFileFactory.cpp
    streamingmanager/MediaFragment.cpp
    streamingmanager/MediaPresentation2.cpp
    streamingmanager/MediaStream2.cpp
    streamingmanager/MediaStreamRequest.cpp
    streamingmanager/MediaTrack2.cpp
    streamingmanager/MfraBoxReader.cpp
    streamingmanager/MimeCodec.cpp
    streamingmanager/MimeType.cpp
    streamingmanager/NetworkMonitor.cpp
    streamingmanager/NflxHeaderReader.cpp
    streamingmanager/NflxIndexReader.cpp
    streamingmanager/NflxIsoMediaFile.cpp
    streamingmanager/PrintManifest.cpp
    streamingmanager/SidxReader.cpp
    streamingmanager/StreamingCommand.cpp
    streamingmanager/StreamingCommandQueue.cpp
    streamingmanager/StreamingManager2.cpp
    streamingmanager/TrackBufferInfo.cpp
    )

  file(GLOB_RECURSE HEURISTICS_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/heuristics/*.h)
  set(HEADERS ${HEADERS} ${HEURISTICS_HEADERS})

  file(GLOB_RECURSE SM_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/streamingmanager/*.h)
  set(HEADERS ${HEADERS} ${SM_HEADERS})

  set(LIB_DEPS ${LIB_DEPS} nrdase_heuristics)

  set(PUBLIC_HEADERS
    ${PUBLIC_HEADERS}
    streamingmanager/AseTimeRange.h
    streamingmanager/IConnectionConfiguration.h
    streamingmanager/IMediaFragment.h
    streamingmanager/IMediaSink.h
    streamingmanager/IMediaFile.h
    streamingmanager/INetworkMonitor.h
    streamingmanager/NetworkMonitor.h
    streamingmanager/ILocationSetMonitor.h
    streamingmanager/IThroughputPredictor.h
    streamingmanager/ILocationSelector.h
    streamingmanager/IStreamingHeuristic.h
    streamingmanager/IStreamingReporter.h
    streamingmanager/IStreamingStatistics.h
    streamingmanager/IStreamSelector.h
    streamingmanager/ITrackBufferInfo.h
    streamingmanager/ITrackInfo.h
    streamingmanager/ILocationSetMonitorFactory.h

    streamingmanager/Manifest.h
    streamingmanager/MimeCodec.h
    streamingmanager/MimeType.h
    streamingmanager/MediaFileFactory.h
    streamingmanager/PrintManifest.h
    streamingmanager/StartplayPoint.h
    streamingmanager/StreamingManager.h
    streamingmanager/MediaStreamRequest.h
    streamingmanager/MediaPresentationRequest.h

    heuristics/netflix/IStreamingLocationSet.h
    heuristics/netflix/SimpleConnectionConfiguration.h
    )

endif()

add_library(nrdase ${SOURCES} ${HEADERS})
target_link_libraries(nrdase ${LIB_DEPS})

create_install_target(
  TARGET nrdase
  DESCRIPTION "StreamingEngine library"
  COMPONENT "NRDP"
  PUBLIC_HEADERS "${PUBLIC_HEADERS}"
  )
