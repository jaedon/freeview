#############################################################################
#
#       Copyright (c) 2003-2011, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: video_shmoo.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 3/9/11 12:07p $
#
# File Description: Builds video shmoo for the Bcm97422 linux platform
#
# Revision History:
#
# $brcm_Log: /rockford/applications/video_shmoo_test/video_shmoo.inc $
# 
# Hydra_Software_Devel/1   3/9/11 12:07p tdo
# SW7422-328: Add video shmoo to appframework
#
#############################################################################

COMMON_INCLUDES += $(ROCKFORD_DIR)/applications/video_shmoo_test/
COMMON_SOURCES  += \
	$(ROCKFORD_DIR)/applications/video_shmoo_test/btst_video_shmoo.c

