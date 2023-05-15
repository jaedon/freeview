############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bvbi.inc $
# $brcm_Revision: Hydra_Software_Devel/14 $
# $brcm_Date: 9/6/12 10:22p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/vbi/bvbi.inc $
# 
# Hydra_Software_Devel/14   9/6/12 10:22p darnstein
# SW7435-323:
# Added directory element "src".
# Uncataloged directory element "7400".
# Uncataloged directory element "7420".
# Added directory element "include".
# Uncataloged symbolic link "35125".
# Uncataloged symbolic link "35230".
# Uncataloged symbolic link "35233".
# Uncataloged symbolic link "35330".
# Uncataloged symbolic link "3548".
# Uncataloged symbolic link "3556".
# Uncataloged symbolic link "3560".
# Uncataloged symbolic link "3563".
# Uncataloged symbolic link "7118".
# Uncataloged symbolic link "7125".
# Uncataloged symbolic link "7231".
# Uncataloged symbolic link "7325".
# Uncataloged symbolic link "7335".
# Uncataloged symbolic link "7336".
# Uncataloged symbolic link "7340".
# Uncataloged symbolic link "7342".
# Uncataloged symbolic link "7344".
# Uncataloged symbolic link "7346".
# Uncataloged symbolic link "7358".
# Uncataloged symbolic link "7360".
# Uncataloged symbolic link "7401".
# Uncataloged symbolic link "7403".
# Uncataloged symbolic link "7405".
# Uncataloged symbolic link "7408".
# Uncataloged symbolic link "7422".
# Uncataloged symbolic link "7425".
# Uncataloged symbolic link "7429".
# Uncataloged symbolic link "7435".
# Uncataloged symbolic link "7438".
# Uncataloged symbolic link "7440".
# Uncataloged symbolic link "7445".
# Uncataloged symbolic link "7468".
# Uncataloged symbolic link "7550".
# Uncataloged symbolic link "7552".
# Uncataloged symbolic link "7563".
# Uncataloged symbolic link "7584".
# Uncataloged symbolic link "7601".
# Uncataloged symbolic link "7630".
# Uncataloged symbolic link "7635".
# Uncataloged directory element "7038".
# Uncataloged directory element "7640".
# SW7435-323: implementation files for BUDP.
# Added file element "budp_bitread.h".
# Added file element "budp.h".
# Added file element "budp_bitread.c".
# Added file element "budp_dccparse.c".
# Added file element "budp_dccparse.h".
# Added file element "budp_dccparse_divicom.c".
# Added file element "budp_dccparse_dss.c".
# Added file element "budp_dccparse_dss.h".
# Added file element "budp_jp3dparse.c".
# Added file element "budp_jp3dparse.h".
# Added file element "budp_scteparse.c".
# Added file element "budp_scteparse.h".
# Added file element "budp_seiparse.c".
# Added file element "budp_seiparse.h".
# Added file element "budp_vce.h".
# Uncataloged file element "budp.h".
# Uncataloged file element "budp_bitread.h".
# Uncataloged file element "budp_dccparse.h".
# Uncataloged file element "budp_dccparse_dss.h".
# Uncataloged file element "budp_jp3dparse.h".
# Uncataloged file element "budp_scteparse.h".
# Uncataloged file element "budp_seiparse.h".
# Uncataloged file element "budp_vce.h".
# Added file element "budp_bitread.h".
# SW7435-323: interface files for BUDP.
# Added file element "budp.h".
# Added file element "budp_bitread.h".
# Added file element "budp_dccparse.h".
# Added file element "budp_dccparse_dss.h".
# Added file element "budp_jp3dparse.h".
# Added file element "budp_scteparse.h".
# Added file element "budp_seiparse.h".
# Added file element "budp_vce.h".
# Uncataloged file element "budp_bitread.h".
# SW7435-323:
# Added directory element "src".
# Added directory element "include".
# Uncataloged file element "budp_bitread.h".
# Uncataloged file element "budp.h".
# Uncataloged file element "budp_bitread.c".
# Uncataloged file element "budp_dccparse.c".
# Uncataloged file element "budp_dccparse.h".
# Uncataloged file element "budp_dccparse_divicom.c".
# Uncataloged file element "budp_dccparse_dss.c".
# Uncataloged file element "budp_dccparse_dss.h".
# Uncataloged file element "budp_jp3dparse.c".
# Uncataloged file element "budp_jp3dparse.h".
# Uncataloged file element "budp_scteparse.c".
# Uncataloged file element "budp_scteparse.h".
# Uncataloged file element "budp_seiparse.c".
# Uncataloged file element "budp_seiparse.h".
# Uncataloged file element "budp_vce.h".
# 
# Hydra_Software_Devel/13   12/3/08 8:04p darnstein
# PR45819: compile new, more modular BVBI files.
# 
# Hydra_Software_Devel/12   7/17/08 8:44p darnstein
# PR44539: compilation now possible for 7601.
# 
# Hydra_Software_Devel/11   12/4/07 1:17p darnstein
# PR36897: VBI software is ready for testing with 97335 chipset.
# 
# Hydra_Software_Devel/10   11/1/07 1:11p darnstein
# PR29723: Always compile bvbi_scte.c. This file uses #ifdefs for chips
# that do not have SCTE encoder core hardware.
# 
# Hydra_Software_Devel/9   10/31/07 3:51p darnstein
# PR34528: BVBI is ready for testing on 7325.
# 
# Hydra_Software_Devel/8   9/12/07 4:56p darnstein
# PR29723: For 7405 SCTE encoder core, account for register differences
# with same core in 7403 chipset.
# 
# Hydra_Software_Devel/7   9/11/07 5:18p darnstein
# PR25708: First release of SCTE encoder software.
# 
# Hydra_Software_Devel/6   6/6/07 12:55p darnstein
# PR30411: multi-line closed caption encoding is ready for test.
# 
# Hydra_Software_Devel/6   6/6/07 12:49p darnstein
# PR30411: multi-line closed caption encoding is ready for test.
# 
# Hydra_Software_Devel/5   3/23/07 4:04p darnstein
# PR24573: Don't try to compile software that supports cores not present
# on 7440.
# 
# Hydra_Software_Devel/4   2/26/07 2:28p darnstein
# PR25990: Some minor changes were required for compiling on 7400-B0.
# 
# Hydra_Software_Devel/3   1/25/07 11:37a jasonh
# PR 27117: Added .c to end of source file.
# 
# Hydra_Software_Devel/2   1/17/07 4:54p mward
# PR 27004: Merge special handling of 7118 (has no bvbi_a656).
# 
# Hydra_Software_Devel/1   1/11/07 2:40p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

