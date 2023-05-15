############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bxvd.inc $
# $brcm_Revision: Hydra_Software_Devel/83 $
# $brcm_Date: 10/8/12 5:12p $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/xvd/bxvd.inc $
# 
# Hydra_Software_Devel/83   10/8/12 5:12p davidp
# SW7445-16: Add support for 7445 platform.
# 
# Hydra_Software_Devel/82   9/28/12 5:16p davidp
# SW7360-36: Enable XVD power managment support for 7360 platform.
# 
# Hydra_Software_Devel/81   6/25/12 4:25p davidp
# SW7563-3: Add support for 7563 platform.
# 
# Hydra_Software_Devel/80   6/18/12 4:24p davidp
# SW7435-164: Implement new directory structure.
# 
# Hydra_Software_Devel/79   6/11/12 10:31a pblanco
# SW7405-5668: The environment is now checked for the values of
# BXVD_FLATTEN_USERDATA, BXVD_BREAK_ON_TYPE_CHANGE and
# BXVD_USERDATA_EXTRA_DEBUG and the compile time flags are set
# accordingly
# 
# Hydra_Software_Devel/78   5/21/12 2:45p davidp
# SW7429-155: Enable power management support for 7429 platforms.
# 
# Hydra_Software_Devel/77   4/24/12 4:09p davidp
# SW7584-20: Add support for 7584 platform.
# 
# Hydra_Software_Devel/76   4/19/12 2:16p gmohile
# SW7435-61 : 7435 power management support
# 
# Hydra_Software_Devel/75   2/29/12 2:32p davidp
# SW7468-403: Enable PM support for 7468.
# 
# Hydra_Software_Devel/74   12/20/11 3:45p davidp
# SW7360-4: Add 7360 support.
# 
# Hydra_Software_Devel/73   12/5/11 2:57p davidp
# SW7435-8: Add support for 7435 platform.
# 
# Hydra_Software_Devel/72   11/9/11 6:38p davidp
# SW7425-1683: Add support for 7425 single memory controller
# configurations.
# 
# Hydra_Software_Devel/71   10/26/11 11:56a davidp
# SW7552-138: Add 7552 to list of supported PM V2 platforms.
# 
# Hydra_Software_Devel/70   10/24/11 2:22p davidp
# SW7358-171: Add PM support for 7358.
# 
# Hydra_Software_Devel/69   10/10/11 5:32p davidp
# SW7429-17: Add support for 7429 platform.
# 
# Hydra_Software_Devel/68   8/24/11 6:47p randyjew
# SW7208-158: Disable 7468 Power Management. Not supported
# 
# Hydra_Software_Devel/67   8/23/11 11:40a randyjew
# SW7208-158: Add 7468 to BXVD_POWER_MANAGEMENT
# 
# Hydra_Software_Devel/66   7/20/11 3:00p davidp
# SW7420-2001: Add ARC600 support.
# 
# Hydra_Software_Devel/65   6/28/11 3:31p davidp
# SW7425-533: Enable power management support on 7425.
# 
# Hydra_Software_Devel/64   6/21/11 5:14p davidp
# SWDTV-7621: Enable dynamic power management by default on 35233.
# 
# Hydra_Software_Devel/63   6/20/11 5:31p randyjew
# SW7344-104: Add PM 2.0 for 7344/7346
# 
# Hydra_Software_Devel/62   4/20/11 4:32p davidp
# SW7231-127: Add XVD powermanagement support for 7231 platform.
# 
# Hydra_Software_Devel/61   4/4/11 11:32a btosi
# SW7405-4736: added support for BXVD_DecoderDbg_P_PrintUnifiedPicture
# 
# Hydra_Software_Devel/60   3/21/11 1:54p davidp
# SWDTV-5972: Add support for 35233 platform.
# 
# Hydra_Software_Devel/59   1/24/11 5:14p davidp
# SWBLURAY-23545: Add 7640 chip to revk0 build rule.
# 
# Hydra_Software_Devel/58   12/17/10 9:11a davidp
# SW7552-11: Add 7552 platform.
# 
# Hydra_Software_Devel/57   12/7/10 7:08p davidp
# SW7408-191: Enable dynamic power management on 7408 platform.
# 
# Hydra_Software_Devel/56   11/24/10 1:22p btosi
# SW7422-72: added BXVD_LEGACY_MVC_SUPPORT for MVC on older DVD chips
# 
# Hydra_Software_Devel/55   11/19/10 6:04p davidp
# SW7344-5: Add support for 7344 and 7346
# 
# Hydra_Software_Devel/54   11/15/10 2:49p davidp
# SW35125-20: Add 35125 support in XVD.
# 
# Hydra_Software_Devel/53   10/21/10 2:57p davidp
# SW7231-8: Add support for 7231 and 7135 platforms.
# 
# Hydra_Software_Devel/52   9/10/10 5:50p davidp
# SW7358-11: Add support for AVD Rev K core.
# 
# Hydra_Software_Devel/51   6/16/10 11:32a btosi
# SW7405-4365: added decoder timer module
# 
# Hydra_Software_Devel/50   5/14/10 6:28p davidp
# SW7125-426: Add dynamic power management support for 7125, disabled by
# default.
# 
# Hydra_Software_Devel/49   2/25/10 4:24p nilesh
# SW7405-2993: XDM Merge
# Added file element "bxvd_decoder.c".
# Added file element "bxvd_decoder.h".
# Added file element "bxvd_dip.c".
# Added file element "bxvd_dip.h".
# Uncataloged file element "bxvd_dispmgr.c".
# Uncataloged file element "bxvd_dispmgr.h".
# Uncataloged file element "bxvd_dispmgr_avg.c".
# Uncataloged file element "bxvd_dispmgr_avg.h".
# Uncataloged file element "bxvd_dispmgr_callback.c".
# Uncataloged file element "bxvd_dispmgr_callback.h".
# Uncataloged file element "bxvd_dispmgr_clip.c".
# Uncataloged file element "bxvd_dispmgr_clip.h".
# Uncataloged file element "bxvd_dispmgr_common.c".
# Uncataloged file element "bxvd_dispmgr_dbg.c".
# Uncataloged file element "bxvd_dispmgr_dbg.h".
# Uncataloged file element "bxvd_dispmgr_dvd.c".
# Uncataloged file element "bxvd_dispmgr_fic.c".
# Uncataloged file element "bxvd_dispmgr_fic.h".
# Uncataloged file element "bxvd_dispmgr_flog.c".
# Uncataloged file element "bxvd_dispmgr_flog.h".
# Uncataloged file element "bxvd_dispmgr_fp.c".
# Uncataloged file element "bxvd_dispmgr_fp.h".
# Uncataloged file element "bxvd_dispmgr_frd.c".
# Uncataloged file element "bxvd_dispmgr_frd.h".
# Uncataloged file element "bxvd_dispmgr_jrc.c".
# Uncataloged file element "bxvd_dispmgr_jrc.h".
# Uncataloged file element "bxvd_dispmgr_output.c".
# Uncataloged file element "bxvd_dispmgr_output.h".
# Uncataloged file element "bxvd_dispmgr_priv.h".
# Uncataloged file element "bxvd_dispmgr_priv_v2.h".
# Uncataloged file element "bxvd_dispmgr_qm.c".
# Uncataloged file element "bxvd_dispmgr_qm.h".
# Uncataloged file element "bxvd_dispmgr_timer.c".
# Uncataloged file element "bxvd_dispmgr_timer.h".
# Uncataloged file element "bxvd_dispmgr_tsm.c".
# Uncataloged file element "bxvd_dispmgr_tsm.h".
# Uncataloged file element "bxvd_dispmgr_v2.c".
# Uncataloged file element "bxvd_dispmgr_v2.h".
# Uncataloged file element "bxvd_dispmgr_ver.h".
# Uncataloged file element "bxvd_dispmgr_vtsm.c".
# Uncataloged file element "bxvd_dispmgr_vtsm.h".
# SW7405-2993: XDM Merge
# 
# Hydra_Software_Devel/SW7405-2993/5   2/12/10 3:42p nilesh
# SW7405-2993:
# Uncataloged file element "bxvd_dispmgr_avg.c".
# Uncataloged file element "bxvd_dispmgr_avg.h".
# Uncataloged file element "bxvd_dispmgr_callback.c".
# Uncataloged file element "bxvd_dispmgr_callback.h".
# Uncataloged file element "bxvd_dispmgr_clip.c".
# Uncataloged file element "bxvd_dispmgr_clip.h".
# Uncataloged file element "bxvd_dispmgr_dbg.c".
# Uncataloged file element "bxvd_dispmgr_dbg.h".
# Uncataloged file element "bxvd_dispmgr_fic.c".
# Uncataloged file element "bxvd_dispmgr_fic.h".
# Uncataloged file element "bxvd_dispmgr_fp.c".
# Uncataloged file element "bxvd_dispmgr_fp.h".
# Uncataloged file element "bxvd_dispmgr_frd.c".
# Uncataloged file element "bxvd_dispmgr_frd.h".
# Uncataloged file element "bxvd_dispmgr_jrc.c".
# Uncataloged file element "bxvd_dispmgr_jrc.h".
# Uncataloged file element "bxvd_dispmgr_output.c".
# Uncataloged file element "bxvd_dispmgr_output.h".
# Uncataloged file element "bxvd_dispmgr_qm.c".
# Uncataloged file element "bxvd_dispmgr_qm.h".
# Uncataloged file element "bxvd_dispmgr_timer.c".
# Uncataloged file element "bxvd_dispmgr_timer.h".
# Uncataloged file element "bxvd_dispmgr_tsm.c".
# Uncataloged file element "bxvd_dispmgr_tsm.h".
# Uncataloged file element "bxvd_dispmgr_ver.h".
# Uncataloged file element "bxvd_dispmgr_vtsm.c".
# Uncataloged file element "bxvd_dispmgr_vtsm.h".
# Uncataloged file element "bxvd_dispmgr_v2.c".
# 
# Hydra_Software_Devel/SW7405-2993/4   1/6/10 4:59p nilesh
# SW7405-2993: Separated DM from rest of XVD
# 
# Hydra_Software_Devel/SW7405-2993/3   12/15/09 1:10p nilesh
# SW7405-2993: Reconcile with mainline
# 
# Hydra_Software_Devel/48   12/14/09 11:27a nilesh
# SW7405-3358: Added Jitter Tolerance Improvement (JTI) logic to prevent
# cadence issues caused when deltaStcPts is near 0 and STC snapshot is
# jittery
# 
# Hydra_Software_Devel/SW7405-3358/1   12/10/09 5:21p nilesh
# SW7405-3358: Initial XVD DM Average Computation library
# 
# Hydra_Software_Devel/SW7405-2993/2   12/1/09 11:31a nilesh
# SW7405-2993: Reconcile with mainline
# 
# Hydra_Software_Devel/47   11/30/09 4:24p btosi
# SW7405-3245: added BXVD_DBG_*; add compile time map to BDBG_INSTANCE_*
# or BDBG_*
# 
# Hydra_Software_Devel/SW7405-2993/1   11/20/09 4:43p nilesh
# SW7405-2993:
# Added file element "bxvd_dip.c".
# Added file element "bxvd_dip.h".
# 
# Hydra_Software_Devel/46   11/16/09 6:32p davidp
# SW7468-14: Add support for 7468 and 7408 platforms.
# 
# Hydra_Software_Devel/45   10/30/09 11:23a btosi
# SW7405-3257: adding DM timer module
# 
# Hydra_Software_Devel/44   9/24/09 10:44a nilesh
# SW3548-2418: PTS Jitter Correction
# 
# Hydra_Software_Devel/43   9/21/09 2:38p davidp
# SW35230-3: Add support for 35230 platform.
# 
# Hydra_Software_Devel/42   9/1/09 4:46p davidp
# SW7405-2963: Add support to printout AVD FW log in PDR_isr.
# 
# Hydra_Software_Devel/41   8/31/09 5:23p davidp
# SW7630-31: Add support for 7630 platform.
# 
# Hydra_Software_Devel/40   7/23/09 1:43p davidp
# PR55546: Add 7125 support to xvd.
# 
# Hydra_Software_Devel/39   7/17/09 4:28p davidp
# PR55227: Add support for 7340 and 7342 platforms.
# 
# Hydra_Software_Devel/38   6/10/09 5:15p davidp
# PR55198: Add 7400 to power management supported platforms.
# 
# Hydra_Software_Devel/37   6/4/09 4:04p davidp
# PR54107: Add support for 35130 platform, create Rev J platform family.
# 
# Hydra_Software_Devel/36   6/2/09 2:27p davidp
# PR55198: Power management is disabled by default for settop chips.
# 
# Hydra_Software_Devel/35   6/1/09 6:42p davidp
# PR55198: Add dynamic power management support to 7405, 7325, 7335 and
# 7420 platforms.
# 
# Hydra_Software_Devel/34   4/3/09 5:40p davidp
# PR53571: Add initial 7635 support.
# 
# Hydra_Software_Devel/33   3/26/09 6:13p nilesh
# PR53580: Cleaned up DM structs
# 
# Hydra_Software_Devel/PR53580/1   3/25/09 6:14p nilesh
# PR53580: Added bxvd_dispmgr_common.c
# 
# Hydra_Software_Devel/32   1/30/09 1:16p davidp
# PR51623: Add support for 7336 chip, Rev I core clone.
# 
# Hydra_Software_Devel/31   1/27/09 7:59p nilesh
# PR51644: Added file element "bxvd_dispmgr_fic.c". Added file element
# "bxvd_dispmgr_fic.h".
# 
# Hydra_Software_Devel/30   1/13/09 3:58p nilesh
# PR47456: Added Frame Rate Detection (FRD) logic
# 
# Hydra_Software_Devel/29   10/31/08 3:31p davidp
# PR46543: Implement BXVD_POWER_MANAGEMENT build switch.
# 
# Hydra_Software_Devel/28   10/20/08 12:05p nilesh
# PR47998: Separated DM Clip support to separate file
# 
# Hydra_Software_Devel/27   7/17/08 11:33a nilesh
# PR38467: Document new build options
# 
# Hydra_Software_Devel/26   6/17/08 2:28p nilesh
# PR43585: 7440 now defaults to DMv2
# 
# Hydra_Software_Devel/25   6/12/08 1:20p nilesh
# PR38467: Convert all platforms to use DMv2 by default
# 
# Hydra_Software_Devel/24   5/28/08 3:15p davidp
# PR38955: Use DM V2 for 3548 and 3556.
# 
# Hydra_Software_Devel/23   5/8/08 3:29p nilesh
# PR38467: Merge DMv2 to mainline
# 
# Hydra_Software_Devel/22   4/14/08 5:31p davidp
# PR40234: Build bxvd_dbg.c
# 
# Hydra_Software_Devel/21   4/10/08 5:28p davidp
# PR40234: Disable building bxvd_dbg for now.
# 
# Hydra_Software_Devel/20   4/10/08 3:43p davidp
# PR40234: Add support for decoder debug logging.
# 
# Hydra_Software_Devel/19   2/20/08 6:53p davidp
# PR39495: Add support for 3556, clone of 7405 using AVD core rev i0.
# 
# Hydra_Software_Devel/18   1/29/08 4:39p davidp
# PR38955: Add support for 3548 platform, clone of 7405
# 
# Hydra_Software_Devel/17   11/28/07 5:53p pblanco
# PR36901: Reinstate 7325->7405 clone change.
# 
# Hydra_Software_Devel/16   11/28/07 5:15p pblanco
# PR36901: backout 7325 clone change until version skew is straigtened
# out in xvd/7325 dir.
# 
# Hydra_Software_Devel/15   11/28/07 4:39p pblanco
# PR36901: Clone 7325 from 7405.
# 
# Hydra_Software_Devel/14   11/13/07 5:37p davidp
# PR36901: Add support for 97335, use BXVD_CHIP families, 97335 is same
# as 97405.
# 
# Hydra_Software_Devel/13   11/6/07 12:37p nilesh
# PR36741: Added XVD status reporting via BDBG console messages
# 
# Hydra_Software_Devel/12   10/23/07 5:41p nilesh
# PR34118: Added BXVD_USE_CUSTOM_CONTEXT build flag to optionally link
# firmware image into binary
# 
# Hydra_Software_Devel/11   8/13/07 4:09p nilesh
# PR29915: Multi-decode merge to mainline
# 
# Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/5   6/19/07 12:14p nilesh
# PR29915: Merge from mainline.
# 
# Hydra_Software_Devel/10   6/12/07 11:37a nilesh
# PR31900: Merged makefile cleanup to mainline to support bsettop
# B_CONFIG_IMAGE build option
# 
# Hydra_Software_Devel/PR31900/5   6/11/07 11:07a nilesh
# PR31900: Fixed BXVD_USE_CUSTOM_IMAGE define when using
# BXVD_IMG_EXTERNAL
# 
# Hydra_Software_Devel/PR31900/4   6/7/07 3:06p nilesh
# PR31900: Changes MAGNUM_CFLAGS define to BXVD_DEFINES
# 
# Hydra_Software_Devel/PR31900/3   6/7/07 12:52p nilesh
# PR31900: Documented compilation flags
# 
# Hydra_Software_Devel/PR31900/2   6/7/07 11:20a nilesh
# PR31900: Added BXVD_IMG_EXTERNAL flag
# 
# Hydra_Software_Devel/PR31900/1   6/7/07 10:56a nilesh
# PR31900: Simplified makefile for platform specific files
# 
# Hydra_Software_Devel/9   6/1/07 3:10p nilesh
# PR23100: Fixed compilation on non-7401 platforms
# 
# Hydra_Software_Devel/8   6/1/07 12:05p pblanco
# PR23100: Added bxvd_platform_7401_image.c to build list.
# 
# Hydra_Software_Devel/7   4/20/07 7:40a rayshap
# PR27168: Replaced bxvd_mem in 7400 a0 build, still needed
# 
# Hydra_Software_Devel/6   4/19/07 10:03a pblanco
# PR27645: Added bxvd_platform_7405.c to build list.
# 
# Hydra_Software_Devel/5   3/27/07 2:03p pblanco
# PR27168: Removed deprecated bxvd_mem.c from build.
# 
# Hydra_Software_Devel/4   2/13/07 9:21a pblanco
# PR26188: Changed BCHP_REV to BCHP_VER in chip version test.
# 
# Hydra_Software_Devel/3   2/13/07 9:06a pblanco
# PR26188: Added 7400 B0 support. This can be simplified once the A0 is
# no longer supported.
# 
# Hydra_Software_Devel/2   1/17/07 4:56p rayshap
# PR27140: New directory structure for A0 support
# 
# Hydra_Software_Devel/1   1/11/07 2:40p jgarrett
# PR27004: Adding new .inc files
# 
############################################################
#
# Conditional Options for this Makefile
#
# BXVD_IMG_ONLY - If defined, only the firmware image related code will be compiled.
# BXVD_IMG_EXTERNAL - If defined, firmware related code (interface AND context) will NOT be compiled. Also defines -DBXVD_USE_CUSTOM_IMAGE.  The app is expected to specify their own firmware interface AND image context when calling BXVD_Open().
# BXVD_USE_CUSTOM_CONTEXT - If defined, the firmware image context will NOT be compiled. The app is expected to provide their own firmware image context when calling BXVD_Open().
# BXVD_USE_DM_V1 - If defined, Display Manager (DM) version 1.x will be used.
# BXVD_POWER_MANAGEMENT - Defaults to 'y' (yes) on DTV platforms, to 'n' (no) used to enable dynamic power management feature  
# BXVD_FW_DEBUG_DRAM_LOGGING - If 'y' (yes), starts AVD FW DRAM logging and reads and prints out the debug log during picture data ready_isr
# BXVD_FLATTEN_USERDATA - If 'y' (yes), the userdata code will coalesce userdata packets otherwise, they will be delivered singly.
# BXVD_BREAK_ON_TYPE_CHANGE - If 'y' (yes), the userdata packet whose UD type is different from other packets in a group will be delivered separately from coalesced packets of the same type. Must be used in conjunction with BXVD_FLATTEN_USERDATA=1
#

