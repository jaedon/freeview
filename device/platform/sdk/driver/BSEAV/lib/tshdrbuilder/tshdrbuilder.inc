############################################################
#     Copyright (c) 2007 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: tshdrbuilder.inc $
# $brcm_Revision: 1 $
# $brcm_Date: 5/18/11 1:57p $
#
# Module Description:
#   TS header builder library
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/tshdrbuilder/tshdrbuilder.inc $
# 
# 1   5/18/11 1:57p hongtaoz
# SW7425-42, SW7405-4957: add make file for library compile;
# 
# 
############################################################

TSHDRBUILDER_INCLUDES := \
    $(BSEAV)/lib/tshdrbuilder \
    $(BSEAV)/lib/mpeg2_ts_parse

TSHDRBUILDER_SOURCES :=  \
    ${BSEAV}/lib/tshdrbuilder/tshdrbuilder.c \
    ${BSEAV}/lib/mpeg2_ts_parse/ts_psi.c \
    ${BSEAV}/lib/mpeg2_ts_parse/ts_pat.c \
    ${BSEAV}/lib/mpeg2_ts_parse/ts_pmt.c \
    ${BSEAV}/lib/mpeg2_ts_parse/ts_priv.c


