# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

file(GLOB_RECURSE HEADERS *.h WebCrypto/*.h WebCryptoTee/*.h Common/*.h Ree/*.h)
process_headers(TARGET nrddpi COMPONENT NRDP PUBLIC_HEADERS ${HEADERS})

set(REE_TEE_IMPLEMENTATION_FILES "")
set(REE_TEE_IMPLEMENTATION_INCLUDE_DIRECTORIES "")
set(REE_TEE_IMPLEMENTATION_DEFINITIONS "")

# WebCrypto
list(APPEND REE_TEE_IMPLEMENTATION_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/WebCrypto")
list(APPEND REE_TEE_IMPLEMENTATION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCrypto/AesCbcCipher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCrypto/AesGcmCipher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCrypto/AesKeyWrapper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCrypto/RsaContext.cpp
)

# WebCryptoTee
list(APPEND REE_TEE_IMPLEMENTATION_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/WebCryptoTee")
list(APPEND REE_TEE_IMPLEMENTATION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/WebCryptoTee/WebCryptoTee.cpp
)

# Ree
list(APPEND REE_TEE_IMPLEMENTATION_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/Ree")
list(APPEND REE_TEE_IMPLEMENTATION_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/Ree/common")
list(APPEND REE_TEE_IMPLEMENTATION_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/Common")
list(APPEND REE_TEE_IMPLEMENTATION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/Error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/RPCUtils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/TeeLog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/TeeApiComm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/TeeApiCrypto.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/TeeApiMgmt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/TeeApiStorage.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/TeeApiDrm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/resources/Connection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/resources/Session.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/resources/MemoryReference.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/resources/Message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/resources/MessageUtils.c
)

if(DPI_REFERENCE_DRM STREQUAL "playready2.5tee")
add_definitions(-DPLAYREADY2_5_TEE)
list(APPEND REE_TEE_IMPLEMENTATION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/playready/TeeApiDrm.c
)
endif()

if(DPI_REFERENCE_DRM STREQUAL "playready2.5-ss-tee")
add_definitions(-DPLAYREADY2_5_TEE)
list(APPEND REE_TEE_IMPLEMENTATION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Ree/playready/TeeApiDrm.c
)
endif()

set(REE_TEE_IMPLEMENTATION_DEFINITIONS "${REE_TEE_IMPLEMENTATION_DEFINITIONS}" PARENT_SCOPE)
set(REE_TEE_IMPLEMENTATION_FILES "${REE_TEE_IMPLEMENTATION_FILES}" PARENT_SCOPE)
set(REE_TEE_IMPLEMENTATION_INCLUDE_DIRECTORIES "${REE_TEE_IMPLEMENTATION_INCLUDE_DIRECTORIES}" PARENT_SCOPE)