# Define this module
MAGNUM_MODULES += BXVD

#
# When true, select the BDBG_INSTANCE_* macros, otherwise use BDBG_*
#
ifeq ($(BXVD_USE_INSTANCE_MACROS),y)
BXVD_DEFINES += BXVD_P_USE_INSTANCE_MACROS
endif

#
# When true, coalesce userdata packets, otherwise they are delivered singly
#
ifeq ($(BXVD_FLATTEN_USERDATA),y)
BXVD_DEFINES += BXVD_FLATTEN_USERDATA
endif

#
# When true, along with BXVD_FLATTEN_USERDATA, a change in userdata type will cause packet coalescing to stop
#
ifeq ($(BXVD_BREAK_ON_TYPE_CHANGE),y)
BXVD_DEFINES += BXVD_BREAK_ON_TYPE_CHANGE
endif

#
# When true, this will cause a live dump of userdata packets. Use with caution as this will have
# a negative effect on packet timing.
#
ifeq ($(BXVD_USERDATA_EXTRA_DEBUG),y)
BXVD_DEFINES += BXVD_USERDATA_EXTRA_DEBUG
endif

# 
# Select XVD CHIP family
#
BXVD_CHIP = $(BCHP_CHIP)

ifeq ($(findstring $(BCHP_CHIP), 7405 7325 7335 3548 3556 7336 7340 7342 7125 7468 7408), $(BCHP_CHIP))
BXVD_CHIP = 7405
endif

