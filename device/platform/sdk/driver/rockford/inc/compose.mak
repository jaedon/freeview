############################################################################
#     Copyright (c) 2003, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: compose.mak $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 10/24/03 10:00a $
#
# Module Description:
#
# build various inc files from the component inc files
#
# Revision History:
#
# $brcm_Log: /rockford/inc/compose.mak $
# 
# Hydra_Software_Devel/2   10/24/03 10:00a vsilyaev
# Use Gnu Make build-in rules to generate list of objects and includes.
# 
# Hydra_Software_Devel/1   4/1/03 8:01p vsilyaev
# Framework make include files - initial version.
# 
# 
###########################################################################


MAGNUM_OBJS := $(addprefix ${ODIR}/, $(addsuffix .o, ${notdir ${OBJ}}))
MAGNUM_INC := $(addprefix -I${MAGNUM}/, $(sort ${INC}))

vpath %c $(addprefix ${MAGNUM}/, $(sort $(dir ${OBJ})))



