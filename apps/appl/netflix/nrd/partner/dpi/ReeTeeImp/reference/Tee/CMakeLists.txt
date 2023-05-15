# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

# XXX - add TEE
include_directories(../Common)
include_directories(.)
include_directories(CommAgent)
include_directories(CryptoAgent)
include_directories(MgmtAgent)
include_directories(DrmAgent)
include_directories(utils)
include_directories(TeePortingInterface)
include_directories(include)
set(TEE_SOURCES
    ../Common/Error.c
    TeePortingInterface/b64.c
    TeePortingInterface/teeUtils.c
    TeePortingInterface/teeCrypto.c
    CommAgent/TeeEntry.c
    CryptoAgent/CryptoAgent.c
    CryptoAgent/CryptoAgentDispatch.c
    CryptoAgent/KeyMgmt.c
    MgmtAgent/TeeMgr.c
    StorageAgent/StorageAgent.c
    StorageAgent/StorageAgentDispatch.c
    DrmAgent/DrmAgent.c
)

add_definitions( -DHUMAX_PLATFORM_BASE )

if(DPI_REFERENCE_DRM STREQUAL "playready2.5tee")
  add_definitions(-DPLAYREADY2_5_TEE)
  list(APPEND TEE_SOURCES DrmAgent/DrmAgentDispatchRPC.c)
endif()

if(DPI_REFERENCE_DRM STREQUAL "playready2.5-ss-tee")
  add_definitions(-DPLAYREADY2_5_TEE)
  list(APPEND TEE_SOURCES DrmAgent/DrmAgentDispatchRPC.c)
endif()

list(APPEND TEE_SOURCES TeePortingInterface/tpiStorage.c)

  add_library(nrdtee ${TEE_SOURCES})
  target_link_libraries(nrdtee ssl crypto)

  create_install_target(
      TARGET nrdtee
      DESCRIPTION "TEE Reference"
      COMPONENT "NRDP"
      PUBLIC_HEADERS ""
  )
