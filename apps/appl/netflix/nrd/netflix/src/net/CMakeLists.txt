# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

set(GROUP_NET "Networking library options")
set(NET_HTTP "sockets" CACHE STRING "HTTP implementation to use")
add_component_option(NET_FIONREAD "Enable TCP throughput measurement (FIONREAD)" ON nrd)

if(NET_FIONREAD)
    add_definitions(-DHAVE_FIONREAD)
endif()

if(APPLE)
  add_definitions(-DBIND_8_COMPAT)
endif()

project(net)

add_subdirectory(httplib/${NET_HTTP})
add_component_option_text(COMPONENT nrd DESCRIPTION "HTTP implementation" VALUE "${NET_HTTP}")

set(SOURCES
    httplib/HttpResponseHeaders.cpp

    util/AseTimeVal.cpp
    util/AseUrl.cpp
    util/CurlError.cpp
    util/CurlMultiThread.cpp
    util/IDnsCache.cpp
    util/IpConnectivityManager.cpp
    util/NrdIpAddr.cpp
    util/NetError.cpp
    util/NetConfiguration.cpp
    util/TelnetServer.cpp
    util/HttpServiceThread.cpp
    util/HttpService.cpp
    util/TrustStore.cpp
    util/TrafficShaper.cpp

    certstatus/X509Cert.cpp

    resourcemanager/ResourceManager.cpp
    resourcemanager/ResourceRequest.cpp
    resourcemanager/Resource.cpp
    resourcemanager/DiskCache.cpp

    websocket/WebSocket.cpp
    websocket/WebSocketClient.cpp
    websocket/WebSocketServer.cpp
    websocket/WebSocketThread.cpp
)

    set(SOURCES
        ${SOURCES}
        util/DnsManagerAres.cpp
        util/HttpsServiceCurl.cpp
    )

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(nrdnet ${SOURCES} ${HEADERS})
target_link_libraries(nrdnet nrdbase ssl crypto curl cares)

file(GLOB CERTSTATUS_HEADERS certstatus/*.h)
file(GLOB UTIL_HEADERS util/*.h)
file(GLOB HTTPLIB_HEADERS httplib/*.h)
file(GLOB RESOURCEMANAGER_HEADERS resourcemanager/*.h)
file(GLOB WEBSOCKET_HEADERS websocket/*.h)

list(APPEND PUBLIC_HEADERS ${CERTSTATUS_HEADERS} ${UTIL_HEADERS} ${HTTPLIB_HEADERS} ${HTTPSTREAM_HEADERS} ${RESOURCEMANAGER_HEADERS} ${WEBSOCKET_HEADERS})

create_install_target(
    TARGET nrdnet
    DESCRIPTION "Network library"
    COMPONENT "NRDP"
    PUBLIC_HEADERS "${PUBLIC_HEADERS}"
)
