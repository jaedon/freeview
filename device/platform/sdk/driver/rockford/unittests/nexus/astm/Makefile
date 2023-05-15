############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/6 $
# $brcm_Date: 10/19/11 2:51p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/astm/Makefile $
# 
# Hydra_Software_Devel/6   10/19/11 2:51p mward
# SW7125-1119:  Change DEBUG to B_REFSW_DEBUG.
# 
# Hydra_Software_Devel/5   3/13/09 3:55p vsilyaev
# PR 53225: Added NEXUS_BIN_DIR_SUFFIX and NEXUS_BIN_DIR
# 
# Hydra_Software_Devel/4   1/23/09 4:21p jtna
# PR51390: Make NEXUS_ASTM_TEST_SUPPORT overridable
# 
# Hydra_Software_Devel/3   1/8/09 11:12p bandrews
# PR50938: Added tests for ALC enabled/disabled
# 
# Hydra_Software_Devel/2   12/12/08 6:17p bandrews
# PR50079: Updated test framework
# 
# Hydra_Software_Devel/1   12/8/08 2:47p bandrews
# PR50079: Create ASTM unit test framework
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = astm_test create_destroy enable_alc disable_alc apts_apts
DESTDIR=../../../../nexus/bin${NEXUS_BIN_DIR_SUFFIX}

ifndef NEXUS_ASTM_TEST_SUPPORT
export NEXUS_ASTM_TEST_SUPPORT=y
endif

ifeq ($(B_REFSW_DEBUG),n)
OBJ_DIR_SUFFIX := release
else
OBJ_DIR_SUFFIX := debug
endif

ifdef MODE
OBJ_DIR = $(NEXUS_TOP)/platforms/$(PLATFORM)/bin${NEXUS_BIN_DIR_SUFFIX}/linuxuser.$(B_REFSW_ARCH).$(MODE).$(OBJ_DIR_SUFFIX)
else
OBJ_DIR = $(NEXUS_TOP)/platforms/$(PLATFORM)/bin${NEXUS_BIN_DIR_SUFFIX}/linuxuser.$(B_REFSW_ARCH).$(OBJ_DIR_SUFFIX)
endif

all: cleanastm nexus apps

cleanastm:
	@echo "[Cleaning ${OBJ_DIR}/ASTM/nexus_astm.*]"
	@rm -f ${OBJ_DIR}/ASTM/nexus_astm.*

include ../common/Makefile.inc

