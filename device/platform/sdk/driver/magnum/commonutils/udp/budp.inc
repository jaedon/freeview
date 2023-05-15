############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: budp.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 9/6/12 10:22p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /magnum/commonutils/udp/budp.inc $
# 
# Hydra_Software_Devel/5   9/6/12 10:22p darnstein
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
# Hydra_Software_Devel/4   4/22/11 5:26p darnstein
# SW7335-1235: Compilation of budp_dccparse_dss.c is now conditional, but
# "on" by default.
# 
# Hydra_Software_Devel/3   10/21/10 4:37p darnstein
# SW7401-2571: cut over to the DSS userdata parser that DSS customer
# engineers perfer.
# 
# Hydra_Software_Devel/2   10/6/10 6:45p darnstein
# SW7405-4817: compile the new JP3D message parser.
# 
# Hydra_Software_Devel/1   7/27/10 5:04p darnstein
# SW3548-3022: userdata parsing software.
# 
############################################################

#
# Conditional Options for this Makefile
#
# N/A
#

# Define this module
MAGNUM_MODULES += BUDP

# Module Includes
BUDP_INCLUDES += \
	$(MAGNUM)/commonutils/udp/src \
	$(MAGNUM)/commonutils/udp/include

BUDP_SOURCES += \
	$(MAGNUM)/commonutils/udp/src/budp_dccparse.c \
	$(MAGNUM)/commonutils/udp/src/budp_dccparse_divicom.c \
	$(MAGNUM)/commonutils/udp/src/budp_scteparse.c \
	$(MAGNUM)/commonutils/udp/src/budp_jp3dparse.c \
	$(MAGNUM)/commonutils/udp/src/budp_bitread.c

# By default, DSS software will be compiled.
ifneq ($(BUDP_DSS), n)
	BUDP_SOURCES += $(MAGNUM)/commonutils/udp/src/budp_dccparse_dss.c
endif
