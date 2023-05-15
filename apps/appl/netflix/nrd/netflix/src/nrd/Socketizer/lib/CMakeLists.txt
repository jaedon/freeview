cmake_minimum_required(VERSION 2.8)

set(SOCKETIZER_FILES
    Socketizer.cpp
)

include_directories(../)

add_library(socketizer SHARED ${SOCKETIZER_FILES})
target_link_libraries(socketizer nrdsocketizer)

create_install_target(
    TARGET socketizer
    DESCRIPTION "Socketizer interposer library"
    COMPONENT "NRDP"
)
