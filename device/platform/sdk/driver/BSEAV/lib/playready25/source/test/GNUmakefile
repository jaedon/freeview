# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************

SRC_DIR = .
TOP_SRC_DIR = .

ifneq "" "$(LINUX_BUILD)"
SHELL = /bin/bash

PLAYREADY_TEST=1
include $(PLAYREADY_DIR)/objs.inc

TARGETNAME  = libplayreadypk_test
MODULE_NAME = libplayreadypk_test

TARGET_OBJS =                         \
    $(WMDRM_MD_ALL_OBJS)              \

TARGETTYPE  = STATIC_LIBRARY

include $(PLAYREADY_DIR)/GNUmakefile.common

endif
