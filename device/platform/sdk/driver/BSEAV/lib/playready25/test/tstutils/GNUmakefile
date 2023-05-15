# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************

ifneq "" "$(LINUX_BUILD)"

TARGETS =          \
    contentkey     \
    generatorutils \
    getclientid    \
    headergen      \
    licgen         \
    utils          \
    xmrlicgen      \

else
# Mac

TARGETS =          \
    headergen      \
    xmrlicgen      \
    generatorutils \

endif

include $(PLAYREADY_DIR)/GNUmakefile.common

