############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bvbilib.inc $
# $brcm_Revision: Hydra_Software_Devel/6 $
# $brcm_Date: 9/6/12 10:22p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/syslib/vbilib/bvbilib.inc $
# 
# Hydra_Software_Devel/6   9/6/12 10:22p darnstein
# SW7435-323:
# Added directory element "src".
# Added directory element "include".
# Uncataloged file element "bvbilib_bitread.h".
# Uncataloged file element "bvbilib.h".
# Uncataloged file element "bvbilib_dccparse.h".
# Uncataloged file element "bvbilib_dccparse_dss.h".
# Uncataloged file element "bvbilib_dccreorder.h".
# Uncataloged file element "bvbilib_priv.h".
# Uncataloged file element "bvbilib_scteparse.h".
# Uncataloged file element "bvbilib.c".
# Uncataloged file element "bvbilib_bitread.c".
# Uncataloged file element "bvbilib_dccparse.c".
# Uncataloged file element "bvbilib_dccparse_divicom.c".
# Uncataloged file element "bvbilib_dccparse_dss.c".
# Uncataloged file element "bvbilib_dccreorder.c".
# Uncataloged file element "bvbilib_list.c".
# Uncataloged file element "bvbilib_scteparse.c".
# SW7435-323:
# Uncataloged symbolic link "bvbi.c".
# Uncataloged symbolic link "bvbi.h".
# Uncataloged symbolic link "bvbi_amol.c".
# Uncataloged symbolic link "bvbi_cc.c".
# Uncataloged symbolic link "bvbi_ccd.c".
# Uncataloged symbolic link "bvbi_cgms.c".
# Uncataloged symbolic link "bvbi_cgmsd.c".
# Uncataloged symbolic link "bvbi_dec.c".
# Uncataloged symbolic link "bvbi_enc.c".
# Uncataloged symbolic link "bvbi_field.c".
# Uncataloged symbolic link "bvbi_gs.c".
# Uncataloged symbolic link "bvbi_gsd.c".
# Uncataloged symbolic link "bvbi_lcop.h".
# Uncataloged symbolic link "bvbi_mcc.c".
# Uncataloged symbolic link "bvbi_p656.c".
# Uncataloged symbolic link "bvbi_priv.h".
# Uncataloged symbolic link "bvbi_prot.c".
# Uncataloged symbolic link "bvbi_prot.h".
# Uncataloged symbolic link "bvbi_scte.c".
# Uncataloged symbolic link "bvbi_top.c".
# Uncataloged symbolic link "bvbi_tt.c".
# Uncataloged symbolic link "bvbi_ttd.c".
# Uncataloged symbolic link "bvbi_util_priv.c".
# Uncataloged symbolic link "bvbi_util_priv.h".
# Uncataloged symbolic link "bvbi_vps.c".
# Uncataloged symbolic link "bvbi_vpsd.c".
# Uncataloged symbolic link "bvbi_wss.c".
# Uncataloged symbolic link "bvbi_wssd.c".
# SW7435-323:
# Uncataloged file element "bvbi.c".
# Uncataloged file element "bvbi.h".
# Uncataloged file element "bvbi_amol.c".
# Uncataloged file element "bvbi_cc.c".
# Uncataloged file element "bvbi_ccd.c".
# Uncataloged file element "bvbi_cgms.c".
# Uncataloged file element "bvbi_cgmsd.c".
# Uncataloged file element "bvbi_dec.c".
# Uncataloged file element "bvbi_enc.c".
# Uncataloged file element "bvbi_field.c".
# Uncataloged file element "bvbi_gs.c".
# Uncataloged file element "bvbi_gsd.c".
# Uncataloged file element "bvbi_lcop.h".
# Uncataloged file element "bvbi_mcc.c".
# Uncataloged file element "bvbi_p656.c".
# Uncataloged file element "bvbi_priv.h".
# Uncataloged file element "bvbi_prot.c".
# Uncataloged file element "bvbi_prot.h".
# Uncataloged file element "bvbi_scte.c".
# Uncataloged file element "bvbi_top.c".
# Uncataloged file element "bvbi_tt.c".
# Uncataloged file element "bvbi_ttd.c".
# Uncataloged file element "bvbi_util_priv.c".
# Uncataloged file element "bvbi_util_priv.h".
# Uncataloged file element "bvbi_vps.c".
# Uncataloged file element "bvbi_vpsd.c".
# Uncataloged file element "bvbi_wss.c".
# Uncataloged file element "bvbi_wssd.c".
# SW7435-323: common implementation files for BVBI.
# Added file element "bvbi.c".
# Added file element "bvbi.h".
# Added file element "bvbi_amol.c".
# Added file element "bvbi_cc.c".
# Added file element "bvbi_ccd.c".
# Added file element "bvbi_cgms.c".
# Added file element "bvbi_cgmsd.c".
# Added file element "bvbi_dec.c".
# Added file element "bvbi_enc.c".
# Added file element "bvbi_field.c".
# Added file element "bvbi_gs.c".
# Added file element "bvbi_gsd.c".
# Added file element "bvbi_lcop.h".
# Added file element "bvbi_mcc.c".
# Added file element "bvbi_p656.c".
# Added file element "bvbi_priv.h".
# Added file element "bvbi_prot.c".
# Added file element "bvbi_prot.h".
# Added file element "bvbi_scte.c".
# Added file element "bvbi_top.c".
# Added file element "bvbi_tt.c".
# Added file element "bvbi_ttd.c".
# Added file element "bvbi_util_priv.c".
# Added file element "bvbi_util_priv.h".
# Uncataloged file element "bvbi.h".
# Added file element "bvbi_vps.c".
# Added file element "bvbi_vpsd.c".
# Added file element "bvbi_wss.c".
# Added file element "bvbi_wssd.c".
# SW7435-323: implementation files for BVBI.
# Added directory element "modularvec".
# Added directory element "orthogonalvec".
# Added directory element "common".
# SW7435-323: interface files for BVBI.
# Added file element "bvbi.h".
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
# Hydra_Software_Devel/5   8/27/10 1:54p darnstein
# SW3548-3022: merge to production branch.
# 
# Hydra_Software_Devel/SW3548-3022/1   7/28/10 3:05p darnstein
# SW3548-3022: Some software is moved from BVBI porting interface module
# to new BUDP commonutils module.
# 
# Hydra_Software_Devel/4   3/24/10 3:17p darnstein
# SW7405-4084: add function BVBIlib_DCCparse_Divicom() to parse Divicom
# closed caption data.
# 
# Hydra_Software_Devel/3   9/11/07 5:18p darnstein
# PR25708: First release of SCTE encoder software.
# 
# Hydra_Software_Devel/2   6/4/07 6:13p darnstein
# PR31097: compile in bvbilib_dccreorder.c.
# 
# Hydra_Software_Devel/2   6/4/07 6:12p darnstein
# PR31097: compile in bvbilib_dccreorder.c
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
MAGNUM_MODULES += BVBILIB

# Module Includes
BVBILIB_INCLUDES += \
	$(MAGNUM)/syslib/vbilib/src \
	$(MAGNUM)/syslib/vbilib/include

BVBILIB_SOURCES += \
	$(MAGNUM)/syslib/vbilib/src/bvbilib.c \
	$(MAGNUM)/syslib/vbilib/src/bvbilib_dccreorder.c \
	$(MAGNUM)/syslib/vbilib/src/bvbilib_list.c

