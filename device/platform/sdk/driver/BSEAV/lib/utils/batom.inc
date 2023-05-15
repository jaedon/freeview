############################################################
#     Copyright (c) 2007-2011 Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: batom.inc $
# $brcm_Revision: 1 $
# $brcm_Date: 4/27/11 5:53p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/utils/batom.inc $
# 
# 1   4/27/11 5:53p vsilyaev
# SW7425-232: batom component
# 
############################################################

# ATOM module
BATOM_INCLUDES := ${BSEAV}/lib/utils
BATOM_SOURCES :=  \
	${BSEAV}/lib/utils/bioatom.c \
	${BSEAV}/lib/utils/bpool.c \
	${BSEAV}/lib/utils/balloc.c \
	${BSEAV}/lib/utils/barena.c

