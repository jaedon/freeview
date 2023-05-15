############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bhdr.inc $
# $brcm_Revision: Hydra_Software_Devel/40 $
# $brcm_Date: 10/10/12 9:26a $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: Q:/projects/7425/latest/magnum/portinginterface/hdr/bhdr.inc $
# 
# Hydra_Software_Devel/40   10/10/12 9:26a rgreen
# SW7445-24: Remove duplicate checkin log
# 
# Hydra_Software_Devel/39   10/4/12 12:59p rgreen
# SW7445-24: Move bhdr_hdcp_priv.c to common directory
# 
# Hydra_Software_Devel/38   9/24/12 6:47p rgreen
# SW7425-1988: For clarification, rename bhdr_repeater to
# bhdr_hdcp_repeater.c
# 
# Hydra_Software_Devel/37   9/24/12 5:53p rgreen
# SW7445-24:  Add 28nm support
# 
# Hydra_Software_Devel/36   9/6/12 5:43p rgreen
# SW7425-3771: Merge changes
# 
# Hydra_Software_Devel/SW7425-3771/1   9/6/12 11:38a rgreen
# SW7425-3771: Flatten HDR directory
# 
# Hydra_Software_Devel/35   5/18/12 3:44p gmohile
# SW7429-155 : Add 7429 Power Management Support
# 
# Hydra_Software_Devel/34   4/19/12 2:13p gmohile
# SW7435-61 : 7435 power management support
# 
# Hydra_Software_Devel/33   3/28/12 1:55p rgreen
# SW7425-2515: Remove support for CEC.  CEC now has separate API
# 
# Hydra_Software_Devel/32   2/6/12 4:52p katrep
# SW7429-1:7429 link issue
# 
# Hydra_Software_Devel/31   1/4/12 11:21a rgreen
# SW7435-20: Add 7535 support
# 
# Hydra_Software_Devel/30   11/29/11 3:50p rgreen
# SW7429-21: Add 7429 Support
#
# Hydra_Software_Devel/SW7435-20/1   11/22/11 12:45p mward
# SW7435-20:  Add 7435, with no pwr support for now.
# 
# Hydra_Software_Devel/29   8/2/11 6:15p rgreen
# SW7425-692:  Update power management support for standby/resume
# 
# Hydra_Software_Devel/28   6/20/11 1:31p rbshah
# SWDTV-6867: 35233: Ensure bhdr_pwr.c is not compiled for 35233 at this
# time.
# 
# Hydra_Software_Devel/27   6/8/11 5:10p rgreen
# SW7425-692: Initial power management support
# 
# Hydra_Software_Devel/26   5/10/11 10:51a rgreen
# SWDTV-4664,SW3556-1233,SW3556-1236: Update HDMI Rx to build with new
# HDCP Key Verification support.
# 
# Hydra_Software_Devel/25   4/18/11 12:11p rgreen
# SW7422-186:  Merge HDMI HDCP Repeater Support
# 
# Hydra_Software_Devel/24   4/15/11 11:31a rbshah
# SWDTV-6184: 35233: Get HDCP to compile and link (temporarily).
# 
# Hydra_Software_Devel/23   1/25/11 4:59p rgreen
# SWBLURAY-23819: Add support for 7640
# 
# Hydra_Software_Devel/22   12/30/10 10:04p rgreen
# SW7422-110: Always add EDID RFAM support for 7422/7425
# 
# Hydra_Software_Devel/21   12/23/10 9:19p hongtaoz
# SW35230-2374: add hdcp support to 7425;
# 
# Hydra_Software_Devel/20   12/23/10 3:03p rgreen
# SW35230-2374:Add HDCP Key support for 7422
# 
# Hydra_Software_Devel/19   12/8/10 12:37p rgreen
# SW35230-2374,SW7422-129: Add support for HDCP Key loading and
# verfication
# 
# Hydra_Software_Devel/18   12/7/10 7:55p rgreen
# SW7422-129:Add support for 7422
# 
# Hydra_Software_Devel/17   12/6/10 5:37p jhaberf
# SW35125-1: Added HDCP verifcation for the 35125
# 
# Hydra_Software_Devel/16   12/6/10 11:31a rgreen
# SW35230-2374: Add support for OTP HDCP Key Set Check
# 
# Hydra_Software_Devel/15   6/7/10 5:17p rgreen
# SW35230-128: Separate phy specific code to separate file
# 
# Hydra_Software_Devel/14   1/8/10 4:16p rgreen
# SW3548-2642: Move packet processing to separate bhdr_packet_priv.c/h
# file
# 
# Hydra_Software_Devel/13   11/24/09 5:55p rgreen
# SW35230-15: CEC support for 3548/56 only
# 
# Hydra_Software_Devel/12   4/8/09 3:28p rgreen
# PR54000: Add debug support; set files specific to 3549/56 compilation
# 
# Hydra_Software_Devel/11   3/23/09 1:20p rgreen
# PR52879: Additional support for EDID RAM
# 
# Hydra_Software_Devel/10   3/6/09 10:32a rgreen
# PR52879: Add compilation/debug support for on-chip EDID RAM
# 
# Hydra_Software_Devel/9   2/26/09 7:57p rgreen
# PR51307: Add compile time option (BHDR_HDCP_REPEATER_SUPPORT) for HDCP
# Repeater
# 
# Hydra_Software_Devel/8   2/3/09 5:07p rgreen
# PR51331: add  macro for conditional compile of power management
# 
# Hydra_Software_Devel/7   10/31/08 1:52p rgreen
# PR48570: Use KYLIN macro to turn on debug Eye Diagra
# 
# Hydra_Software_Devel/6   9/11/08 6:25p rgreen
# PR44527:Remove BHDR_CEC macro;always build
# 
# Hydra_Software_Devel/5   9/4/08 5:32p rgreen
# PR44257: PR44257:Add CEC support
# PR44257: Add CEC support for 3549/3556
# 
# Hydra_Software_Devel/PR44257/2   9/4/08 5:31p rgreen
# PR44257:Add CEC support
# 
# Hydra_Software_Devel/PR44257/1   7/21/08 6:20p vle
# PR44257: Add CEC support for 3549/3556
# 
# Hydra_Software_Devel/4   6/23/08 6:11p rgreen
# PR39463: Remove references to unused bhdr_hdcp.c/h files
# 
# Hydra_Software_Devel/3   4/27/08 10:23p rgreen
# PR38948,PR39463: Do not compile bhdr_fe into 3563
# 
# Hydra_Software_Devel/2   4/22/08 11:26p rgreen
# PR38947,PR39463: Add bhdr_fe.c to build
# 
# Hydra_Software_Devel/1   1/11/07 2:41p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BHDR

