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
# $brcm_Workfile: bvdc.inc $
# $brcm_Revision: Hydra_Software_Devel/75 $
# $brcm_Date: 5/10/12 3:50p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /magnum/portinginterface/vdc/bvdc.inc $
# 
# Hydra_Software_Devel/75   5/10/12 3:50p pntruong
# SW7435-98: Renamed 'core' directory to 'common' to be consistent with
# other modules.
#
# Hydra_Software_Devel/74   5/9/12 10:52a syang
# SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
# error fix with GetSurface; use shared gfxsurface to manage gfx surface
#
# Hydra_Software_Devel/73   4/26/12 5:06p pntruong
# SW7435-98: Moved source file/director into src.
#
# Hydra_Software_Devel/72   4/26/12 11:34a tdo
# SW7425-2571: Add VDC API to query BVN module error
#
# Hydra_Software_Devel/71   4/5/12 10:48p pntruong
# SW7435-98: Remove softlinks and flatten out vdc directory structure.
#
# Hydra_Software_Devel/70   2/8/12 10:05a pntruong
# SW7360-3: Initial support.
#
# Hydra_Software_Devel/69   11/23/11 11:32a tdo
# SW7435-9: add support for 7435A0 in VDC
#
# Hydra_Software_Devel/68   11/8/11 1:12p vanessah
# SW7425-1704: factor STG functions out of bvdc_displayhandler_priv.c
#
# Hydra_Software_Devel/67   10/7/11 3:22p pntruong
# SW7429-16: Initial support.
#
# Hydra_Software_Devel/67   10/7/11 3:21p pntruong
# SW7429-16: Initial support.
#
# Hydra_Software_Devel/66   4/19/11 2:06p tqiu
# SWDTV-6626: include DTV tuning parameter path
#
# Hydra_Software_Devel/65   4/13/11 4:59p shyi
# SWDTV-6130: Added hardware cursor support for 35233
#
# Hydra_Software_Devel/64   4/11/11 10:42p pntruong
# SWDTV-6264: Fixed stb mainline build errors.
#
# Hydra_Software_Devel/63   4/11/11 3:01p franli
# SWDTV-6264:General SHADE support for 35330
#
# Hydra_Software_Devel/62   4/7/11 4:37p petlee
# SWDTV-6171: Add B3DC
#
# Hydra_Software_Devel/61   3/31/11 4:12p jasonx
# SWDTV-5876: add init v0be file.
#
# Hydra_Software_Devel/60   3/29/11 4:14p franli
# SWDTV-6196:Initial bringup vdc pi for 35233
#
# Hydra_Software_Devel/59   3/28/11 4:47p franli
# SWDTV-6196:Initial bringup vdc pi for 35233
#
# Hydra_Software_Devel/59   3/28/11 4:43p franli
# SWDTV-6196:Initial bringup vdc pi for 35233
#
# Hydra_Software_Devel/58   1/18/11 1:21p yuxiaz
# SW7552-17: Added 7552 support. Merge from Branch.
#
# Hydra_Software_Devel/SW7552-17/1   12/20/10 7:28p xhuang
# SW7552-17: add 7552 support
#
# Hydra_Software_Devel/57   12/2/10 3:53p shyi
# SW35230-2395: Added hardware cursor support
#
# Hydra_Software_Devel/56   12/2/10 12:25p jhaberf
# SW35230-18: Intergrated changes from the 35125 CFE team
#
# Hydra_Software_Devel/55   11/30/10 3:09p zhang
# SW35125-18: First attempt to compile VDC with 35125 C0 RDB.
#
# Hydra_Software_Devel/54   11/19/10 6:23p darnstein
# SW7231-2: add support for 7344, 7346, 7231.
#
# Hydra_Software_Devel/53   11/11/10 9:38a pntruong
# SW35230-1815: Fixed mainline build error.
#
# Hydra_Software_Devel/52   11/11/10 12:08p mbatchel
# SW35230-1815: Integrate PFA support.
#
# Hydra_Software_Devel/SW35230-1815/1   10/20/10 1:21p tqiu
# SW35230-1815: add PFA support with tuning and custom pq
#
# Hydra_Software_Devel/51   11/10/10 4:37p mbatchel
# SW35230-1927: Expand dirty bits beyond 32.
#
# Hydra_Software_Devel/50   10/21/10 2:30p mbatchel
# SW35230-968: Merge HIST_1 maxRGB support
#
# Hydra_Software_Devel/49   10/20/10 7:21p mbatchel
# SW35230-1647: Add reserved customization hooks.
#
# Hydra_Software_Devel/SW35230-1647/1   10/8/10 7:59p mbatchel
# SW35230-1647: Add reserved customization hooks.
#
# Hydra_Software_Devel/48   10/14/10 12:26p tqiu
# SW35230-1750: remove plm from public build
#
# Hydra_Software_Devel/47   9/14/10 9:11a yuxiaz
# SW7358-4: Added initial VDC support for 7358.
#
# Hydra_Software_Devel/47   9/13/10 4:44p yuxiaz
# SW7358-4: Added initial VDC support for 7358.
#
# Hydra_Software_Devel/46   9/8/10 7:05p kaiz
# SW35230-62: customization callback framework - adding into build
#
# Hydra_Software_Devel/45   8/30/10 4:00p tdo
# SW7422-57, SW7425-13: Add simple vdc support
#
# Hydra_Software_Devel/44   8/13/10 4:34p kaiz
# SW35230-189: Add main MIPS infrastructure and tuning code to support
# embedded backlight MIPS - after initial test
#
# Hydra_Software_Devel/43   7/23/10 1:21p tqiu
# SW35230-167: initial version for 35230 A0 and B0
#
# Hydra_Software_Devel/42   7/22/10 2:00p mbatchel
# SW35230-187: Add xvycc tuning support.
#
# Hydra_Software_Devel/SW35230-187/1   7/21/10 1:10p mbatchel
# SW35230-187: Add XVYCC Tuning support.
#
# Hydra_Software_Devel/41   6/10/10 8:36a mbatchel
# SW35230-266: Merge mcdi back into mad in preparation for 35125/[330].
#
# Hydra_Software_Devel/40   4/22/10 9:41a tqiu
# SW35230-175: add shade code into build
#
# Hydra_Software_Devel/39   4/9/10 4:38p mbatchel
# SW35230-162, SW35230-164: Merge initial vdc tuning framework and HIST_0
# tuning into main.
#
# Hydra_Software_Devel/SW35230-162/1   4/9/10 4:37p mbatchel
# SW35230-162, SW35230-164: Merge initial vdc tuning framework and HIST_0
# tuning into main.
#
# Hydra_Software_Devel/38   3/23/10 4:05p kaiz
# SW35230-114: add new programmable N2L & L2N blocks in 35230
#
# Hydra_Software_Devel/37   3/12/10 1:04p bhilder
# SW35230-72: merge bvdc-Frc subproject-72 to main
#
# Hydra_Software_Devel/36   3/11/10 3:22p paultsai
# SW35230-97: Add PDP source file for 35230
#
# Hydra_Software_Devel/35   1/29/10 5:26p paultsai
# SW35230-19: 35230 VDC Compile
#
# Hydra_Software_Devel/34   11/30/09 3:23p rpan
# SW7468-30: More OSCL support.
#
# Hydra_Software_Devel/33   11/24/09 5:47p yuxiaz
# SW7420-462: Added new tuning functions for Tuning Database.
#
# Hydra_Software_Devel/32   11/19/09 11:52a pntruong
# SW7408-13: Initial check in to get 7408 build.
#
# Hydra_Software_Devel/31   11/10/09 3:07p rpan
# SW7468-20: Initial support for 7468.
#
# Hydra_Software_Devel/30   8/25/09 7:44p albertl
# SW7125-10: Initial 7125 support.
#
# Hydra_Software_Devel/29   6/18/09 3:15p darnstein
# PR53430,PR54876,PR55812: handle all orthogonal VEC chipsets in the same
# fashion.
#
# Hydra_Software_Devel/28   3/16/09 10:59p tdo
# PR45785, PR45789: Merge from MCVP branch
#
# Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:38p syang
# PR 45785, PR 45789: add PI support for new the new module  MCVP
# (MCTF+MCDI)
#
# Hydra_Software_Devel/27   1/12/09 1:01p darnstein
# PR25971: For DCS source files, directory name was incorrect.
#
# Hydra_Software_Devel/26   12/3/08 4:38p rpan
# PR45804: Initial effort for the orthogonal VEC support.
#
# Hydra_Software_Devel/25   10/7/08 9:54a jessem
# PR 47304: Added bvdc_tntalgorithm_priv.c
#
# Hydra_Software_Devel/24   9/17/08 8:42p pntruong
# PR47013: B0: Initial vdc support.
#
# Hydra_Software_Devel/23   7/15/08 3:50p rpan
# PR44641: Clean-up.
#
# Hydra_Software_Devel/22   3/31/08 2:40p jessem
# PR 39335: Added CM3D files.
#
# Hydra_Software_Devel/21   3/19/08 5:45p tdo
# PR39333: Implement HIST core
#
# Hydra_Software_Devel/20   3/10/08 8:27p darnstein
# PR25971: first try at DCS for 7400
#
# Hydra_Software_Devel/19   2/28/08 6:24p hongtaoz
# PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
# fixed non-linear scaling when MAD is on;
#
# Hydra_Software_Devel/18   11/2/07 7:35p albertl
# PR36701:  Added MUD functionality for 7440 C0.
#
# Hydra_Software_Devel/17   10/31/07 11:22a pntruong
# PR34239: Added more explicity check of turn on copy protection build.
#
# Hydra_Software_Devel/16   10/23/07 11:18a yuxiaz
# PR29569, PR36290: Add FGT support on 7405.
#
# Hydra_Software_Devel/15   6/27/07 12:34p tdo
# PR32489: Add support for custom adjust DNR
#
# Hydra_Software_Devel/14   6/13/07 9:52a syang
# PR 31944: put ANR K value table inro seperate file
#
# Hydra_Software_Devel/13   5/31/07 3:28p tdo
# PR28978: C0: Provide API to realize CMS function
#
# Hydra_Software_Devel/12   5/25/07 2:12p tdo
# PR30613: Move dynamic contrast stretch into its own file
#
# Hydra_Software_Devel/11   4/19/07 2:32p tdo
# PR29965: PEP cleanup
#
# Hydra_Software_Devel/10   2/9/07 2:49p pntruong
# PR 27117: Included all chipsets support in mainline.
#
# Hydra_Software_Devel/9   2/8/07 11:29a pntruong
# PR 27117: Updating to use module specific .inc files.
#
# Hydra_Software_Devel/8   2/7/07 11:51p pntruong
# PR 27117: Updating to use module specific .inc files.
#
# Hydra_Software_Devel/7   1/25/07 10:58a yuxiaz
# PR 23202: Added bvdc_rts_priv.c for RTS reconfig in VDC.
#
# Hydra_Software_Devel/6   1/17/07 5:46p jasonh
# PR 27117: Fixed 7400 B0 build.
#
# Hydra_Software_Devel/4   1/17/07 4:55p mward
# PR 27004: Handle 7118 like 7401
#
# Hydra_Software_Devel/3   1/15/07 9:01p pntruong
# PR23222: VDEC - ATSC input support.  Tablelized FE for each formats.
#
# Hydra_Software_Devel/3   1/15/07 9:01p pntruong
# PR23222: VDEC - ATSC input support.  Tablelized FE for each formats.
#
# Hydra_Software_Devel/2   1/12/07 3:07p jgarrett
# PR 27004: Fixing glitch for new chips
#
# Hydra_Software_Devel/1   1/11/07 2:39p jgarrett
# PR27004: Adding new .inc files
#
# Hydra_Software_Devel/16   1/9/07 1:48p maivu
# PR24457: Merge latest 3563 to main branch for 7400
#
# Hydra_Software_Devel/Refsw_Devel_3563/5   1/7/07 9:06a dyzhang
# PR24457: Merge latest 3563 vdc.inc
#
# Hydra_Software_Devel/Refsw_Devel_3563/4   11/17/06 1:54p jerryy
# PR 24457: Added new files for 3563.
#
# Hydra_Software_Devel/Refsw_Devel_3563/3   11/2/06 4:35p pntruong
# PR 24457: does not like the extension.
#
# Hydra_Software_Devel/Refsw_Devel_3563/2   11/2/06 2:54p pntruong
# PR 24457: Missing back slash.
#
# Hydra_Software_Devel/Refsw_Devel_3563/1   11/2/06 10:04a pntruong
# PR 24457: Added new files for 3563.
#
#############################################################################
#
# Conditional Options for this Makefile
#
# BVDC_MACROVISION - If set to 'y', macrovision support will be added.
# BVDC_DCS - If set to 'y', DCS support will be added.
#