##
# Chip specific selections!!!
#
ifeq ($(filter $(BCHP_CHIP), 7325 7335 7336 7400 7405), $(BCHP_CHIP))
    BVBI_VEC_DIR = modularvec
else
    BVBI_VEC_DIR = orthogonalvec
endif

# Define this module
MAGNUM_MODULES += BVBI

# Module Includes
BVBI_INCLUDES += \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR) \
	$(MAGNUM)/portinginterface/vbi/src/common \
	$(MAGNUM)/portinginterface/vbi/include

# Module Sources, common.
BVBI_SOURCES += \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_amol.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_cc.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_ccd.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_cgms.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_cgmsd.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_dec.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_enc.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_field.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_gs.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_gsd.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_mcc.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_p656.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_prot.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_scte.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_top.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_tt.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_ttd.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_util_priv.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_vps.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_vpsd.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_wss.c \
	$(MAGNUM)/portinginterface/vbi/src/common/bvbi_wssd.c

# Module sources, chip-specific.
BVBI_SOURCES += \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_a656.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_amole.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_cce.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_cgmse.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_gse.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_in656.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_itu656.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_mcce.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_sctee.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_tte.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_ve.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_veccore.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_vie.c \
 	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_vpse.c \
	$(MAGNUM)/portinginterface/vbi/src/$(BVBI_VEC_DIR)/bvbi_wsse.c
