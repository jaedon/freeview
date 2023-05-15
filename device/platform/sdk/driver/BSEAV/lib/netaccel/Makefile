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
# $brcm_Revision: 21 $
# $brcm_Date: 9/6/12 11:28p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /BSEAV/lib/netaccel/Makefile $
# 
# 21   9/6/12 11:28p mphillip
# SW7425-3739: Merge symlink removal to main
# 
# SW7425-3739/1   9/5/12 5:16p mphillip
# SW7425-3739: Remove BSEAV/lib symlinks
# 
# 20   5/16/12 2:43p katrep
# SW7346-751:replace spaces with tab
# 
# 19   4/2/12 11:23a jrubio
# SW7346-751: add logic for Makefile
# 
# 18   4/2/12 11:02a jrubio
# SW7346-751: add 33 install
# 
# 17   1/24/12 5:45p jrubio
# SW7346-647: fix for 2637
# 
# 16   1/24/12 4:54p jrubio
# SW7346-647: remove 2618/2612 support add 2637 support
# 
# 15   1/20/12 1:43p jrubio
# SW7346-647: add Kbuild for 2631(37)
# 
# 14   3/24/11 5:01p jrubio
# SW7400-2511: take out ARCH and use B_REFSW_ARCH
# 
# 13   3/4/10 5:22p jrubio
# SW7420-228: change directory structure for 2631
# 
# 12   10/9/09 2:32p lwhite
# SW7420-228: Add 2.6.31 support
# 
# 11   4/8/08 5:51p ssood
# PR40689: refactoring to add Linux kernel abstraction layer
# 
# 10   3/19/08 12:36p ssood
# PR39645: defined NETACCEL_SUPPORT variable by default to allow
# buildling from lib/netaccel directory w/o requiring definition of this
# variable in build env
# 
# 9   2/18/08 2:34p ssood
# PR39645: Add NetAccel support in User Mode Builds
# 
# 8   2/17/08 1:32p ssood
# PR36422: BCMEMAC support flag was not being defined correctly
# 
# 7   11/28/07 5:44p mward
# PR35552: Remove debug echo.
# 
# 6   11/28/07 5:27p mward
# PR35552: Move netaccel driver copying logic from Brutus makefile to
# netaccel one.
# 
# 5   10/17/07 2:22p mward
# PR34172: SQA requests LINUX default to /opt/brcm/linux if not defined.
# 
# 4   9/6/07 4:03p jrubio
# PR34597: added ability to not compile lib and app in usermode
# 
# 3   8/21/07 10:17a jrubio
# PR33903: seperated BCMEMAC support
# 
# 2   8/10/07 3:26p jrubio
# PR33786:fixed -j issue, renamed makefile->Makefile, added new bin
# 
# 1   8/10/07 12:18a ssood
# PR33786: changes to accomodate Net DMA & IP Streamer refactoring into
# bcmnetaccel driver
# 
# 2   5/18/07 5:14p jrubio
# PR30010: added changes
# 
# 1   10/11/06 3:39p jrubio
# PR24672:Initial checkin for IPStreamer lib. Yasantha's ip streamer code
# with some mods.
#
############################################################

LINUX ?= /opt/brcm/linux

include ../../api/build/tools.mak
include chip.mak

TOP_DIR 	= $(shell ${PWD})
BSEAV 	?= $(shell cd ../..;pwd)
NETACCEL_BIN=$(TOP_DIR)/bin/${BCHP_CHIP}
QUIET=@
BUILD_INSTALL_DIR=${TOP_DIR}/bin/test
INSTALL_DIR ?= ${BSEAV}/bin

#Default build is driver and lib dirs. You can 
# make the app dir by "make app"
BUILD_DIRS = module lib 
#ifeq ($(NETACCEL_APP),y)
#BUILD_DIRS += app
#endif

.PHONY: clean all app lib module bin 
# Turn off BCMEMAC support for 7118, 
ifeq ($(findstring $(PLATFORM),97018 97018RNG 97118 97118RNG), $(PLATFORM))
BCMEMAC_SUPPORT=n
else
BCMEMAC_SUPPORT=y
endif

ifneq ($(NETACCEL_SUPPORT),n)
NETACCEL_SUPPORT=y
export NETACCEL_SUPPORT
endif


ifeq (${BCMEMAC_SUPPORT},y)
	export BCMEMAC_SUPPORT=y
	CFLAGS += -DB_HAS_BCMEMAC_SUPPORT
endif 

ifeq (${NETACCEL_SUPPORT},y)
	CFLAGS += -DB_HAS_NETACCEL
	export NETACCEL_MEMDMA_DIRECT=y

