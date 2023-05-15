#############################################################################
#
#       Copyright (c) 2003-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bvce.inc $
# $brcm_Revision: Hydra_Software_Devel/18 $
# $brcm_Date: 10/11/12 12:08p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/vce/bvce.inc $
# 
# Hydra_Software_Devel/18   10/11/12 12:08p nilesh
# SW7425-2568: Added bvce_debug.c
# 
# Hydra_Software_Devel/17   10/1/12 2:03p nilesh
# SW7445-39: Split power and output code into separate files
# 
# Hydra_Software_Devel/16   9/6/12 11:40p nilesh
# SW7425-3783: Removed symlinks
# 
# Hydra_Software_Devel/SW7425-3783/1   8/30/12 2:17p nilesh
# SW7425-3783: Removed symlinks
# 
# Hydra_Software_Devel/15   8/29/12 2:43p nilesh
# SW7425-3783: Fix check test failures
# 
# Hydra_Software_Devel/14   8/21/12 12:36p nilesh
# SW7425-3783: Updated directory structure
# 
# Hydra_Software_Devel/13   5/10/12 3:58p nilesh
# SW7425-2568: Enabled VCE userdata dumps when in BVCE_TEST_MODE
# 
# Hydra_Software_Devel/12   3/8/12 11:51a nilesh
# SW7425-2568: Added BVCE_TEST_MODE compile time option
# 
# Hydra_Software_Devel/11   12/20/11 2:48p nilesh
# SW7425-1882: Added BVCE_DUMP_USERDATA_LOG build flag
# 
# Hydra_Software_Devel/10   10/14/11 11:56a nilesh
# SW7425-1213: Added Telemetry Parser
# 
# Hydra_Software_Devel/9   9/26/11 3:46p nilesh
# SW7425-1213: Added BVCE_DUMP_ARC_DEBUG_LOG build flag to enable dump of
# raw debug log buffers to file
# 
# Hydra_Software_Devel/8   9/15/11 3:04p nilesh
# SW7425-890: Added compile time flag (BVCE_ENABLE_DUAL_ENCODE_RTS) to
# force dual encode RTS for VIP_1
# 
# Hydra_Software_Devel/7   4/19/11 10:52a nilesh
# SW7425-115: Added BVCE_ENABLE_OUTPUT=y build option
# 
# Hydra_Software_Devel/6   3/7/11 10:32p nilesh
# SW7425-1: Added BVCE_DUMP_OUTPUT_ITB_DESC dbg dump compile flag
# 
# Hydra_Software_Devel/5   3/7/11 2:18p nilesh
# SW7425-1: Added compile time debug flags BVCE_DUMP_OUTPUT_[ITB/CDB]
# 
# Hydra_Software_Devel/4   2/22/11 3:27p nilesh
# SW7425-1: Added bvce_fw_api.c
# 
# Hydra_Software_Devel/3   11/17/10 12:41p nilesh
# SW7425-1:
# Added file element "bvce_debug.h".
# Added file element "bvce_platform.c".
# Added file element "bvce_platform_7425.c".
# 
# Hydra_Software_Devel/2   10/7/10 11:10a nilesh
# SW7425-1: Added bvce.c, bvce_image.c, and bvce_platform_7425_image.c
# 
# Hydra_Software_Devel/1   9/20/10 4:42p nilesh
# SW7425-1:
# Added file element "bvce.h".
# SW7425-1: Merged vce to mainline
# 
# Hydra_Software_Devel/SW7425-1/1   7/14/10 4:29p nilesh
# SW7425-1: Created VCE PI
# 
#############################################################################

############################################################
#
# Conditional Options for this Makefile
#
# BVCE_IMG_ONLY - If defined, only the firmware image related code will be compiled.
# BVCE_IMG_EXTERNAL - If defined, firmware related code (interface AND context) will NOT be compiled. Also defines -DBVCE_USE_CUSTOM_IMAGE.  The app is expected to specify their own firmware interface AND image context when calling BVCE_Open().
# BVCE_USE_CUSTOM_CONTEXT - If defined, the firmware image context will NOT be compiled. The app is expected to provide their own firmware image context when calling BVCE_Open().
#

# Define this module
MAGNUM_MODULES += BVCE

#
# Select VCE CHIP family
#
BVCE_CHIP = $(BCHP_CHIP)

#### Module Includes ####
BVCE_INCLUDES += \
	$(MAGNUM)/portinginterface/vce/include \
	$(MAGNUM)/portinginterface/vce/src

#### Source File(s) ####
ifeq ($(BVCE_IMG_ONLY),)
#### Common File(s) ####
BVCE_SOURCES += \
             $(MAGNUM)/portinginterface/vce/src/bvce.c \
             $(MAGNUM)/portinginterface/vce/src/bvce_platform.c \
             $(MAGNUM)/portinginterface/vce/src/bvce_platform_$(BVCE_CHIP).c \
             $(MAGNUM)/portinginterface/vce/src/bvce_fw_api.c \
             $(MAGNUM)/portinginterface/vce/src/bvce_telem.c \
             $(MAGNUM)/portinginterface/vce/src/bvce_output.c \
             $(MAGNUM)/portinginterface/vce/src/bvce_power.c \
             $(MAGNUM)/portinginterface/vce/src/bvce_debug.c

#### Core File(s) ####

#### Platform File(s) ####
BVCE_SOURCES += 

endif # BVCE_IMG_ONLY

#### Firmware image file(s) ####
ifeq ($(BVCE_IMG_EXTERNAL),)
# Common image file(s)
BVCE_SOURCES +=	\
             $(MAGNUM)/portinginterface/vce/src/bvce_image.c

ifeq ($(BVCE_USE_CUSTOM_CONTEXT),)
# Platform image file(s)
BVCE_SOURCES += \
             $(MAGNUM)/portinginterface/vce/src/bvce_platform_$(BVCE_CHIP)_image.c
endif # !BVCE_USE_CUSTOM_CONTEXT

else # !BVCE_USE_CUSTOM_IMAGE
BVCE_DEFINES += BVCE_USE_CUSTOM_IMAGE
endif #!BVCE_IMG_EXTERNAL

# DEBUG Build Flags
ifeq ($(BVCE_DUMP_OUTPUT_CDB),y)
BVCE_DEFINES += BVCE_P_DUMP_OUTPUT_CDB=1
endif
ifeq ($(BVCE_DUMP_OUTPUT_ITB),y)
BVCE_DEFINES += BVCE_P_DUMP_OUTPUT_ITB=1
endif
ifeq ($(BVCE_DUMP_OUTPUT_ITB_DESC),y)
BVCE_DEFINES += BVCE_P_DUMP_OUTPUT_ITB_DESC=1
endif
ifeq ($(BVCE_ENABLE_UART),y)
BVCE_DEFINES += BVCE_P_ENABLE_UART=1
endif
ifeq ($(BVCE_DUMP_ARC_DEBUG_LOG),y)
BVCE_DEFINES += BVCE_P_DUMP_ARC_DEBUG_LOG=1
endif
ifeq ($(BVCE_DUMP_USERDATA_LOG),y)
BVCE_DEFINES += BVCE_P_DUMP_USERDATA_LOG=1
endif
ifeq ($(BVCE_TEST_MODE),y)
BVCE_DEFINES += BVCE_P_TEST_MODE=1
BVCE_DEFINES += BVCE_P_DUMP_USERDATA_LOG=1
endif

