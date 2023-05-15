############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: basf.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 2/12/10 5:37p $
#
# Module Description:
#   ASF media library module descriptor
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/asf/basf.inc $
# 
# 2   2/12/10 5:37p vsilyaev
# SW7468-112: Fixed compile problem when ASF directory is not present
# 
# 1   7/20/07 12:44p vsilyaev
# PR 32813: Module description file
# 
# 
############################################################

BASF_INCLUDES := $(BSEAV)/lib/asf
ifeq ($(HUMAX_PLATFORM_BASE),y)
ifeq ($(CONFIG_PLAYREADY_25),y)
BASF_INCLUDES += $(BSEAV)/lib/playready
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/inc
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/results
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/certs
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/xmr
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/ndtinc
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/keyfile
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/keyfileformat
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/tools/common
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/oem/common/inc
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/oem/ansi/inc
BASF_INCLUDES += $(BSEAV)/lib/playready25/source/oem/ansi/trace
else
BASF_INCLUDES += $(BSEAV)/lib/playready
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/inc
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/results
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/certs
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/xmr
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/ndtinc
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/keyfile
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/keyfileformat
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/tools/common
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/oem/common/inc
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/oem/ansi/inc
BASF_INCLUDES += $(BSEAV)/lib/playready20/source/oem/ansi/trace
endif
endif

BASF_DEFINES := B_HAS_ASF=1
ifeq ($(HUMAX_PLATFORM_BASE),y)
BASF_DEFINES += DRM_BUILD_PROFILE=10 TARGET_LITTLE_ENDIAN=1
endif

BASF_SOURCES :=  \
	$(BSEAV)/lib/asf/basf_util.c  \
	$(BSEAV)/lib/asf/basf_parser.c 

BASF_FILTER_SOURCES := \
	$(BSEAV)/lib/asf/basf_stream.c 

BASF_PLAYER_SOURCES := \
	$(BSEAV)/lib/asf/basf_player.c

BASF_PROBE_SOURCES := \
	$(BSEAV)/lib/asf/basf_probe.c

BASF_PARS_SOURCES := \
	$(BSEAV)/lib/asf/asfpars.c

