############################################################
#     Copyright (c) 2003-2004, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: gfx.inc $
# $brcm_Revision: 1 $
# $brcm_Date: 2/7/05 6:53p $
#
# Module Description:
#
# Revision History:
#
# Created: 02/09/2001 by Marcus Kellerman
#
# $brcm_Log: /BSEAV/api/build/magnum/gfx.inc $
# 
# 1   2/7/05 6:53p dlwin
# Merge down for release 2005_REFSW_MERGETOMAIN:
# 
# Irvine_BSEAVSW_Devel/2   5/12/04 10:23a erickson
# PR10967: added standard header
############################################################

R_INC += ${ROCKFORD}/applications/mvd_vdc_test/
R_OBJ += \
 ${ROCKFORD}/applications/mvd_vdc_test/bgfx_image \
 ${ROCKFORD}/applications/mvd_vdc_test/static_buffer

