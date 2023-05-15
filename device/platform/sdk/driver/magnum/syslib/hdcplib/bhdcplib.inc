############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bhdcplib.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 9/6/12 5:50p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/syslib/hdcplib/bhdcplib.inc $
# 
# Hydra_Software_Devel/3   9/6/12 5:50p vle
# SW7425-3763: Remove symlinks from Magnum HDCPLIB syslib modules
# 
# Hydra_Software_Devel/SW7425-3763/1   8/31/12 3:09p vle
# SW7425-3763: Remove symlinks from magnum HDCPLIB syslib
# 
# Hydra_Software_Devel/2   3/18/08 7:22p vle
# PR 39991: include bhdcplib_hdcpkeys.c
# 
# Hydra_Software_Devel/1   2/26/08 5:43p jgarrett
# PR 39991: Renaming new HDMlib to HDCPlib
# 
# Hydra_Software_Devel/Syslib_Initiative/3   1/8/08 6:05p vle
# PR 31924: HDMI Syslib needs to be informed of HDMI events (Ri, Pj and
# Hotplug)
# 
# Hydra_Software_Devel/Syslib_Initiative/2   1/4/08 6:50p vle
# PR 31924: HDMI Tx Syslib Support
# 
# Hydra_Software_Devel/Syslib_Initiative/1   9/14/07 5:56p vle
# PR 32936: Add new syslib
# 
# Hydra_Software_Devel/1   1/11/07 2:41p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# BHDCPLIB_HDCP_SUPPORT - If defined, HDCP support will be enabled.
#

# Define this module
MAGNUM_MODULES += BHDCPLIB

# Module Includes
BHDCPLIB_INCLUDES +=	$(MAGNUM)/syslib/hdcplib/include \
			$(MAGNUM)/syslib/hdcplib/src/common

# Module Sources
BHDCPLIB_SOURCES += $(MAGNUM)/syslib/hdcplib/src/common/bhdcplib.c
BHDCPLIB_SOURCES += $(MAGNUM)/syslib/hdcplib/src/common/bhdcplib_hdcpkeys.c

# HDCP Option
ifneq ($(BHDCPLIB_HDCP_SUPPORT),)

	############## 65nm platforms #################
	ifeq ($(filter $(BCHP_CHIP), 7125 7208 7325 7335 7336 7340 7342 7400 7405 7408 7420 7468 7550 7563 7572 7580), $(BCHP_CHIP))

		######## non-BSP platforms ###########
		ifeq ($(filter $(BCHP_CHIP), 7550 7563 7572 7580 7408), $(BCHP_CHIP))
		BHDCPLIB_SOURCES += $(MAGNUM)/syslib/hdcplib/src/core65nm/nonbsp/bhdcplib_keyloader.c
		endif

		######## non-ASKM platforms ###########
		ifeq ($(filter $(BCHP_CHIP), 7325 7335 7336 7400 7405), $(BCHP_CHIP))
		BHDCPLIB_SOURCES += $(MAGNUM)/syslib/hdcplib/src/core65nm/nonaskm/bhdcplib_keyloader.c
		endif
		
		######## ASKM platforms ###########
		ifeq ($(filter $(BCHP_CHIP), 7125 7208 7340 7342 7420 7468 ), $(BCHP_CHIP))
		BHDCPLIB_SOURCES += $(MAGNUM)/syslib/hdcplib/src/core65nm/askm/bhdcplib_keyloader.c
		endif

	else

	############# 40nm platforms ################
	BHDCPLIB_SOURCES += $(MAGNUM)/syslib/hdcplib/src/core40nm/bhdcplib_keyloader.c
	endif
	
else
BHDCPLIB_SOURCES += $(MAGNUM)/syslib/hdcplib/src/common/bhdcplib_keyloader_stub.c
endif

