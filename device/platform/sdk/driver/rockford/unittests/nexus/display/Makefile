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
# $brcm_Revision: Hydra_Software_Devel/31 $
# $brcm_Date: 7/13/12 11:11a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/display/Makefile $
# 
# Hydra_Software_Devel/31   7/13/12 11:11a ahulse
# SW7425-3310: fix write_vbi and Makefile so it compiles and runs.
# 
# Hydra_Software_Devel/30   10/27/11 3:32p erickson
# SW7425-1581: add display_crc unittest
# 
# Hydra_Software_Devel/29   9/8/11 2:18p erickson
# SW7425-1267: add unittest for GFD
# 
# Hydra_Software_Devel/28   7/11/11 1:07p erickson
# SWDEPRECATED-2425: add display/heaps
# 
# Hydra_Software_Devel/27   6/28/11 9:46a erickson
# SW7405-5366: add unittest for synclock master and delay status
# 
# Hydra_Software_Devel/26   1/8/10 3:15p erickson
# SW3556-979: added vbi encode stress test
#
# Hydra_Software_Devel/25   6/5/09 11:32a erickson
# PR55427: add pip_swap test
#
# Hydra_Software_Devel/24   3/23/09 1:31p erickson
# PR42679: update
#
# Hydra_Software_Devel/23   3/19/09 2:27p erickson
# PR53151: added stress test of NEXUS_Display_SetCustomFormatSettings
#
# Hydra_Software_Devel/22   2/24/09 11:17a erickson
# PR52428: add panel_tests
#
# Hydra_Software_Devel/21   2/20/09 10:54a erickson
# PR52270: add test
#
# Hydra_Software_Devel/20   1/30/09 2:59p erickson
# PR44764: remove external VDB support. nexus now supports VDB and
# dynamic RTS internally.
#
# Hydra_Software_Devel/19   1/19/09 12:25p erickson
# PR50742: test app which recreated PR 50742 quickly
#
# Hydra_Software_Devel/18   11/26/08 11:07a erickson
# PR42679: automate test
#
# Hydra_Software_Devel/17   10/27/08 4:30p jtna
# PR42679: added new unittest display_vbi
#
# Hydra_Software_Devel/16   10/24/08 11:31a erickson
# PR42679: add tests
#
# Hydra_Software_Devel/15   10/23/08 3:59p erickson
# PR42679: update
#
# Hydra_Software_Devel/14   10/16/08 1:56p erickson
# PR47892: fixes
#
# Hydra_Software_Devel/13   8/22/08 11:08p erickson
# PR45711: added unittest for analog channel change
#
# Hydra_Software_Devel/12   7/11/08 3:51p erickson
# PR44764: initial checkin
#
# Hydra_Software_Devel/11   7/9/08 12:08p erickson
# PR42679: update Makefile
#
# Hydra_Software_Devel/10   7/7/08 4:06p erickson
# PR44619: test variations of open/close
#
# Hydra_Software_Devel/9   5/14/08 2:30p erickson
# PR39257: add bypass_display test
#
# Hydra_Software_Devel/8   5/14/08 10:07a erickson
# PR42679: eliminate duplicate code in unittest Makefiles
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
	analog_channel_change \
	analog_video \
	analog_video2 \
	bypass_display \
	capture_buffer \
	colorclip \
	custom_format \
	digital_cc_capture \
	display_crc \
	display_vbi \
	dtv_pip \
	dynamic_backlight \
	graphics_stress \
	heaps \
	open \
	outputs \
	panel_tests \
	pip_swap \
	pq \
	resize_window \
	video_input_status \
	display_vbi_stress \
	synclock_master

# autodetect, pcinput require user input

include ../common/Makefile.inc

# Explicit rule for exceptions
write_vbi: write_vbi.c ttdata0.c ttdata1.c $(VXOBJS)
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS)

install_write_vbi: write_vbi
	$(CP) write_vbi $(DESTDIR)

