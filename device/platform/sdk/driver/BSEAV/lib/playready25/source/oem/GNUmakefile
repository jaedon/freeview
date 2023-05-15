# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************

ifneq "" "$(LINUX_BUILD)"
ifeq "950" "$(DRM_BUILD_PROFILE)"
TARGETS =   \
    prndtx1 \
    common
else
TARGETS =   \
    linux   \
    common
endif
else
TARGETS =   \
    ansi    \
    common
endif

include $(PLAYREADY_DIR)/GNUmakefile.common

