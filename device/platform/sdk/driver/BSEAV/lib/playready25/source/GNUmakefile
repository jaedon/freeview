# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************
#include ../../../../platform.mak
CROSS_COMPILE_PREFIX=$(CONFIG_TOOLCHAIN_PATH)/bin/mipsel-linux-

PLAYREADY_ROOT = $(shell pwd)/..
PLAYREADY_DIR=$(PLAYREADY_ROOT)/source/linux
INSTALL_DIR=$(PLAYREADY_ROOT)/source/playready
LINUX_BUILD=1
_NTROOT=$(PLAYREADY_ROOT)
PRDY_ROBUSTNESS_ENABLE = y
PRDY_NETFLIX_SECURESTOP_ENABLE = y
PRDY_HWDECRYPT_ENABLE = y
export PLAYREADY_ROOT PLAYREADY_DIR PLAYREADY_PROFILE LINUX_BUILD _NTROOT CROSS_COMPILE_PREFIX PRDY_ROBUSTNESS_ENABLE PRDY_NETFLIX_SECURESTOP_ENABLE PRDY_HWDECRYPT_ENABLE

BINDIR = $(PLAYREADY_ROOT)/output/bin/playready
ifeq ($(PRDY_ROBUSTNESS_ENABLE), y)
LIBDIR = $(PLAYREADY_ROOT)/output/lib/playready_robust
else
LIBDIR = $(PLAYREADY_ROOT)/output/lib/playready
endif
export BINDIR LIBDIR PRDY_ROBUSTNESS_ENABLE

SRC_DIR = .
TOP_SRC_DIR = .

ifneq "" "$(LINUX_BUILD)"
SHELL = /bin/bash

include $(PLAYREADY_DIR)/objs.inc

TARGETS =               \
    blackbox            \
    certs               \
    cmdlnpars           \
    constants           \
    core                \
    drmmanager          \
    initiators          \
    keyfile             \
    keyfileformat       \
    linux               \
    modules             \
    ndt                 \
    ndtasf              \
    ndtmgr              \
    oem                 \
    results             \
    tools               \
    wmdrmcert           \
    xmr                 \
    test                \
    tee			\

TARGETNAME  = libprdy
MODULE_NAME = libprdy
else
include $(BASEDIR)/pathvars.common

TARGETS =               \
    profile             \
    results             \
    certs               \
    crypto              \
    constants           \
    core                \
    modules             \
    initiators          \
    blackbox            \
    oem                 \
    drmmanager          \
    cmdlnpars           \
    wmdrmcert           \
    domain              \
    domainstore         \
    xmr                 \
    keyfile             \
    keyfileformat       \
    tools               \
    tee			\

TARGETNAME  = libPortingKit
MODULE_NAME = libPortingKit

BINFOLDER   = PK
endif

TARGET_OBJS =                         \
    $(WMDRM_MD_ALL_OBJS)              \

#TARGETTYPE  = SHARED_LIBRARY
TARGETTYPE  = STATIC_LIBRARY

include $(PLAYREADY_DIR)/GNUmakefile.common
