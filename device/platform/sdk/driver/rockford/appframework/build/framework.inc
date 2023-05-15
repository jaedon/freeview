#############################################################################
#
#       Copyright (c) 2003-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: framework.inc $
# $brcm_Revision: Hydra_Software_Devel/49 $
# $brcm_Date: 10/4/12 4:28p $
#
# File Description: system build include file
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/framework.inc $
# 
# Hydra_Software_Devel/49   10/4/12 4:28p syang
# SW7435-337: add /BSEAV/linux/driver/usermode/ to  FRAMEWORK_INCLUDES
# for bcm_driver.h use in linuxuser
# 
# Hydra_Software_Devel/48   9/19/12 3:55p katrep
# SW7445-45:7445 build
# 
# Hydra_Software_Devel/47   9/14/12 4:04p katrep
# SW7445-45:cacheflush not suppoorted on 7445
# 
# Hydra_Software_Devel/46   4/13/11 7:02p pntruong
# SW7425-340: Don't include video shmoo for b0 chip.
#
# Hydra_Software_Devel/45   3/15/11 11:51a pntruong
# SW7422-328: Fixed 97231 build errors.
#
# Hydra_Software_Devel/44   3/14/11 8:26a tdo
# SW7422-328: Add video shmoo to appframework
#
# Hydra_Software_Devel/43   3/10/11 5:51p tdo
# SW7422-328: Add video shmoo to appframework
#
# Hydra_Software_Devel/42   3/9/11 5:26p tdo
# SW7422-328: Add video shmoo to appframework
#
# Hydra_Software_Devel/41   3/9/11 12:26p tdo
# SW7422-328: Add video shmoo to appframework
#
# Hydra_Software_Devel/40   9/28/10 3:04p hongtaoz
# SW7425-10: adding generic interrupt polling support for all PI modules;
#
# Hydra_Software_Devel/39   1/20/10 10:22a kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/SW35230-56/1   1/18/10 6:06p kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/38   8/27/09 3:29p pntruong
# SW7550-28: Included tm for 97401 and 97403.
#
# Hydra_Software_Devel/37   8/27/09 12:04a pntruong
# SW7550-28: Make it more portability.
#
# Hydra_Software_Devel/36   7/23/09 10:19a mward
# PR 55545: Fix typo.
#
# Hydra_Software_Devel/35   7/15/09 5:39p tdo
# PR56690: Undo previous check ins
#
# Hydra_Software_Devel/34   7/15/09 1:45p jhaberf
# PR53796: Updated file so the bgio does not get built for the 935130
# platform.
#
# Hydra_Software_Devel/33   7/15/09 11:35a jhaberf
# PR53796: Added infrastruction for 35130 build.
#
# Hydra_Software_Devel/32   7/14/09 3:57p tdo
# PR56690: AppFramework: add calls to BPWR_Open/BPWR_Close before calling
# BCHP_Open
#
# Hydra_Software_Devel/31   7/10/09 2:02p darnstein
# PR55219: tm.inc not needed for 7340, 7342, 7550.
#
# Hydra_Software_Devel/30   6/22/09 4:40p jessem
# PR 54380, PR 54881: Partitioned client and server codes for emulation
# and replaced linuxclient with linuxemu.
#
# Hydra_Software_Devel/29   6/16/09 1:48p darnstein
# PR55763: compile BGIO module on 7340.
#
# Hydra_Software_Devel/28   4/28/09 3:18p jessem
# PR 54380: Changed -DIKOS to -DEMULATION
#
# Hydra_Software_Devel/27   4/28/09 5:53p jhaberf
# PR53796: Added support for the BCM35130 DTV chip.
#
# Hydra_Software_Devel/26   1/27/09 8:17p tdo
# PR51638: Add appframework support for 7336
#
# Hydra_Software_Devel/25   1/20/09 9:17a rpan
# PR50391: Bring up HDMI output.
#
# Hydra_Software_Devel/24   7/14/08 1:30p rpan
# PR36501: Enabled I2C support for 93539.
#
# Hydra_Software_Devel/23   6/13/08 11:26a jessem
# PR 38765: Pulled in FRAMEWORK_SOURCES from linuxuser, linuxclient, and
# vxworks build.inc.
#
# Hydra_Software_Devel/22   3/5/08 12:30p tdo
# PR39459: Basic GIO PI support for 3556
#
# Hydra_Software_Devel/21   2/7/08 5:58p jessem
# PR 39142: Added FRAMEWORK_CFLAGS to CFLAGS.
#
# Hydra_Software_Devel/20   2/1/08 1:55p tdo
# PR38930: Display Bring-up of Application Framework for 3548
#
# Hydra_Software_Devel/19   10/22/07 4:44p jessem
# PR 36129: Removed rule for sims.
#
# Hydra_Software_Devel/18   10/16/07 3:58p jessem
# PR 36129: Added support for IKOS (sims) emulation platform.
#
# Hydra_Software_Devel/17   2/7/07 11:47p pntruong
# PR 27117: Updating to use module specific .inc files.
#
# Hydra_Software_Devel/16   1/16/07 11:27a hongtaoz
# PR23260, PR23280: 3563 bringup added LCD panel ctrl;
#
# Hydra_Software_Devel/15   6/20/06 9:58a maivu
# PR 22212: Moved framework sources to OS build.inc
#
# Hydra_Software_Devel/14   6/19/06 4:35p maivu
# PR 22212: Rearrange framework directories
#
# Hydra_Software_Devel/13   4/13/06 2:41p syang
# PR 20748: use FRAMEWORK_DO_FPGA / I2C to decide whether include
# fpga.inc and tm.inc
#
# Hydra_Software_Devel/12   4/12/06 11:09a syang
# PR 20748: remove tm.inc, which mess up isr?
#
# Hydra_Software_Devel/11   4/10/06 5:56p syang
# PR 20748: fix 97398 compile problem
#
# Hydra_Software_Devel/10   4/10/06 2:18p syang
# PR 20748:  added fpga.inc tm.inc for fpga program
#
# Hydra_Software_Devel/9   4/10/06 10:47a jessem
# PR18367: Added VxWorks support.
#
# Hydra_Software_Devel/8   11/10/05 2:04p jasonh
# PR 18068: Updating directory structure.
#
# Hydra_Software_Devel/7   9/26/05 5:18p darnstein
# PR16885: change the way the BSWCHMGR code is compiled.
#
# Hydra_Software_Devel/6   9/12/05 7:19p darnstein
# PR16885: Add support for Sony mux in BCM93560. Needs to be refactored
# still.
#
# Hydra_Software_Devel/5   8/31/05 5:21p jasonh
# PR 16943: Removed fpga.c
#
# Hydra_Software_Devel/4   7/27/05 4:41p jasonh
# PR 16397: Added fpga specific file to list.
#
# Hydra_Software_Devel/3   7/27/05 4:29p jasonh
# PR 16397: Added main.c
#
# Hydra_Software_Devel/2   6/28/05 4:38p jasonh
# PR 16012: Fixed framework soruce path.
#
# Hydra_Software_Devel/1   1/19/05 4:26p jasonh
# PR 13863: Upgraded version of application framework.
#
# Hydra_Software_Devel/4   2/4/04 7:57a marcusk
# PR9588: Added support for fpga, tested and working.
#
# Hydra_Software_Devel/3   12/16/03 1:12p frusso
# PR8916: Renamed variables to be framework specific
#
# Hydra_Software_Devel/2   12/11/03 8:28a frusso
# PR8916: added framework.c
#
# Hydra_Software_Devel/1   12/9/03 2:34p frusso
# PR8916: initial checkin
#
#############################################################################

