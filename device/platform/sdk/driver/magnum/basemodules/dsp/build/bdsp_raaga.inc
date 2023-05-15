############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bdsp_raaga.inc $
# $brcm_Revision: Hydra_Software_Devel/10 $
# $brcm_Date: 8/9/12 8:15p $
#
# Module Description:
#
# Revision History:
#
# Created: 12/15/2010 by Jon Garrett
#
# $brcm_Log: /magnum/basemodules/dsp/build/bdsp_raaga.inc $
# 
# Hydra_Software_Devel/10   8/9/12 8:15p gprasad
# SW7425-2952: Fixing build error while building NDS app
# 
# Hydra_Software_Devel/9   6/19/12 3:14a gprasad
# SW7425-2952: DSP directory structure flattening
# 
# Hydra_Software_Devel/8   5/27/11 4:45p gprasad
# SW7422-389: [7422] Add Dolby MS11 support
# 
# Hydra_Software_Devel/7   4/6/11 2:34p srajapur
# SW7425-291: [7425]BDSP directory structure changes
# 
# Hydra_Software_Devel/6   3/24/11 11:48a jgarrett
# SW7422-146: Fixing kernel mode builds
# 
# Hydra_Software_Devel/5   1/26/11 3:17a gautamk
# SW7422-191: Allocating FW debug buffer on basis of runtime Parrameter
# enableDebugLog of BDSP_RaagaSettings.
# 
# Hydra_Software_Devel/4   1/25/11 2:22a gautamk
# SW7422-191:Adding error recovery code for Raaga Open
# 
# Hydra_Software_Devel/3   1/13/11 5:27a gautamk
# SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
# includes - DSP Booting - Open time Download :  - Start Time Audio
# Decode algorithm download  - Message buffer interaction with DSP:  -
# Synchronized Interrupt:  - Async interrupts:  - Config Commands for
# Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
# Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
# level interrupt
# 
# Hydra_Software_Devel/2   12/15/10 6:47p jgarrett
# SW7422-146: Initial compileable prototype
# 
# Hydra_Software_Devel/1   12/15/10 1:59p jgarrett
# SW7422-146: Adding initial BDSP files
# 
############################################################

#
# Conditional Options for this Makefile
#
# BDSP_AUDIO_SUPPORT (default=y) - Include audio support
# BDSP_VIDEO_SUPPORT (default=n) - Include video support
# BDSP_IMG_ONLY (default=n) - Include firmware only
# BDSP_IMG_EXTERNAL (default=n) - Include non-firmware files only
#

# Include standard files
BDSP_INCLUDES += $(MAGNUM)/basemodules/dsp/include/raaga

# Include standard files
BDSP_INCLUDES += $(MAGNUM)/basemodules/dsp/src/raaga/firmware/$(BDSP_RAAGA_FP_VER)

# Include standard files
ifneq ($(BDSP_IMG_ONLY),y)
BDSP_SOURCES += \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga.c \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga_fwdownload_priv.c \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga_mm_priv.c \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga_fwinterface_priv.c \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga_int_priv.c \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga_util.c \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga_priv.c
endif

# Include standard files
ifneq ($(BDSP_IMG_ONLY),y)
BDSP_SOURCES += \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga_fw_priv.c \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga_cit_priv.c \
        $(MAGNUM)/basemodules/dsp/src/raaga/bdsp_raaga_fw_settings.c
endif

# Set CFLAG for Raaga Support
BDSP_DEFINES += BDSP_RAAGA_SUPPORT=1

# Include firmware files
include $(MAGNUM)/basemodules/dsp/build/bdsp_raaga_img.inc