# Module Defines 
ifeq ($(KYLIN),y)
BHDR_DEFINES += KYLIN=1
endif

# Module Includes
BHDR_INCLUDES = \
	$(MAGNUM)/portinginterface/hdr/include \
	$(MAGNUM)/portinginterface/hdr/src/common


# Module Common Sources
BHDR_SOURCES = \
	$(MAGNUM)/portinginterface/hdr/src/common/bhdr.c \
	$(MAGNUM)/portinginterface/hdr/src/common/bhdr_hdcp.c \
	$(MAGNUM)/portinginterface/hdr/src/common/bhdr_packet_priv.c \
	$(MAGNUM)/portinginterface/hdr/src/common/bhdr_debug.c


ifeq ($(filter $(BCHP_CHIP), 7422 7425 7429 7435 7640 7445), $(BCHP_CHIP))
BHDR_EDID_RAM_SUPPORT=y
BHDR_HDCP_REPEATER_SUPPORT=y
endif


ifeq ($(BHDR_EDID_RAM_SUPPORT),y)
BHDR_SOURCES += $(MAGNUM)/portinginterface/hdr/src/common/bhdr_edid_ram.c
BHDR_DEFINES += BHDR_CONFIG_EDID_RAM=1
endif


ifeq ($(BHDR_HDCP_REPEATER_SUPPORT),y)
BHDR_DEFINES += BHDR_CONFIG_HDCP_REPEATER=1
BHDR_SOURCES += \
	$(MAGNUM)/portinginterface/hdr/src/common/bhdr_hdcp_repeater.c
else
BHDR_DEFINES += BHDR_CONFIG_HDCP_REPEATER=0
endif


#################
#Chip specific Sources
#################




ifeq ($(filter $(BCHP_CHIP), 7422 7425 7429 7435), $(BCHP_CHIP))
ifeq ($(BCHP_PWR_SUPPORT),y)
BHDR_SOURCES += \
	$(MAGNUM)/portinginterface/hdr/src/core40nm/bhdr_pwr.c
endif
endif

####### 28nm ######  
ifeq ($(filter $(BCHP_CHIP), 7445), $(BCHP_CHIP))
BHDR_INCLUDES += \
	$(MAGNUM)/portinginterface/hdr/src/core28nm

BHDR_SOURCES += \
	$(MAGNUM)/portinginterface/hdr/src/core28nm/bhdr_fe.c \
	$(MAGNUM)/portinginterface/hdr/src/core28nm/bhdr_fe_priv.c \
	$(MAGNUM)/portinginterface/hdr/src/common/bhdr_hdcp_priv.c \
	$(MAGNUM)/portinginterface/hdr/src/common/bhdr_phy_priv.c
	
#BHDR_SOURCES += $(MAGNUM)/portinginterface/hdr/src/core40nm/bhdr_edid_ram.c
endif


###### 40nm  ######
ifeq ($(filter $(BCHP_CHIP), 35230 35233 35125 7422 7425 7435 7429), $(BCHP_CHIP))
BHDR_INCLUDES += \
	$(MAGNUM)/portinginterface/hdr/src/core40nm

BHDR_SOURCES += \
	$(MAGNUM)/portinginterface/hdr/src/core40nm/bhdr_fe.c \
	$(MAGNUM)/portinginterface/hdr/src/core40nm/bhdr_fe_priv.c \
	$(MAGNUM)/portinginterface/hdr/src/common/bhdr_hdcp_priv.c \
	$(MAGNUM)/portinginterface/hdr/src/common/bhdr_phy_priv.c
endif


###### 65nm ######
ifeq ($(filter $(BCHP_CHIP), 3548 3556), $(BCHP_CHIP))
BHDR_SOURCES += \
	$(MAGNUM)/portinginterface/hdr/src/core65nm/bhdr_hdcp_priv.c
endif




