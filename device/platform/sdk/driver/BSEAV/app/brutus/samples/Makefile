############################################################
#	  Copyright (c) 2003-2012, Broadcom Corporation
#	  All Rights Reserved
#	  Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 1 $
# $brcm_Date: 9/7/12 10:45a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/app/brutus/samples/Makefile $
# 
# 1   9/7/12 10:45a mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/6/12 5:28p mphillip
# SW7425-3739: Remove symlinks
# 
#
#
############################################################

include ../../../api/build/tools.mak

# This preserves the symlink spaghetti which existed before.  New platforms should use default.channels.txt
# (which will occur by default), unless there's a specific reason to fork it.

SAMPLE_PLATFORM ?= default

ifeq ($(findstring $(NEXUS_PLATFORM), 97019 97125),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97125
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97458 97455),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97455
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97459 97456),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97456
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97468 97208),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97468
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97336 97325 97335 97340 97342),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97336
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97346 97344 97418),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97346
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97319 97328),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97328
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97358),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97358
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97360),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97360
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97408),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97408
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97409),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97409
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97550),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97550
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97552),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97552
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97584),$(NEXUS_PLATFORM))
SAMPLE_PLATFORM=97584
endif

# Same for (platform).ip.channels.txt.  Note that the old symlinks copied channels.txt for some 
# platforms, which is preserved below.
SAMPLE_IP_PLATFORM ?= default
SAMPLE_IP_CHANNELS ?= ip.channels

ifeq ($(findstring $(NEXUS_PLATFORM), 97336 97346 97325 97335 97344 97418),$(NEXUS_PLATFORM))
SAMPLE_IP_CHANNELS=channels
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97336 97325 97335),$(NEXUS_PLATFORM))
SAMPLE_IP_PLATFORM=97336
endif

ifeq ($(findstring $(NEXUS_PLATFORM), 97346 97344 97418),$(NEXUS_PLATFORM))
SAMPLE_IP_PLATFORM=97346
endif

install-sample-files:
	${Q_}$(CP) ${SAMPLE_PLATFORM}.channels.txt ${INSTALL_DIR}/channels.txt

install-sample-ip-files:
	${Q_}$(CP) ${SAMPLE_IP_PLATFORM}.${SAMPLE_IP_CHANNELS}.txt ${INSTALL_DIR}/ip.channels.txt

