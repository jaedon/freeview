# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

set(FILES
    AsyncHttpSocketClient.cpp
    AsyncHttpSocketConnection.cpp
    AsyncHttpSocketRequest.cpp
    AsyncHttpTraceRoutePlugin.cpp
    ConnectionPoolImpl.cpp
)

add_library(nrdnet_http ${FILES})

target_link_libraries(nrdnet_http nrdbase nrdnet cares)

create_install_target(
    TARGET nrdnet_http
    DESCRIPTION "HTTP library"
    COMPONENT "NRDP"
    PUBLIC_HEADERS "${PUBLIC_HEADERS}"
)
