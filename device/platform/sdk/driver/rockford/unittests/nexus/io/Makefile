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
# $brcm_Revision: Hydra_Software_Devel/6 $
# $brcm_Date: 8/6/12 11:22a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/io/Makefile $
# 
# Hydra_Software_Devel/6   8/6/12 11:22a erickson
# SW7231-929: add unittest for i2c_virtualization
# 
# Hydra_Software_Devel/5   8/4/10 11:27a erickson
# SW7400-2843: add ir_input test apps
#
# Hydra_Software_Devel/4   7/30/10 10:55a erickson
#
# SW7400-2845: add ir_blaster stress test
#
# Hydra_Software_Devel/3   2/27/09 4:33p erickson
# PR51468: added ir_input_open
#
# Hydra_Software_Devel/2   2/18/09 9:37a erickson
# PR52223: add i2c app
#
# Hydra_Software_Devel/1   11/6/08 2:15p erickson
# PR42679: added sejin_ir test
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
	i2c \
	i2c_virtualization \
	ir_blaster \
	ir_input \
	ir_input_open \
	ir_input_quiet \
	sejin_ir

include ../common/Makefile.inc
