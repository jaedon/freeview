# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

add_definitions(
   -DREE_DRM_DEBUG
)
file(GLOB_RECURSE HEADERS *.h)
process_headers(TARGET nrddpi COMPONENT NRDP PUBLIC_HEADERS ${HEADERS})

set(DPI_COMMON_FILES "")
set(DPI_COMMON_INCLUDE_DIRECTORIES "")
set(DPI_COMMON_DEFINITIONS "")

if(BUILD_SSO)
    list(APPEND DPI_COMMON_DEFINITIONS "-DBUILD_SSO")
    list(APPEND DPI_COMMON_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/sso")
    list(APPEND DPI_COMMON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/sso/SingleSignOnBridge.cpp)
endif()

if(BUILD_VOICE)
    list(APPEND DPI_COMMON_DEFINITIONS "-DBUILD_VOICE")
    list(APPEND DPI_COMMON_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/voice")
    list(APPEND DPI_COMMON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/voice/VoiceBridge.cpp)
endif()

if(BUILD_SIGNUP)
    list(APPEND DPI_COMMON_DEFINITIONS "-DBUILD_SIGNUP")
    list(APPEND DPI_COMMON_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/signup")
    list(APPEND DPI_COMMON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/signup/SignupBridge.cpp)
endif()

set(DPI_COMMON_DEFINITIONS "${DPI_COMMON_DEFINITIONS}" PARENT_SCOPE)
set(DPI_COMMON_FILES "${DPI_COMMON_FILES}" PARENT_SCOPE)
set(DPI_COMMON_INCLUDE_DIRECTORIES "${DPI_COMMON_INCLUDE_DIRECTORIES}" PARENT_SCOPE)
