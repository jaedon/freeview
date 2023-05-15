#############################################################################
#
#       Copyright (c) 2005-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/6 $
# $brcm_Date: 7/10/12 9:25a $
#
# File Description:
# 	Makefile for compiling APE unit test applications
#
# $brcm_Log: /rockford/unittests/portinginterface/ape/97422/Makefile $
# 
# Hydra_Software_Devel/6   7/10/12 9:25a jgarrett
# SW7425-3457: Adding ape_decode application
# 
# Hydra_Software_Devel/5   4/11/12 7:25p jgarrett
# SW7425-2075: Adding HDMITX support and HBR test tone app
# 
# Hydra_Software_Devel/4   9/13/11 11:05a jgarrett
# SW7346-436: Adding application to test SPDIF format switching
# 
# Hydra_Software_Devel/3   8/30/11 5:11p gskerl
# SW7425-1178: Added apex (ape exerciser) to Makefile.
# 
# Hydra_Software_Devel/2   8/15/11 9:33a jgarrett
# SWDTV-8208: Fixing dependencies
# 
# Hydra_Software_Devel/1   8/12/11 4:12p jgarrett
# SWDTV-8208: Resolving issues with ape_test and EC
# 
#############################################################################

# Requires the use of the 'plat' script which defines the following:
#   PLATFORM (97038, 97395, ...)
#   BCHP_REV (B0, B1, C0, ...)
#   DEBUG
#   LINUX
#   ARCH
#   TOOLCHAIN

APPS := ape_decode ape_playback apex spdif_format_switch hbr_test_tone

.PHONY: all
all: $(APPS)

.PHONY: clean
clean: $(foreach app,$(APPS),$(app)_clean)


.PHONY: %_clean
%_clean: 
	@echo [Clean... $(patsubst %_clean,%,$@)]
	@$(MAKE) --no-print-directory -f Makefile.$(patsubst %_clean,%,$@) clean

.PHONY: $(APPS)
$(APPS): 
	@echo [Build... $@]
	@$(MAKE) --no-print-directory -f Makefile.$@

