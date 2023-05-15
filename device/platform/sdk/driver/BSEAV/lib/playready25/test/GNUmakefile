# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************


# This is the top-level GNU makefile for the PlayReady Porting Kit TEST tree.
#
# This file is NOT generated

include ../../../../platform.mak
PLAYREADY_ROOT=$(shell pwd)/..
PLAYREADY_DIR=$(PLAYREADY_ROOT)/source/linux
LINUX_BUILD=1
_NTROOT=$(PLAYREADY_ROOT)
export PLAYREADY_ROOT PLAYREADY_DIR PLAYREADY_PROFILE LINUX_BUILD _NTROOT CROSS_COMPILE_PREFIX

ifneq "" "$(LINUX_BUILD)"

# Testing - core functionality
TARGETS =    \
    testcore \
    tOEMImp  \
    tstutils \
    NetIO    \

# Test areas
ifneq "$(DRM_BUILD_PROFILE)" "950"
TARGETS +=          \
    blackbox        \
    certs           \
    cmdlnpars       \
    ConsoleNDR      \
    ConsoleNDT      \
    CoreEmbedding   \
    Crypto          \
    Datastore       \
    devicedevcert   \
    devicestore     \
    domainapi       \
    domainstore     \
    DrmManager      \
    HDS             \
    Header          \
    initparser      \
    KeyFile         \
    licacqv3        \
    LicEval         \
    licgen          \
    LicStore        \
    LiveTV          \
    m2tsencryptor   \
    mathsafe        \
    meta            \
    metercertstore  \
    Metering        \
    noncestore      \
    OEM             \
    performance     \
    PolicyState     \
    prnd            \
    SecStore        \
    SecureClock     \
    SyncList        \
    Test_Suite_NDT  \
    utf             \
    Utils           \
    XMLBuilder      \
    XMLBuilderA     \
    XMLParser       \
    xmlsig          \
    xmr             \

else    
# DRM_BUILD_PROFILE == 950
TARGETS +=          \
    blackbox        \
    certs           \
    Crypto          \
    DrmManager      \
    HDS             \
    Header          \
    KeyFile         \
    licgen          \
    LiveTV          \
    m2tsencryptor   \
    mathsafe        \
    Metering        \
    OEM             \
    performance     \
    prnd            \
    SecureClock     \
    xmr             \

endif
    
else
# Mac

TARGETS =    \
    tstutils \

endif


include $(PLAYREADY_DIR)/GNUmakefile.common
