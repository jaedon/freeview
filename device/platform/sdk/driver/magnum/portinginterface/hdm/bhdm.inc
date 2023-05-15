############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bhdm.inc $
# $brcm_Revision: Hydra_Software_Devel/13 $
# $brcm_Date: 9/24/12 2:51p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/hdm/bhdm.inc $
# 
# Hydra_Software_Devel/13   9/24/12 2:51p vle
# SW7445-23: Fix mistake for 7429 platforms
# 
# Hydra_Software_Devel/12   9/20/12 3:26p vle
# SW7445-23: Add HDM PI for 7445
# 
# Hydra_Software_Devel/11   7/12/12 4:07p rgreen
# SW7425-2989: Merge changes
# 
# Hydra_Software_Devel/SW7425-2989/1   7/6/12 2:16p rgreen
# SW7425-2989: Add debug monitor to report format changes to HDMI Tx core
# 
# Hydra_Software_Devel/10   6/18/12 3:41p vle
# SW7425-2951: Update HDM PI directory structure
# 
# Hydra_Software_Devel/9   3/9/12 2:41p rgreen
# SW7425-2515: Remove support for bhdm_cec files.  See cec pi
# 
# Hydra_Software_Devel/8   12/12/11 3:05p rgreen
# SW7125-1165: Merge changes
# 
# Hydra_Software_Devel/SW7125-1165/1   12/8/11 1:41p mward
# SW7125-1165: Replace findstring with filter to do exact word match.
# 
# Hydra_Software_Devel/7   6/17/11 1:43p vle
# SW7405-5358, SW7405-5312: Separate 3D support from EDID parser to allow
# easier back port
# 
# Hydra_Software_Devel/6   2/23/10 1:12a vle
# SW7420-579: Refactor HDMI PI code
# 
# Hydra_Software_Devel/5   5/6/08 10:19a rgreen
# PR38947,PR39463: fix comment for enabling LVDS/DVO support; should be
# enabled for all RXs
# 
# Hydra_Software_Devel/4   4/28/08 6:02p vle
# PR42271: Move HDMI CEC implementations to bhdm_cec_priv to allow ease
# of code sharing between HDMI transmitter and receiver platforms.
# 
# Hydra_Software_Devel/3   3/5/08 10:46a rgreen
# PR38946,PR39503: Add HDM Support for 3548 and 3556
# 
# Hydra_Software_Devel/2   6/7/07 12:21p vle
# PR 31917: Fix condition on checking CEC support.
# 
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# BHDM_CEC_SUPPORT - If defined, this will include CEC support.
#

# Define this module
MAGNUM_MODULES += BHDM

# Module Includes
BHDM_INCLUDES += $(MAGNUM)/portinginterface/hdm/include \
		 $(MAGNUM)/portinginterface/hdm/src/common
		 
# Module Sources
BHDM_SOURCES += $(MAGNUM)/portinginterface/hdm/src/common/bhdm.c \
		$(MAGNUM)/portinginterface/hdm/src/common/bhdm_edid.c \
		$(MAGNUM)/portinginterface/hdm/src/common/bhdm_hdcp.c \
		$(MAGNUM)/portinginterface/hdm/src/common/bhdm_packet.c \
		$(MAGNUM)/portinginterface/hdm/src/common/bhdm_debug.c 

#3D Support
ifndef (BHDM_3D_SUPPORT)
BHDM_3D_SUPPORT=y
endif

ifeq ($(BHDM_3D_SUPPORT), y)
BHDM_SOURCES +=	$(MAGNUM)/portinginterface/hdm/src/common/bhdm_edid_3d.c
BHDM_DEFINES += BHDM_3D_SUPPORT	
endif


#################
#Chip specific
#################

### HDMI 1.3 feature #####
ifneq ($(filter $(BCHP_CHIP), 7325 7335 7400 7405), $(BCHP_CHIP))
BHDM_DEFINES += BHDM_HAS_HDMI_1_3_FEATURES
endif	


###### platform specific source #######

### 65nm platforms with HDMI FALCON core
ifeq ($(filter $(BCHP_CHIP), 7325 7335 7400 7405), $(BCHP_CHIP))

BHDM_SOURCES +=	$(MAGNUM)/portinginterface/hdm/src/core65nm/bhdm_priv_falcon.c

# DVO Support
ifeq ($(BHDM_DVO_ENABLE_SUPPORT),y)
BHDM_SOURCES += $(MAGNUM)/portinginterface/hdm/src/core65nm/bhdm_dvo_falcon.c
BHDM_DEFINES += BHDM_ENABLE_DVO
endif

else
	### 65nm platforms with HDMI DVP_HT core 
	ifeq ($(filter $(BCHP_CHIP), 7125 7340 7342 7408 7420 7468 7550), $(BCHP_CHIP))

	BHDM_SOURCES +=	$(MAGNUM)/portinginterface/hdm/src/core65nm/bhdm_priv_dvpht.c
	
	# DVO Support
	ifeq ($(BHDM_DVO_ENABLE_SUPPORT),y)
	BHDM_SOURCES += $(MAGNUM)/portinginterface/hdm/src/core65nm/bhdm_dvo_dvpht.c
	BHDM_DEFINES += BHDM_ENABLE_DVO
	endif
	
	else
		###40nm platforms
		ifeq ($(filter $(BCHP_CHIP), 7422 7425 7429 7435 7231 7358 7344 7346 7360 7552 7640 7584 7563), $(BCHP_CHIP))
		BHDM_SOURCES +=	$(MAGNUM)/portinginterface/hdm/src/core40nm/bhdm_priv.c
		else

		###28nm platforms
		BHDM_SOURCES +=	$(MAGNUM)/portinginterface/hdm/src/core28nm/bhdm_priv.c
		endif
	endif
endif

