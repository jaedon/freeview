# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

add_library(bsdiff bsdiff.c bsdiff.h)
create_install_target(TARGET bsdiff DESCRIPTION "binary diff" COMPONENT "NRDP")

#add_executable(diff main.c)
#target_link_libraries(diff bsdiff)