##
# Chip specific selections!!!
#
ifeq ($(filter $(BCHP_CHIP), 7325 7335 7336 7400 7405), $(BCHP_CHIP))
	BVDC_VEC_DIR = modularvec
else
	BVDC_VEC_DIR = orthogonalvec
endif

##
# Define this module
#
MAGNUM_MODULES += BVDC

##
# Module Includes
#
BVDC_INCLUDES = \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR) \
	$(MAGNUM)/portinginterface/vdc/src/common \
	$(MAGNUM)/portinginterface/vdc/include
##
# Module Sources, core file selection
#
BVDC_SOURCES = \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_656in_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_anrtable_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_anr_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_boxdetect_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_bufferheap_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_buffer_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_capture_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_ccbtable_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_ccb_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_chromacoeff_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_cm3d.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_cm3d_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_coeff_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_compositor.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_compositor_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_csc_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_dbg.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_bvnerrtable_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_displayfmt_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_displaymatrices_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_displaytable_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_dnrtable_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_dnr_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_downsample_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_feeder_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_fgttable_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_fgt_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_gfxfeeder.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_gfxfeedertable_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_gfxfeeder_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_gfxsurface_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_hddvi_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_heap.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_hist_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_hscaler_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_mad_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_mcdi_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_mcvp_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_mud_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_oscl_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_pepalgorithm_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_pepcmstable_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_pepcms_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_pepdyncontrast_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_pep_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_resource_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_rts_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_scaler_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_source.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_source_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_subrul_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_test.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_tntalgorithm_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_tnt_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_tune.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_tune_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_vdecfmt_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_vdec_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_videotable_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_vnet_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_window.c \
	$(MAGNUM)/portinginterface/vdc/src/common/bvdc_window_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_display.c \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_display_priv.c

##
# Copy protection files selection
#
ifeq ($(BVDC_MACROVISION),y)
BVDC_SOURCES += \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_macrovision.c \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_macrovision_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_macrovisiontable_priv.c
else
ifeq ($(BVDC_DCS),y)
BVDC_SOURCES += \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_dcs.c \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_dcs_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_dcstable_priv.c
else
BVDC_SOURCES += \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_nomacrovision_priv.c
endif
endif

##
# Vec files selection
#
ifeq ($(BVDC_VEC_DIR),orthogonalvec)
BVDC_SOURCES += \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_displaystg_priv.c \
	$(MAGNUM)/portinginterface/vdc/src/$(BVDC_VEC_DIR)/bvdc_displayhandler_priv.c
endif

##
# End of file!
#