#############################################################################
# The purpose of this makefile is to add object files, include paths,
# and C-flags required to build the base framework functions.
# The following defines are the result:
#  FRAMEWORK_SOURCES
#  FRAMEWORK_INCLUDES
#  FRAMEWORK_CFLAGS

# Include other common modules (adds to COMMON_SOURCES, COMMON_INCLUDES, and
# COMMON_CFLAGS)
ifeq "$(FRAMEWORK_DO_FPGA)" "1"
include $(FRAMEWORK_DIR)/build/modules/common/fpga.inc
endif

ifeq "$(FRAMEWORK_DO_I2C)" "1"
ifneq ($(findstring $(PLATFORM), 97401 97403),) #DEFAULT: Not include
include $(FRAMEWORK_DIR)/build/modules/common/tm.inc
endif
endif

ifneq ($(findstring $(PLATFORM), 97422 97425 97344 97346 97231),) #DEFAULT: Not include
ifneq ($(findstring $(BCHP_VER), A0 A1),) #DEFAULT: Not include
include $(ROCKFORD_DIR)/applications/video_shmoo_test/video_shmoo.inc
FRAMEWORK_CFLAGS += -DFRAMEWORK_DO_SHMOO
endif
endif

vpath %.c $(FRAMEWORK_DIR)/src/$(TARGET_OS)
vpath %.c $(FRAMEWORK_DIR)/src/$(TARGET_OS)/client

FRAMEWORK_INCLUDES += \
	src/ \
	src/common/appframework \
	src/board/$(PLATFORM)/

ifneq (${TARGET_OS}, linuxemu)
FRAMEWORK_INCLUDES += src/$(TARGET_OS)/
ifeq (${TARGET_OS}, linuxuser)
FRAMEWORK_INCLUDES += ../../BSEAV/linux/driver/usermode/
endif
else
FRAMEWORK_INCLUDES += src/$(TARGET_OS)/client/
endif

FRAMEWORK_SOURCES += \
	framework.c \
	systeminit.c \
	intmgr.c \
	memmgr.c

ifeq (${TARGET_OS}, vxworks)
FRAMEWORK_SOURCES += \
	int1.c
else
ifeq (${TARGET_OS}, linuxemu)
FRAMEWORK_SOURCES += \
	main.c \
	mutex.c \
	pagelib.c \
	int1.c \
	client.c
else
ifeq (${TARGET_OS}, linuxpal)
FRAMEWORK_SOURCES += \
	main.c \
	pciconf.c \
	framework_pal.c
else
FRAMEWORK_SOURCES += \
	int1.c \
	main.c \
	framework_board.c
endif
endif
endif

FRAMEWORK_CFLAGS +=

ifeq ($(TARGET_OS),linuxemu)
FRAMEWORK_CFLAGS += -DEMULATION
else
ifeq (${TARGET_OS}, linuxpal)
FRAMEWORK_CFLAGS += -DPALLADIUM_EMULATION
endif
endif