ifeq ($(findstring $(BCHP_CHIP), 7601 7635), $(BCHP_CHIP))
BXVD_CHIP = 7601
endif

ifeq ($(findstring $(BCHP_CHIP), 35230 7550 7630 35125), $(BCHP_CHIP))
BXVD_CHIP = revj0
endif 

ifeq ($(findstring $(BCHP_CHIP), 35233, 7135 7231 7358 7344 7346 7360 7422 7425 7435 7429 7552, 7563, 7584, 7640), $(BCHP_CHIP))
BXVD_CHIP = revk0
BXVD_DEFINES += BXVD_P_AVD_ARC600
endif 

ifeq ($(findstring $(BCHP_CHIP), 7445), $(BCHP_CHIP))
BXVD_CHIP = revn0
BXVD_DEFINES += BXVD_P_AVD_ARC600
endif 

# 
# Power management is enabled by default on DTV chips
#
ifeq ($(findstring $(BCHP_CHIP), 3548 3556 35233), $(BCHP_CHIP))
ifneq ($(BXVD_POWER_MANAGEMENT),n)
BXVD_DEFINES += BXVD_P_POWER_MANAGEMENT
endif
endif

# 
# Power management is disabled by default on Settop chips
#
ifeq ($(findstring $(BCHP_CHIP), 7405 7125 7231 7325 7335 7344 7346 7358 7360 7400 7408 7420 7425 7429 7435 7468 7552), $(BCHP_CHIP))
ifeq ($(BXVD_POWER_MANAGEMENT),y)
BXVD_DEFINES += BXVD_P_POWER_MANAGEMENT
endif
endif

