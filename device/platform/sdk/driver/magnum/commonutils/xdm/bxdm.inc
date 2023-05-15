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
# $brcm_Workfile: bxdm.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 6/25/12 2:49p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /magnum/commonutils/xdm/bxdm.inc $
# 
# Hydra_Software_Devel/3   6/25/12 2:49p btosi
# SW7425-1064: merged XMO changes from branch
# 
# Hydra_Software_Devel/SW7425-1064/1   11/29/11 1:31p btosi
# SW7425-1064: adding support for XMO
# 
# Hydra_Software_Devel/2   4/28/11 1:51p nilesh
# SW7405-5057:
# Added file element "bxdm_pp_fix33.c".
# Added file element "bxdm_pp_fix33.h".
# 
# Hydra_Software_Devel/1   2/16/10 10:50a nilesh
# SW7405-2993: Initial XDM version
# 
#############################################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BXDM

# Module Includes
BXDM_INCLUDES += $(MAGNUM)/commonutils/xdm/

# Module Sources
BXDM_SOURCES+= $(MAGNUM)/commonutils/xdm/bxdm_dih.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_v2.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_qm.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_tsm.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_callback.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_output.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_fp.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_vtsm.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_dbg.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_clip.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_frd.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_fic.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_jrc.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_timer.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_avg.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_fix33.c \
               $(MAGNUM)/commonutils/xdm/bxdm_pp_xmo.c

ifeq ($(BXDM_PPJRC_DUMP),y)
BXDM_DEFINES += BXDM_PPJRC_P_DUMP=1
endif