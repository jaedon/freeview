# (c) 1997-2012 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

include_directories(inc)

set(SRC_FILELIST
    inc/FreeList.h
    inc/ithread.h
    inc/LinkedList.h
    inc/ThreadPool.h
    inc/TimerThread.h

    src/FreeList.c
    src/LinkedList.c
    src/ThreadPool.c
    src/TimerThread.c
)

add_library(mdxlib_threadutil ${SRC_FILELIST})

create_install_target(
    TARGET mdxlib_threadutil
    DESCRIPTION "Netflix MdxLib Thead library"
    COMPONENT "MDXLIB"
    PUBLIC_HEADERS "${PUBLIC_HEADERS}"
)