# 
# Force the SVD picture buffers to be on MEMC0
#
ifeq ($(BXVD_SVD_USE_MEMC0),y)
BXVD_DEFINES += BXVD_P_SVD_USE_MEMC0
endif

# 
# For MVC, set the pNext on the older DVD platforms.
#
ifeq ($(findstring $(BCHP_CHIP), 7630 7631 7632 7633 7634), $(BCHP_CHIP))
ifneq ($(BXVD_LEGACY_MVC_SUPPORT),n)
BXVD_DEFINES += BXVD_LEGACY_MVC_SUPPORT
endif
endif

#
# Enable FW DRAM Log output to be read and printed in PDR_isr()
#
ifeq ($(BXVD_FW_DEBUG_DRAM_LOGGING),y)
BXVD_DEFINES += BXVD_P_FW_DEBUG_DRAM_LOGGING
endif

#### Use DMv2 by default for all platforms
BXVD_DM_VERSION = 2

ifeq ($(BXVD_USE_DM_V1),)
else
BXVD_DM_VERSION = 1
endif

#### Module Includes ####
BXVD_INCLUDES += $(MAGNUM)/portinginterface/xvd/include

#### Source file(s) ####
ifeq ($(BXVD_IMG_ONLY),)
#### Common file(s) ####
BXVD_SOURCES += \
	$(MAGNUM)/portinginterface/xvd/src/bxvd.c \
	$(MAGNUM)/portinginterface/xvd/src/bxvd_devcmds_priv.c \
	$(MAGNUM)/portinginterface/xvd/src/bxvd_relf.c \
	$(MAGNUM)/portinginterface/xvd/src/bxvd_priv.c \
	$(MAGNUM)/portinginterface/xvd/src/bxvd_reg.c \
	$(MAGNUM)/portinginterface/xvd/src/bxvd_userdata.c \
	$(MAGNUM)/portinginterface/xvd/src/bxvd_pvr.c \
	$(MAGNUM)/portinginterface/xvd/src/bxvd_intr.c \
        $(MAGNUM)/portinginterface/xvd/src/bxvd_memory_priv.c \
        $(MAGNUM)/portinginterface/xvd/src/bxvd_status.c \
        $(MAGNUM)/portinginterface/xvd/src/bxvd_dbg.c

