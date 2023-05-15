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
# $brcm_Revision: Hydra_Software_Devel/8 $
# $brcm_Date: 2/20/12 1:23p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/dma/Makefile $
# 
# Hydra_Software_Devel/8   2/20/12 1:23p jessem
# SW7425-2211: Added back dma_bulk, dma_crypto, dma_mixedmode, and
# dma_mixedmode_stress.
#
# Hydra_Software_Devel/7   2/17/12 4:24p jessem
# SW7425-2211: Reverted back to previous version due to missing
# HSM/SECURITY  labels.
#
# Hydra_Software_Devel/6   2/17/12 11:59a jessem
# SW7425-2211: Added back security support for 7425B2.
#
# Hydra_Software_Devel/5   2/6/12 3:36p jessem
# SW7425-2211: Temporarily removed dma_bulk, dma_crypto, dma_mixedmode,
# dma_mixedmode_stress until the 7425 B2 HSM symlinks are created.
#
# Hydra_Software_Devel/4   2/6/12 2:38p jessem
# SW7425-2211: Temporarily removed dma_bulk, dma_crypto, dma_mixedmode,
# dma_mixedmode_stress unitl the B2 HSM symlinks are created.
#
# Hydra_Software_Devel/3   5/27/11 12:24p erickson
# SWDEPRECATED-3044: compile all
#
# Hydra_Software_Devel/2   9/29/10 2:31p jtna
# SW7420-1010: added dma_multi.c
#
# Hydra_Software_Devel/1   6/18/08 3:18p vsilyaev
# PR 42392: DMA unit test
#
# Hydra_Software_Devel/9   5/14/08 2:30p erickson
# PR39257: add bypass_display test
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
	dma_chain \
	dma_api_usage \
	dma_callbacks \
	dma_crypto \
	dma_mixedmode \
	dma_mixedmode_stress

include ../common/Makefile.inc


