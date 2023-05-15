# (c) 1997-2014 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

set(BUILD_NRDSYSTEM "${NOVA_PLATFORM}" CACHE STRING "" FORCE)

# configure gibbon paths
set(GIBBON_PARTNERBRIDGE "${CMAKE_SOURCE_DIR}/${NRDAPP_DIRECTORY}/src/platform/gibbon/platform/nova/${BUILD_NRDSYSTEM}/PartnerBridge.js" CACHE STRING "" FORCE)
set(PERL_EXECUTABLE "perl.exe")
set(PYTHON_EXECUTABLE "python.exe")

# configure nrdp options
set(BUILD_COMPILE_RESOURCES ON CACHE BOOL "" FORCE)
set(BUILD_QA ON CACHE BOOL "" FORCE)
set(GIBBON_BSDIFF OFF CACHE BOOL "" FORCE)
set(GIBBON_SURFACEDECODER_FORK OFF CACHE BOOL "" FORCE)

set(NRDP_APPLICATION "gibbon" CACHE STRING "" FORCE)
set(NRDP_BOOTSTRAP OFF CACHE BOOL "" FORCE)
set(NRDP_CLOAKWARE ON CACHE BOOL "" FORCE)
set(NRDP_ESPLAYER ON CACHE BOOL "" FORCE)
set(NRDP_HAS_ATOMICS ON CACHE BOOL "" FORCE)
set(NRDP_HAS_IPV6 OFF CACHE BOOL "" FORCE)
set(NRDP_HAS_PERFORMANCE_MARKERS ON CACHE BOOL "" FORCE)
set(NRDP_MEMTEST OFF CACHE BOOL "" FORCE)
set(NRDP_HAS_PARTNER_INTEGRATION ON CACHE BOOL "" FORCE)
set(NRDP_HAS_MULTISESSION_DRM ON CACHE BOOL "" FORCE)

if(BUILD_PS3 OR BUILD_PS4 OR BUILD_PSP2)
    set(NRDP_EXIT OFF CACHE BOOL "" FORCE)
endif()

if (NOT DEFINED ENV{JAVA_HOME})
    message(WARNING, "no JAVA_HOME environment variable set, minification of JS files disabled")
else()
    set(BUILD_SMALL ON CACHE BOOL "" FORCE)
endif()

if(BUILD_PS3)
    # Specify PS3 3D split mode
    add_definitions(-DUSE_MVC_SPLIT)
endif()

add_definitions(
    ${NRDP_DEFINITIONS}

    # JavaScriptCore internally restricts including ctype.h. However some platforms (such as PS4)
    # include it as a byproduct of std::string. Since this restriction is purely internal and does
    # not affect our usage of JavaScriptCore, use the macro guard variable to disable this restriction.
    -DWTF_DisallowCType_h
)

include_directories(
    ${CMAKE_SOURCE_DIR}/3rdParty/boost
    ${CMAKE_SOURCE_DIR}/3rdParty/cares
    ${CMAKE_SOURCE_DIR}/3rdParty/curl/include
    ${CMAKE_SOURCE_DIR}/3rdParty/expat/lib
    ${CMAKE_SOURCE_DIR}/3rdParty/graphite/include
    ${CMAKE_SOURCE_DIR}/3rdParty/openssl/include
    ${CMAKE_SOURCE_DIR}/3rdParty/zlib
    ${CMAKE_SOURCE_DIR}/3rdParty/icu/source/common
    ${CMAKE_SOURCE_DIR}/3rdParty/icu/source/i18n
)

# append nrdp compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${NRDP_COMPILER_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${NRDP_COMPILER_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_TEST "${CMAKE_C_FLAGS_TEST} ${NRDP_COMPILER_FLAGS_TEST}")
set(CMAKE_C_FLAGS_PERFORMANCE "${CMAKE_C_FLAGS_PERFORMANCE} ${NRDP_COMPILER_FLAGS_PERFORMANCE}")
set(CMAKE_C_FLAGS_PRODUCTION "${CMAKE_C_FLAGS_PRODUCTION} ${NRDP_COMPILER_FLAGS_PRODUCTION}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NRDP_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${NRDP_COMPILER_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_TEST "${CMAKE_CXX_FLAGS_TEST} ${NRDP_COMPILER_FLAGS_TEST}")
set(CMAKE_CXX_FLAGS_PERFORMANCE "${CMAKE_CXX_FLAGS_PERFORMANCE} ${NRDP_COMPILER_FLAGS_PERFORMANCE}")
set(CMAKE_CXX_FLAGS_PRODUCTION "${CMAKE_CXX_FLAGS_PRODUCTION} ${NRDP_COMPILER_FLAGS_PRODUCTION}")

# function override for libraries to link against nrd apps and libs
function(target_link_libraries)
    if(${ARGV0} STREQUAL "nrd" OR
       ${ARGV0} STREQUAL "nrdapp" OR
       ${ARGV0} STREQUAL "nrdase" OR
       ${ARGV0} STREQUAL "nrdil" OR
       ${ARGV0} STREQUAL "nrdnet" OR
       ${ARGV0} STREQUAL "nrdnet_http" OR
       ${ARGV0} STREQUAL "nrdase_http" OR
       ${ARGV0} STREQUAL "nrdbase" OR
       ${ARGV0} STREQUAL "nrdcertstatus" OR
       ((BUILD_PS4 OR BUILD_PS3 OR BUILD_WIIU ) AND (${ARGV0} STREQUAL "playready")) OR
       ${ARGV0} STREQUAL "streamingengine" OR
       ${ARGV0} STREQUAL "WTF" OR
       ${ARGV0} STREQUAL "JavaScriptCore")
    else()
        _target_link_libraries(${ARGV})
    endif()
endfunction()

if(BUILD_PS3 OR BUILD_WIIU)
    set(DPI_REFERENCE_DRM "playready2.5-ss-tee" CACHE STRING "" FORCE)
    # using obfuscated version in security/<platform>/2.5-ss-tee
elseif(BUILD_PS4)
    set(DPI_REFERENCE_DRM "playready2.5-ss-tee" CACHE STRING "" FORCE)
    add_subdirectory(3rdparty/playready/2.5-ss-tee)
endif()