#### Core file(s) ####
BXVD_SOURCES += \
	$(MAGNUM)/portinginterface/xvd/src/bxvd_core_avd_reve0.c

#### Platform specific file(s) ####
ifeq ($(BXVD_DM_VERSION),2)
BXVD_SOURCES += \
        $(MAGNUM)/portinginterface/xvd/src/bxvd_dip.c \
        $(MAGNUM)/portinginterface/xvd/src/bxvd_decoder.c \
        $(MAGNUM)/portinginterface/xvd/src/bxvd_decoder_timer.c \
        $(MAGNUM)/portinginterface/xvd/src/bxvd_decoder_dbg.c
endif

BXVD_SOURCES += \
        $(MAGNUM)/portinginterface/xvd/src/bxvd_platform_$(BXVD_CHIP).c

endif # BXVD_IMG_ONLY

#### Firmware image file(s) ####
ifeq ($(BXVD_IMG_EXTERNAL),)
# Common image file(s)
BXVD_SOURCES +=	\
        $(MAGNUM)/portinginterface/xvd/src/bxvd_image.c

ifeq ($(BXVD_USE_CUSTOM_CONTEXT),)
# Platform image file(s)
BXVD_SOURCES += \
        $(MAGNUM)/portinginterface/xvd/src/bxvd_platform_$(BXVD_CHIP)_image.c
endif # !BXVD_USE_CUSTOM_CONTEXT

else # !BXVD_USE_CUSTOM_IMAGE
BXVD_DEFINES += BXVD_USE_CUSTOM_IMAGE
endif

