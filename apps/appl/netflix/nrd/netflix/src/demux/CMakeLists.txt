# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

project(nrddemux)

set(SOURCES
    Mp4Parser/Base.cpp
    Mp4Parser/Context.cpp
    Mp4Parser/Reader.cpp
    Mp4Parser/Dumper.cpp
    Mp4Parser/ISOBoxes.cpp
    Mp4Parser/AVCBoxes.cpp
    Mp4Parser/MP4Boxes.cpp
    Mp4Parser/MP4Descriptors.cpp
    Mp4Parser/PIFFBoxes.cpp
    Mp4Parser/NFLXBoxes.cpp
    Mp4Parser/Mp4Parser.cpp
    Mp4Parser/HEVCBoxes.cpp
    Mp4Parser/DolbyBoxes.cpp
    Mp4Parser/SampleGroupEntries.cpp
)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(nrddemux ${SOURCES} ${HEADERS})
target_link_libraries(nrddemux nrdbase)

# Public headers
set(PUBLIC_HEADERS
    Mp4Parser/Context.h
    Mp4Parser/Mp4Parser.h
    Mp4Parser/Reader.h
    Mp4Parser/Base.h
)

create_install_target(
    TARGET nrddemux
    DESCRIPTION "Demux library"
    COMPONENT "NRDP"
    PUBLIC_HEADERS "${PUBLIC_HEADERS}"
)
