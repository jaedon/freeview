############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/22 $
# $brcm_Date: 10/17/12 4:44p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/transport/Makefile $
# 
# Hydra_Software_Devel/22   10/17/12 4:44p mward
# SW7435-56:  Add message_dss.
# 
# Hydra_Software_Devel/21   7/30/12 10:55a erickson
# SW7358-266: add unittest for playpump/playback open pid channel options
# 
# Hydra_Software_Devel/20   12/16/11 11:44a erickson
# SW7405-5325: add packetsub1 test
# 
# Hydra_Software_Devel/19   7/29/11 10:33a erickson
# SW7425-1029: add timebase unittest
# 
# Hydra_Software_Devel/18   3/30/10 6:24p mward
# SW7125-195: Build additional tests by default.
# 
# Hydra_Software_Devel/17   10/8/09 1:41p erickson
# SWDEPRECATED-3717: switch non-7401/7403 to message_stress testse
#
# Hydra_Software_Devel/16   6/2/09 11:44a erickson
# PR55611: add test app
#
# Hydra_Software_Devel/15   4/9/09 1:39p erickson
# PR54038: added test for bad NEXUS_Message_ReadComplete calls. system
# should survive.
#
# Hydra_Software_Devel/14   3/18/09 4:55p erickson
# PR53390: added filter group stress test
#
# Hydra_Software_Devel/13   11/20/08 5:00p erickson
# PR48848: added message_group test
#
# Hydra_Software_Devel/12   10/27/08 4:14p erickson
# PR42679: added pacing testse
#
# Hydra_Software_Devel/11   10/23/08 3:58p erickson
# PR42679: update
#
# Hydra_Software_Devel/10   7/21/08 1:26p erickson
# PR45008: add pid_splicing test
#
# Hydra_Software_Devel/9   5/14/08 12:29p erickson
# PR42679: make test cruisecontrol friendly
#
# Hydra_Software_Devel/8   5/14/08 10:07a erickson
# PR42679: eliminate duplicate code in unittest Makefiles
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
	change_timebase \
	message \
	message_alloc \
	message_dss \
	message_pes \
	open \
	pacing_playpump \
	pacing_recpump \
	packetsub1 \
	parser_band \
	pcr_test \
	pid_splicing \
	playpump_openpidchannel \
	recpump \
	playpump_cc_test \
	set_stc \
	timebase \
	tpit

NEXUS_PLATFORM ?= $(PLATFORM)
ifneq ($(findstring $(NEXUS_PLATFORM),97401 97403),)
APPS += \
	message_group \
	message_group2 \
	message_group3
else
APPS += \
	message_stress1 \
	message_stress2 \
	message_stress3 \
	message_stress4
endif

# scrambling_status is 3563 only

include ../common/Makefile.inc
