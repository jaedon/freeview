############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bape_img_mips.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 6/25/12 4:20p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/ape/build/bape_img_mips.inc $
# 
# Hydra_Software_Devel/5   6/25/12 4:20p jgarrett
# SW7425-2952: Restructuring magnum directories
# 
# Hydra_Software_Devel/4   7/6/10 2:14p jgarrett
# SW7408-102: Supporting removal of codecs based upon licensing
# 
# Hydra_Software_Devel/3   2/2/10 1:12p vsilyaev
# SW7408-63: Added PCM WAV module
# 
# Hydra_Software_Devel/2   12/10/09 7:57p vsilyaev
# SW7408-17: Changed names of FW images
# 
# Hydra_Software_Devel/1   11/20/09 5:24p jgarrett
# SW7408-17: Adding APE
# 
############################################################

MAGNUM_MODULES += BAPE_IMG
BAPE_IMG_INCLUDES := $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380

ifneq ($(BAPE_FILE_IMAGE),y)

BAPE_IMG_SOURCES := $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380/bape_img.c \
                    $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380/bape_img_bkernel.c

ifeq ($(BAPE_AAC_SUPPORT),y)
BAPE_IMG_SOURCES += $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380/bape_img_aac_1_decoder.c
endif
ifeq ($(BAPE_AC3_PASSTHROUGH_SUPPORT),y)
BAPE_IMG_SOURCES += $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380/bape_img_ac3_passthrough.c
endif
ifeq ($(BAPE_DDP_SUPPORT),y)
# DDP includes both AC3 and AC3+ support.
BAPE_IMG_SOURCES += $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380/bape_img_ddp_2_converter.c
else
# Only include AC3 if DDP is not defined
ifeq ($(BAPE_AC3_SUPPORT),y)
BAPE_IMG_SOURCES += $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380/bape_img_ac3_decoder.c
endif
endif
ifeq ($(BAPE_MPEG_SUPPORT),y)
BAPE_IMG_SOURCES += $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380/bape_img_mpeg_1_decoder.c
endif
ifeq ($(BAPE_PCMWAV_SUPPORT),y)
BAPE_IMG_SOURCES += $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380/bape_img_pcm_wav.c
endif
ifeq ($(BAPE_WMA_SUPPORT),y)
BAPE_IMG_SOURCES += $(MAGNUM)/portinginterface/ape/src/mips/firmware/bmips4380/bape_img_wma_1_decoder.c
endif

BAPE_IMG_DEFINES += BAPE_IMG_COMPILED=1
else
BAPE_IMG_DEFINES += BAPE_FILE_IMAGE=1
endif
