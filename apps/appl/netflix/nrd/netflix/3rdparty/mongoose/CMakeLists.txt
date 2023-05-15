# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

add_library(mongoose mongoose.c mongoose.h)
create_install_target(TARGET mongoose DESCRIPTION "mongoose" COMPONENT "NRDP")
set_property(SOURCE mongoose.c PROPERTY COMPILE_DEFINITIONS NO_SSL NO_CGI NO_POPEN NO_PROXY NO_SSI NO_SSL_DL BUFSIZ=4096)
