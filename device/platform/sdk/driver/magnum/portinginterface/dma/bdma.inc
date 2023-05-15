############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bdma.inc $
# $brcm_Revision: Hydra_Software_Devel/27 $
# $brcm_Date: 12/8/11 1:53p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/dma/bdma.inc $
# 
# Hydra_Software_Devel/27   12/8/11 1:53p mward
# SW7125-1165: Replace findstring with filter to do exact word match.
# 
# Hydra_Software_Devel/26   4/4/11 11:45p vanessah
# SWDTV-6186: 35233 DMA support
# 
# Hydra_Software_Devel/25   2/2/11 9:50a vanessah
# SW7550-670: add Sharf mem DMA support for 7550 B0
# 
# Hydra_Software_Devel/24   1/18/11 3:36p hongtaoz
# SW35125-56: fixed compile error for chips other than 35125/230;
# 
# Hydra_Software_Devel/23   1/18/11 5:53p cdisc
# SW35230-2816: bdma_mem_priv.c was not being pulled in by bdma.inc for
# 35230
# 
# Hydra_Software_Devel/22   1/18/11 4:53p cdisc
# SW35125-56: findstring didn't catch the 35125, replacing with ifeq
# 
# Hydra_Software_Devel/21   12/22/10 3:03p vanessah
# SWBLURAY-23683:
# Add DMA PortingInterface support for Blast (7640) chip
# 
# Hydra_Software_Devel/20   12/1/10 12:08p cdisc
# SW35230-2451: adding Sharf support for 35230 C0
# 
# Hydra_Software_Devel/19   11/17/10 4:13p vanessah
# SW7231-18:  Add DMA support for 7231 7344 7346
# 
# Hydra_Software_Devel/18   11/9/10 5:10p vanessah
# SW35125-11:  DMA PI support for 35125
# 
# Hydra_Software_Devel/17   11/4/10 1:41p vanessah
# SW7552-14:  Add DMA PI support
# 
# Hydra_Software_Devel/16   11/1/10 1:56p yuxiaz
# SW7358-15: Added DMA PI support on 7358.
# 
# Hydra_Software_Devel/15   8/31/10 2:31p tdo
# SW7422-62: Add DMA PI
#
# Hydra_Software_Devel/14   2/2/10 3:07p syang
# SW7408-68: add 7408 support
#
# Hydra_Software_Devel/13   12/1/09 11:53a rpan
# SW7468-19: Added support for 7468.
#
# Hydra_Software_Devel/12   9/16/09 6:05p tdo
# SW7630-13: Fix 7630 compiling error
#
# Hydra_Software_Devel/11   9/15/09 11:08p tdo
# SW7630-13: Remove compiling error for 7630 DMA
#
# Hydra_Software_Devel/10   9/3/09 1:58p tdo
# SW7630-13: Add support for 7630
#
# Hydra_Software_Devel/9   4/7/09 4:09p syang
# PR 52973: add 7635 support
#
# Hydra_Software_Devel/8   8/19/08 3:19p syang
# PR 43729:  add sharf files for 7601
#
# Hydra_Software_Devel/7   7/11/08 11:48a syang
# PR 43729:  make bdma.h chip independent
#
# Hydra_Software_Devel/6   2/22/08 9:50a rpan
# PR39458: Initial work for bcm3556.
#
# Hydra_Software_Devel/5   2/21/08 12:12p rpan
# PR34854: Got codes to compile for bcm3548.
#
# Hydra_Software_Devel/4   11/8/07 11:12a tdo
# PR36046: Basic DMA support for 7440 C0
#
# Hydra_Software_Devel/3   11/7/07 5:29p tdo
# PR35401: Temporary remove sharf functionality for 7440C0 compilation.
#
# Hydra_Software_Devel/2   3/19/07 4:37p syang
# PR 28171: add sharf files
#
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
#
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BDMA

# Module Include Path
BDMA_INCLUDES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)

# Module Sources
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma.c
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem.c
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem_priv.c

BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_pci.c
ifeq ($(filter $(BCHP_CHIP), 3548 3556 7630 7640 7468 7408 7422 7425 7358 7552 7231 7344 7346 7550),)
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_pci_priv.c
endif

BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_priv.c
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_queue.c

ifeq (${BCHP_CHIP}, 7440)
ifneq (${BCHP_VER}, A0)
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem_sharf.c
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem_sharf_priv.c
endif
endif

ifeq (${BCHP_CHIP}, 7550)
ifeq (${BCHP_VER}, B0)
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem_sharf.c
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem_sharf_priv.c
endif
endif

ifeq (${BCHP_CHIP}, 35230)
ifeq (${BCHP_VER}, C0)
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem_sharf.c
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem_sharf_priv.c
endif
endif


ifneq ($(filter ${BCHP_CHIP}, 7635 7640 7601 7630 35233),)
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem_sharf.c
BDMA_SOURCES += $(MAGNUM)/portinginterface/dma/$(BCHP_CHIP)/bdma_mem_sharf_priv.c
endif