all: module lib bin

else
# only make the bcmnetaccel usermode driver when 
# NETACCEL SUPPORT is not on
all: bin module

endif
	
app: bin lib 
	$(QUIET)$(MAKE) TOP_DIR=$(TOP_DIR) -C app all
	$(QUIET)$(MAKE) TOP_DIR=$(TOP_DIR) -C app INSTALL_DIR=$(BUILD_INSTALL_DIR) install

		   
lib: bin module
	$(QUIET)$(MAKE) TOP_DIR=$(TOP_DIR) -C lib all
	$(QUIET)$(MAKE) TOP_DIR=$(TOP_DIR) -C lib INSTALL_DIR=$(BUILD_INSTALL_DIR) install


module: bin 
	$(QUIET)$(MAKE) TOP_DIR=$(TOP_DIR) -C module all
	$(QUIET)$(MAKE) TOP_DIR=$(TOP_DIR) -C module INSTALL_DIR=$(BUILD_INSTALL_DIR) install

bin:
	$(QUIET)$(MKDIR) ${TOP_DIR}/bin
	$(QUIET)$(MKDIR) ${TOP_DIR}/bin/test

copy:
	$(QUIET)echo [Copy... NetAccel Binaries] 
		
clean:
	$(QUIET)for dir in $(BUILD_DIRS); do \
		$(MAKE) TOP_DIR=$(TOP_DIR) -C $$dir clean; \
	done
	$(QUIET)$(RM) -f $(TOP_DIR)/bin/test/*

ifneq ($(wildcard $(TOP_DIR)/module/Makefile),)
install: all
# This will copy binaries from build destination netaccel/bin/test if built
# from available source, and copy pre-built binaries corresponding to the
# ${LINUX} kernel otherwise.
	@if [ -d $(INSTALL_DIR) ]; then \
		cp -f $(TOP_DIR)/bin/test/* $(INSTALL_DIR); \
	fi
	@if [ -d $(BSEAV)/../nexus/bin ]; then \
		cp -f $(TOP_DIR)/bin/test/bcmnetaccel.ko $(BSEAV)/../nexus/bin; \
	fi
else
install: copy
    ifeq ($(shell (grep 'VERSION = 3' ${LINUX}/Makefile >/dev/null && echo 'y')),y)
        ifeq ($(B_REFSW_ARCH),mips-linux)
		$(CP) $(NETACCEL_BIN)/33/be/* $(INSTALL_DIR)
        else
		${QUIET}$(CP) $(filter-out $(NETACCEL_BIN)/33/be, \
		$(wildcard $(NETACCEL_BIN)/33/*)) $(INSTALL_DIR)
        endif
    else	
    ifeq ($(shell (grep 'SUBLEVEL = 31' ${LINUX}/Makefile >/dev/null && echo 'y')),y)
        ifeq ($(SMP),y)
          ifeq ($(B_REFSW_ARCH),mips-linux)
		$(CP) $(NETACCEL_BIN)/2631/be/smp/* $(INSTALL_DIR)
          else
		${QUIET}$(CP) $(filter-out $(NETACCEL_BIN)/2631/smp/be , \
		$(wildcard $(NETACCEL_BIN)/2631/smp/*)) $(INSTALL_DIR)
          endif
        else
          ifeq ($(B_REFSW_ARCH),mips-linux) 	
		${QUIET}$(CP) $(filter-out $(NETACCEL_BIN)/2631/be/smp, \
		$(wildcard $(NETACCEL_BIN)/2631/be/*)) $(INSTALL_DIR)
          else
		${QUIET}$(CP) $(filter-out $(NETACCEL_BIN)/2631/smp $(NETACCEL_BIN)/2631/be , \
		$(wildcard $(NETACCEL_BIN)/2631/*)) $(INSTALL_DIR)
          endif
        endif
    else
    ifeq ($(shell (grep 'SUBLEVEL = 37' ${LINUX}/Makefile >/dev/null && echo 'y')),y)
        ifeq ($(B_REFSW_ARCH),mips-linux)
		$(CP) $(NETACCEL_BIN)/2637/be/* $(INSTALL_DIR)
        else
		${QUIET}$(CP) $(filter-out $(NETACCEL_BIN)/2637/be, \
		$(wildcard $(NETACCEL_BIN)/2637/*)) $(INSTALL_DIR)
        endif	
    else
	@echo [Unsupported Linux Version... set NETACCEL_SUPPORT=n]
    endif
    endif
    endif
    
endif
