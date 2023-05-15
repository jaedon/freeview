cmake_minimum_required(VERSION 2.8)

set(SOCKETIZER_CONFIG_FILES
    SocketizerIPAddress.cpp
    SocketizerLog.cpp
    Socketizer.cpp
)

set(PUBLIC_HEADERS
    Socketizer.h
)

add_library(nrdsocketizer SHARED ${SOCKETIZER_CONFIG_FILES})
target_link_libraries(nrdsocketizer ${SOCKETIZER_CONFIG_SYSTEM_LIBRARIES})

create_install_target(
    TARGET nrdsocketizer
    DESCRIPTION "Socketizer configuration library"
    COMPONENT "NRDP"
    PUBLIC_HEADERS "${PUBLIC_HEADERS}"
)
