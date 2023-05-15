############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#  $brcm_Workfile: Makefile $
#  $brcm_Revision: 7 $
#  $brcm_Date: 3/11/11 9:15a $
# 
#  Revision History:
#  $brcm_Log: /BSEAV/lib/livemedia/blive_ext/build/Makefile $
# 
# 7   3/11/11 9:15a ssood
# SW7420-1617: set default value of B_REFSW_ARCH
# 
# 6   3/9/11 7:47p ssood
# SW7420-1617: use B_REFSW_ARCH instead of ARCH for default arch variable
# 
# 5   4/22/10 4:10p mward
# SW7400-2511: Remove debug.
# 
# 4   4/22/10 4:02p mward
# SW7400-2511: don't B_REFSW_ to ARCH, BSEAV build system uses ARCH.
# 
# 3   4/22/10 12:02p mward
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
# 
# 2   10/2/06 8:02a ssood
# PR24576: Added Keywords to enable revision history
# 
#  blive_ext Makefile
############################################################

include ../../../../api/build/tools.mak
BSEAV = $(shell cd "../../../.." && ${PWD})

include $(BSEAV)/lib/livemedia/blive_ext/include/blive_ext.mak
include $(BSEAV)/build/refsw_inc.mak

B_REFSW_ARCH ?= mipsel-uclibc
BLIVE_EXT_ODIR = $(BLIVE_EXT_DIR)/lib/$(B_REFSW_ARCH).$(DEBUG_SUFFIX)
BLIVE_EXT_LIB = $(BLIVE_EXT_ODIR)/libblive_ext.a
LIVEMEDIA_DIR = $(BSEAV)/lib/livemedia/live

.PHONY: check_environment clean
all: check_environment $(BLIVE_EXT_LIB)

ifeq ($(VERBOSE),)
MAKEFLAGS += -s
endif

ifeq ($(SYSTEM),vxworks)
CFLAGS += -I$(WIND_BASE)/target/h
endif
CFLAGS += -W -Wall
CFLAGS += -I$(BLIVE_EXT_DIR)/include -I$(BLIVE_EXT_DIR)/src \
	-I$(BSEAV)/api/include \
	-I$(LIVEMEDIA_DIR)/liveMedia/include \
	-I$(LIVEMEDIA_DIR)/groupsock/include \
	-I$(LIVEMEDIA_DIR)/UsageEnvironment/include \
	-I$(LIVEMEDIA_DIR)/BasicUsageEnvironment/include
CFLAGS += $(B_REFSW_CFLAGS) $(B_REFSW_GENERIC_MAGNUM_CFLAGS) $(B_REFSW_MAGNUM_INCLUDE_DIRS)
CXXFLAGS += $(CFLAGS)

vpath %.cpp $(BLIVE_EXT_DIR)/src

BLIVE_EXT_OBJS = bsap.o bscheduler.o

$(BLIVE_EXT_ODIR)/%.o: %.cpp
	@echo [Compile... $(notdir $<)]
	${Q_}$(CXX) $(CXXFLAGS) -c $< -o $@

$(BLIVE_EXT_LIB): $(addprefix $(BLIVE_EXT_ODIR)/,$(BLIVE_EXT_OBJS))
	@echo [Linking... $(notdir $@)]
	${Q_}$(RM) $@
	${Q_}$(AR) cr $@ $^
	${Q_}$(RANLIB) $@ 

clean:
	-@$(RM) -rf $(BLIVE_EXT_ODIR)

check_environment: $(BLIVE_EXT_DIR)/lib $(BLIVE_EXT_ODIR)

$(BLIVE_EXT_DIR)/lib:
	${Q_}$(MKDIR) "$@"

$(BLIVE_EXT_ODIR):
	${Q_}$(MKDIR) "$@"

include $(BSEAV)/api/build/rules.mak
