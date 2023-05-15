/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_feeder_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/109 $
 * $brcm_Date: 8/28/12 4:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_feeder_priv.h $
 * 
 * Hydra_Software_Devel/109   8/28/12 4:50p yuxiaz
 * SW7360-26: MFD_0_DATA_MODE is added with 3D.
 * 
 * Hydra_Software_Devel/108   8/17/12 2:06p tdo
 * SW7445-8: Fix window timeout error due to MFD post mux value calculated
 * wrong
 *
 * Hydra_Software_Devel/107   8/7/12 10:34a vanessah
 * SW7563-9: MRE3D disable for 7563
 *
 * Hydra_Software_Devel/106   7/31/12 4:55p yuxiaz
 * SW7425-1835: Use RDC slot timestamp for capture and playback timestamp
 * in multi-buffering algo.
 *
 * Hydra_Software_Devel/105   6/15/12 10:34a vanessah
 * SW7435-237: reset vnet channel connections
 *
 * Hydra_Software_Devel/104   6/14/12 3:42p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/103   5/23/12 6:46p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 *
 * Hydra_Software_Devel/102   5/9/12 10:53a syang
 * SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
 * error fix with GetSurface; use shared gfxsurface to manage gfx surface
 *
 * Hydra_Software_Devel/101   3/14/12 4:22p pntruong
 * HW7435-347: Compiled out work-around for SW7231-635.
 *
 * Hydra_Software_Devel/100   9/28/11 12:15p yuxiaz
 * SW7425-985: Fix artifacts at bottom of screen on new feeder.
 *
 * Hydra_Software_Devel/99   9/23/11 3:18p yuxiaz
 * SW7425-985: Added flag BVDC_P_MFD_ENABLE_SKIP_FIRST_LINE to enable
 * SKIP_FIRST_LINE.
 *
 * Hydra_Software_Devel/98   9/8/11 10:22a yuxiaz
 * SW7552-100: Fixed compile error on 7552B0.
 *
 * Hydra_Software_Devel/97   9/2/11 2:52p yuxiaz
 * SW7425-985, SW7552-100: Bring up new MFD on emulator.
 *
 * Hydra_Software_Devel/96   9/1/11 9:48p yuxiaz
 * SW7425-985, SW7425-967: Added MRE support in MFD.
 *
 * Hydra_Software_Devel/95   9/1/11 2:23p yuxiaz
 * SW7425-985, SW7552-100: Bring up new MFD on emulator.
 *
 * Hydra_Software_Devel/94   8/28/11 2:16p yuxiaz
 * SW7425-985, SW7552-100: Merge into mainline.Clean up feeder.
 *
 * Hydra_Software_Devel/SW7425-985/4   8/26/11 4:02p yuxiaz
 * SW7425-985, SW7552-100: Fixed the macro.
 *
 * Hydra_Software_Devel/SW7425-985/3   8/26/11 3:00p yuxiaz
 * SW7425-985, SW7552-100: Added support for new MFD.
 *
 * Hydra_Software_Devel/SW7425-985/2   8/25/11 3:40p yuxiaz
 * SW7425-985, SW7552-100: More clean up for feeder.
 *
 * Hydra_Software_Devel/SW7425-985/1   8/23/11 4:00p yuxiaz
 * SW7425-985, SW7552-100: Clean up for new MFD design.
 *
 * Hydra_Software_Devel/93   8/11/11 3:59p pntruong
 * SW7231-318: Initial vdc support with error free build.
 *
 * Hydra_Software_Devel/92   8/5/11 2:05p yuxiaz
 * SW7425-985: Get 7425 B0 compiled.
 *
 * Hydra_Software_Devel/91   7/12/11 3:28p yuxiaz
 * SW7358-40: Added second CRC support in VDC.
 *
 * Hydra_Software_Devel/90   4/27/11 11:38a yuxiaz
 * SW7358-4: Updated VDC flags.
 *
 * Hydra_Software_Devel/89   1/4/11 4:07p yuxiaz
 * SW7422-174: Refactor DCX macros.
 *
 * Hydra_Software_Devel/88   11/12/10 5:06p hongtaoz
 * SW7422-39: VNET_F assign MFD2 to special crossbar value;
 *
 * Hydra_Software_Devel/87   11/1/10 4:24p jessem
 * SW7422-82: Added stereo 3D support to VFD as source feature.
 *
 * Hydra_Software_Devel/86   11/1/10 1:12p vanessah
 * SW7422-43:  merge 2D into 3D with a unified interface, change input
 * data structure as const
 *
 * Hydra_Software_Devel/85   10/29/10 5:06p vanessah
 * SW7422-43:  3D graphics render,merge 2D into 3D with a unified
 * interface
 *
 * Hydra_Software_Devel/84   10/25/10 7:18p vanessah
 * SW7422-43:  Handle GFD programming for 3D.
 *
 * Hydra_Software_Devel/83   10/13/10 1:52p yuxiaz
 * SW7422-48: Add SCB Client select for MFD.
 *
 * Hydra_Software_Devel/82   10/12/10 5:30p hongtaoz
 * SW7425-30, SW7425-33: added 4th CMP and 3rd MFD support for 7425;
 *
 * Hydra_Software_Devel/81   10/11/10 12:26p jessem
 * SW7420-173: Added support for using VFD as  a  source.
 *
 * Hydra_Software_Devel/80   9/29/10 5:22p yuxiaz
 * SW7422-39: Added 3D video support in CAP and VFD.
 *
 * Hydra_Software_Devel/79   9/1/10 1:22p yuxiaz
 * SW7422-39: Clean up register offset in MFD and VFD.
 *
 * Hydra_Software_Devel/78   8/31/10 5:00p yuxiaz
 * SW7422-39: Clean up feeder programming.
 *
 * Hydra_Software_Devel/77   8/26/10 5:26p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/76   4/7/10 11:29a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/75   4/5/10 4:05p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/74   12/1/09 3:54p yuxiaz
 * CR7340-133: BVN MFD Move the pixel saturation control from the test
 * mode register to the feeder control register.
 *
 * Hydra_Software_Devel/73   12/5/08 11:16a yuxiaz
 * PR45800: Enable Dering Filter for CMP_0 Video Surfaces on 7420.
 *
 * Hydra_Software_Devel/72   12/4/08 3:10p yuxiaz
 * PR45815: Added MFD support on 7420.
 *
 * Hydra_Software_Devel/71   10/23/08 9:40p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/70   10/23/08 11:13a yuxiaz
 * PR46305: Reset VFd during mode switch.
 *
 * Hydra_Software_Devel/69   10/3/08 11:00a yuxiaz
 * PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
 * Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
 * BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
 *
 * Hydra_Software_Devel/68   10/3/08 9:24a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/67   9/26/08 3:24p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/66   9/15/08 3:12p yuxiaz
 * PR46495: Resolve register re-arrangment for MFD/VFD on 3548 B0.
 *
 * Hydra_Software_Devel/65   9/12/08 4:22p yuxiaz
 * PR46497: Fix CRC register processing in MFD for 3548 B0.
 *
 * Hydra_Software_Devel/64   6/3/08 2:53p yuxiaz
 * PR43247: MFD: CRC Status Register Should be Updated on the "Go" Bit.
 *
 * Hydra_Software_Devel/63   2/22/08 2:32p yuxiaz
 * PR39155: Added de-ringing filters in BVN.
 *
 * Hydra_Software_Devel/62   2/19/08 3:45p yuxiaz
 * PR39251: Added new packed format for MFD and VDC on 3548.
 *
 * Hydra_Software_Devel/61   2/15/08 2:33p yuxiaz
 * PR34712, PR39251: Use feeder version to combine all the feeder flags.
 * Regroup feeder support based on feeder version.
 *
 * Hydra_Software_Devel/60   5/31/07 1:43p yuxiaz
 * PR30015: Added support in VDC to accept strip width as input parameter
 * from XVD.
 *
 * Hydra_Software_Devel/59   5/7/07 6:03p pntruong
 * PR28899: C0: BVN: VFD 10-bit 444 to 422 Mode Transition Requires Soft
 * Reset.
 *
 * Hydra_Software_Devel/58   4/19/07 1:47p yuxiaz
 * PR 26052: Add CSC support in MFD on 7400 B0.
 *
 * Hydra_Software_Devel/57   4/2/07 11:28a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/56   2/23/07 6:29p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/55   1/24/07 11:07p pntruong
 * PR22237: Fixed previous checked in build errors.
 *
 * Hydra_Software_Devel/54   1/24/07 9:06p albertl
 * PR22237:  Updated BMEM calls to use new BMEM_Heap functions.
 *
 * Hydra_Software_Devel/52   1/11/07 7:02p syang
 * PR 22569: 1st time check in after anr building vnet correctly and not
 * hang the sys. But display is still static
 *
 * Hydra_Software_Devel/51   1/10/07 10:59a syang
 * PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
 * dnr, mfd, vfd, cap, ...
 *
 * Hydra_Software_Devel/50   12/18/06 11:19p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 12:55p pntruong
 * PR23222, PR23225:  Defer window and source heap assignments until user
 * create.
 *
 * Hydra_Software_Devel/49   8/28/06 1:45p yuxiaz
 * PR20875: Renamed BVDC_P_SUPPORT_10BIT_BVN to
 * BVDC_P_SUPPORT_BVN_10BIT_444. Removed BVDC_P_BVNDataMode, use window
 * and buffer node's pixel format. Integrated with new 10 bit pixel
 * formats.
 *
 * Hydra_Software_Devel/48   8/21/06 3:31p yuxiaz
 * PR20875: Added 10bit BVN support for capture and feeder.
 *
 * Hydra_Software_Devel/47   7/7/06 4:14p yuxiaz
 * PR 22296, PR 22108, PR22109: Removed
 * BVDC_Window_InstallLipSyncDelayCallback, added new generic window
 * callback function BVDC_Window_InstallCallback for lip sync.
 *
 * Hydra_Software_Devel/46   5/26/06 3:39p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/45   5/15/06 12:24p jessem
 * PR 17530: Added instruction to feeder RUL such that the timer register
 * is read during RUL execution time instead of using BTMR_ReadTimer_isr.
 * Added BTMR_TimerRegister field to BVDC_P_Feeder_Context.
 *
 * Hydra_Software_Devel/44   5/10/06 3:23p jessem
 * PR 17530: Added BTMR support. BVDC_P_Feeder_BuildRul_isr stores
 * timestamp for use by BVDC_P_Window_Reader_isr. BVDC_P_Feeder_Create
 * takes in a new parameter: BTMR_TimerHandle.
 *
 * Hydra_Software_Devel/43   4/6/06 1:56p pntruong
 * PR20642: Optimized VDC codes (refactoring).
 *
 * Hydra_Software_Devel/42   3/22/06 5:23p jessem
 * PR 19517: Reverted to old previous function signatures of
 * BVDC_P_Feeder_SetVertWindow_isr, BVDC_P_Feeder_SetMpegInfo_isr, and
 * BVDC_P_Feeder_SetPlaybackInfo_isr by removing the BFMT_VideoFmt
 * parameter. The check for the source height against the display format
 * is now made at apply changes, and during connection and disconnection
 * of source from the window it is tied to.
 *
 * Hydra_Software_Devel/41   3/21/06 2:47p jessem
 * PR 19517: Added BFMT_VideoFmt parameter to
 * BVDC_P_Feeder_SetVertWindow_isr, BVDC_P_Feeder_SetPlaybackInfo_isr,
 * and BVDC_P_Feeder_SetMpegInfo_isr. The parameter is needed to check
 * whether the ulHeight parameter in BVDC_P_Feeder_SetVertWindow_isr
 * exceeds 1080 for non-1080p display formats.
 *
 * Hydra_Software_Devel/40   1/17/06 4:18p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/39   1/6/06 1:31p pntruong
 * PR18907: Unable to change mute color when already muted.  Also removed
 * not need state in picture.
 *
 * Hydra_Software_Devel/38   11/29/05 3:30p pntruong
 * PR18320, PR17788: Add support for BVDC_MuteMode_eConst.  Allow
 * deinterlaced for down scale window, and reset mad when drain vnet.
 *
 * Hydra_Software_Devel/37   10/6/05 6:29p hongtaoz
 * PR12061: added CRC callback support for MFD;
 *
 * Hydra_Software_Devel/36   8/18/05 1:15p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/17/05 12:43p pntruong
 * PR12519: Take in additional support for 7401.
 *
 * Hydra_Software_Devel/35   7/29/05 10:07a yuxiaz
 * PR16182: Added pan scan support for 7401.
 *
 * Hydra_Software_Devel/34   7/12/05 9:55a yuxiaz
 * PR16182: Added initial support for new AVC/MPEG feeder for 7401.
 *
 * Hydra_Software_Devel/33   3/17/05 6:36p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/32   3/9/05 4:50p pntruong
 * PR14112, PR14046: Support out of order destroys for "cloned" windows in
 * VDC. and While in mirror mode with PIP, VDC times out when toggling
 * between analog and digital channel.
 *
 * Hydra_Software_Devel/31   11/16/04 8:01p pntruong
 * PR13076: Video jitter under heavy system load.  Added some
 * optimizations, additional work needed.
 *
 * Hydra_Software_Devel/30   7/30/04 5:54p pntruong
 * PR10752: channel change will sometimes cause video output to disappear
 * forever.
 *
 ***************************************************************************/
#ifndef BVDC_FEEDER_PRIV_H__
#define BVDC_FEEDER_PRIV_H__

#include "bchp_common.h"
#include "bvdc_common_priv.h"
#include "bchp_mfd_0.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_gfxsurface_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Private defines
 ***************************************************************************/
#if 0
/* The following are legacy chips */
/* 3563:
 * MPEG feeder + BVDC_P_SUPPORT_BVN_10BIT_444. No AVC support, */
#define BVDC_P_MFD_VER_0                            (0)

/* 7401A, 7401B, 7440:
 * Basic AVC feeder*/
#define BVDC_P_MFD_VER_1                            (1)

/* 7401C and above, 7403, 7118:
 * AVC feeder + MFD_0_RANGE_EXP_REMAP_CNTL support */
#define BVDC_P_MFD_VER_2                            (2)
#endif

/* 7400:
 * AVC feeder + MFD_0_RANGE_EXP_REMAP_CNTL + MFD_0_CSC_CNTL +
 * MFD_0_DS_CNTL support */
#define BVDC_P_MFD_VER_3                            (3)

/* 7405:
 * AVC feeder + MFD_0_RANGE_EXP_REMAP_CNTL + MFD_0_CSC_CNTL +
 * MFD_0_DS_CNTL + MFD_0_LAC_CNTL.STRIPE_WIDTH_SEL support */
#define BVDC_P_MFD_VER_4                            (4)

/* 7325, 7335:
 * AVC feeder + MFD_0_RANGE_EXP_REMAP_CNTL +
 * MFD_0_CSC_CNTL (new BVDC_P_SUPPORT_CSC_MAT_COEF_VER) +
 * MFD_0_LAC_CNTL.STRIPE_WIDTH_SEL support */
#define BVDC_P_MFD_VER_5                            (5)

/* 3548 A0:
 * AVC feeder + MFD_0_RANGE_EXP_REMAP_CNTL +
 * BVDC_P_SUPPORT_BVN_10BIT_444 +
 * MFD_0_LAC_CNTL.STRIPE_WIDTH_SEL support +
 * BVDC_P_Feeder_ImageFormat_ePacked_new +
 * MFD_0_BYTE_ORDER
 */
#define BVDC_P_MFD_VER_6                            (6)

/* 3548 B0:
 * AVC feeder + MFD_0_RANGE_EXP_REMAP_CNTL +
 * BVDC_P_SUPPORT_BVN_10BIT_444 +
 * MFD_0_LAC_CNTL.STRIPE_WIDTH_SEL support +
 * BVDC_P_Feeder_ImageFormat_ePacked_new +
 * MFD_0_BYTE_ORDER +
 * CRC Status Register updated on the "Go" Bit
 */
#define BVDC_P_MFD_VER_7                            (7)

/* 7420 A0:
 * AVC feeder + MFD_0_RANGE_EXP_REMAP_CNTL +
 * MFD_0_LAC_CNTL.STRIPE_WIDTH_SEL support +
 * BVDC_P_Feeder_ImageFormat_ePacked_new +
 * MFD_0_BYTE_ORDER +
 * CRC Status Register updated on the "Go" Bit
 */
#define BVDC_P_MFD_VER_8                            (8)

/* 7340, 7342, 7550, 7125, 7468, 7408:
 * AVC feeder + MFD_0_RANGE_EXP_REMAP_CNTL +
 * MFD_0_LAC_CNTL.STRIPE_WIDTH_SEL support +
 * BVDC_P_Feeder_ImageFormat_ePacked_new +
 * MFD_0_BYTE_ORDER +
 * CRC Status Register updated on the "Go" Bit +
 * MFD_0_FEEDER_CNTL.PIXEL_SATURATION_ENABLE
 */
#define BVDC_P_MFD_VER_9                            (9)

/* 7422, 7425Ax, 7231Ax, 7346Ax, 7344Ax:
 * 3D support + Replace MFD_0_PICTURE0_DISP_VERT_WINDOW
 * with MFD_0_DISP_VSIZE + MFD_0_DATA_MODE
 */
#define BVDC_P_MFD_VER_10                           (10)

/* 7358Ax, 7552Ax,
 * Add MFD_0_LUMA_CRC_R, MFD_0_CHROMA_CRC_R
 */
#define BVDC_P_MFD_VER_11                           (11)

/* 7552Bx,
 * New MFD design (MVFD):
 * Remove:
 *  - MFD_0_FEEDER_CNTL.PACKING_TYPE, PACKED_NEW format
 * Add:
 *  - INITIAL_PHASE, LINE_REPEAT, MFD_0_US_422_TO_444_DERING
 */
#define BVDC_P_MFD_VER_12                           (12)

/* 7425 Bx, 7231Bx, 7346Bx, 7344Bx
 * New MFD design (MVFD):
 * Add MRE3D support:
 */
#define BVDC_P_MFD_VER_13                           (13)

/* 7435Ax+, 7429Bx+, 7584Ax+
 * Add BVDC_P_MVFD_ALIGNMENT_WORKAROUND fix.
 */
#define BVDC_P_MFD_VER_14                           (14)

/* New MFD design (MVFD) */
#define BVDC_P_MFD_FEEDER_IS_MVFD                    \
	(BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_12)

/* MFD_0_FEEDER_CNTL */
#define BVDC_P_MFD_SUPPORT_IMAGE_FORMAT_PACKED_NEW   \
	((BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_6) && \
	 (BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_11))

#define BVDC_P_MFD_SUPPORT_PIXEL_SATURATION_ENABLE   \
	(BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_9)

#define BVDC_P_MFD_SUPPORT_PACKING_TYPE   \
	(BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_11)

#define BVDC_P_MFD_SUPPORT_SMOOTHING_BUFFER   \
	((BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_10) || \
	 (BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_11))

#define BVDC_P_MFD_SUPPORT_SCB_CLIENT_SEL   \
	(BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_10)

#define BVDC_P_MFD_SUPPORT_INIT_PHASE   \
	(BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_12)

#define BVDC_P_MFD_SUPPORT_SKIP_FIRST_LINE   \
	(BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_12)


#define BVDC_P_MFD_SUPPORT_3D_VIDEO   \
	 (BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_10)

#ifdef BCHP_MFD_0_MRE3D_SYN_FILT_CTRL
#define BVDC_P_MFD_SUPPORT_3D_VIDEO_MRE       (1)
#else
#define BVDC_P_MFD_SUPPORT_3D_VIDEO_MRE       (0)
#endif

/* VFD_0_FEEDER_CNTL */
#define BVDC_P_VFD_SUPPORT_IMAGE_FORMAT   \
	(BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_6)

/* MFD_0_LAC_CNTL */
#define BVDC_P_MFD_SUPPORT_STRIPE_WIDTH_SEL  \
	 (BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_4)

#define BVDC_P_MFD_SUPPORT_CHROMA_VERT_POSITION   \
	(BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_11)


#define BVDC_P_MFD_SUPPORT_CSC   \
	((BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_3) || \
	 (BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_4) || \
	 (BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_5))

#define BVDC_P_MFD_SUPPORT_NEW_CSC   \
	 (BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_5)

#define BVDC_P_MFD_SUPPORT_10BIT_444   \
	((BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_6) && \
	 (BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_7))

#define BVDC_P_MFD_SUPPORT_BYTE_ORDER   \
	(BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_6)

#define BVDC_P_MFD_SUPPORT_DERINGING    \
	(BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_6)

#define BVDC_P_MFD_SUPPORT_DATA_MODE    \
	((BVDC_P_MFD_SUPPORT_10BIT_444)      || \
	(BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_10))

/* The CRC Status register had some problems when it swapped on end of picture.
 * The issue is that if an interrupt fires but isn't handled until after the
 * picture is finished (very small feed) then the CRC might be wrong.
 * The workaround is to use a scratch register to read the value in RUL.
 * This is fixed in BVDC_P_MFD_VER_7, the update now is to flip based on
 * the "Go" bit for the feeder. */
#define BVDC_P_MFD_NEED_CRC_WORKAROUND               \
	 (BVDC_P_SUPPORT_MFD_VER <= BVDC_P_MFD_VER_6)

#define BVDC_P_MFD_SUPPORT_CRC_R      \
	 (BVDC_P_SUPPORT_MFD_VER >= BVDC_P_MFD_VER_11)

#define BVDC_P_MFD_INVALID_CRC         (0xffffffff)

	/* syang */
#if 1
#define  BVDC_P_FEEDER_SUR_NO_MATCH       (0)
#define  BVDC_P_FEEDER_SUR_NODE_MASK      (0x7fffffff)
#define  BVDC_P_FEEDER_SUR_OF_CURR_NODE   (1<<0)   /* match the rec in cur node */
#define  BVDC_P_FEEDER_SUR_OF_NEW_SET     (1<<31)  /* match the rec in pNewSet */
#define BVDC_P_FEEDER_NUM_SUR_NODES       2
#endif

/***************************************************************************
 * Private macros
 ***************************************************************************/

#define BVDC_P_MFD_GET_REG_IDX(reg) \
	((BCHP##_##reg - BCHP_MFD_0_REG_START) / sizeof(uint32_t))

/* Get register data */
#define BVDC_P_MFD_GET_REG_DATA(reg) \
	(hFeeder->aulRegs[BVDC_P_MFD_GET_REG_IDX(reg)])
#define BVDC_P_MFD_SET_REG_DATA(reg, data) \
	(BVDC_P_MFD_GET_REG_DATA(reg) = (uint32_t)(data))

/* Get field */
#define BVDC_P_MFD_GET_FIELD_NAME(reg, field) \
	((BVDC_P_MFD_GET_REG_DATA(reg) & BCHP##_##reg##_##field##_MASK) >> \
	BCHP##_##reg##_##field##_SHIFT)

/* Compare field */
#define BVDC_P_MFD_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_MFD_GET_FIELD_NAME(reg, field)==(data))

#define BVDC_P_MFD_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_MFD_GET_FIELD_NAME(reg, field)==BCHP##_##reg##_##field##_##name)

#define BVDC_P_MFD_WRITE_TO_RUL(reg, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BCHP##_##reg + hFeeder->ulRegOffset); \
	*addr_ptr++ = BVDC_P_MFD_GET_REG_DATA(reg); \
}

/* This macro does a block write into RUL */
#define BVDC_P_MFD_BLOCK_WRITE_TO_RUL(from, to, pulCurrent) do { \
	uint32_t ulBlockSize = \
		BVDC_P_REGS_ENTRIES(from, to);\
	*pulCurrent++ = BRDC_OP_IMMS_TO_REGS( ulBlockSize ); \
	*pulCurrent++ = BRDC_REGISTER(BCHP##_##from + hFeeder->ulRegOffset); \
	BKNI_Memcpy((void*)pulCurrent, \
		(void*)&(hFeeder->aulRegs[BVDC_P_MFD_GET_REG_IDX(from)]), \
		ulBlockSize * sizeof(uint32_t)); \
	pulCurrent += ulBlockSize; \
} while (0)

/* check parameters */
#define BVDC_P_MFD_REGS_COUNT    \
	BVDC_P_REGS_ENTRIES(MFD_0_REG_START, MFD_0_REG_END)

#define BVDC_P_Feeder_IS_MPEG(hFeeder) \
	((BVDC_P_FeederId_eVfd0 > (hFeeder->eId)) ? true : false)

#define BVDC_P_Feeder_IS_VFD(hFeeder) \
	((BVDC_P_FeederId_eVfd0<=(hFeeder->eId)) ? true : false)

/* Gets macroblock pan scan. The result pan scan is in the uint
 * of macroblock.
 * This is related to frame buffer organization in MVD. */
#define BVDC_P_Feeder_GET_MB_PANSCAN(PanScan) \
	( PanScan >> BVDC_P_16TH_PIXEL_SHIFT )

/* Gets pixel pan scan. The result pan scan is in the uint
 * of pixel.
 * This is related to frame buffer organization in MVD. */
#define BVDC_P_Feeder_GET_PIXEL_PANSCAN(PanScan) \
	( PanScan & 0xf )

/* Combines Macroblock pan scan and pixel pan scan together.
 * This is related to frame buffer organization in MVD. */
#define BVDC_P_Feeder_GET_COMBINE_PANSCAN(PanScan) \
	( (BVDC_P_Feeder_GET_MB_PANSCAN(PanScan)<< BVDC_P_16TH_PIXEL_SHIFT) + \
	BVDC_P_Feeder_GET_PIXEL_PANSCAN(PanScan) )

#define BVDC_P_FEEDER_IMAGE_FORMAT_IS_MPEG(eImageFormat) \
	((eImageFormat) == BVDC_P_Feeder_ImageFormat_eAVC_MPEG)

#define BVDC_P_FEEDER_IMAGE_FORMAT_IS_PACKED(eImageFormat) \
	(((eImageFormat) == BVDC_P_Feeder_ImageFormat_ePacked) ||  \
	 ((eImageFormat) == BVDC_P_Feeder_ImageFormat_ePacked_new))

#define BVDC_P_FEEDER_IMAGE_FORMAT_IS_YUV444(eImageFormat) \
	((eImageFormat) == BVDC_P_Feeder_ImageFormat_eYUV444)

#define BVDC_P_FEEDER_PIXEL_FORMAT_IS_10BIT(ePxlFmt) \
	(((ePxlFmt) == BVDC_P_CAP_PIXEL_FORMAT_10BIT444) ||  \
	 ((ePxlFmt) == BVDC_P_CAP_PIXEL_FORMAT_10BIT422))


/* NOTE: weirdness of VNET_F crossbar puts MFD2 in non-contiguous value */
#ifdef BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_2
#define BVDC_P_Feeder_PostMuxValue(hFeeder)  \
    ((BVDC_P_FeederId_eMfd2 == (hFeeder)->eId)? (BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_2) :  \
	 (BVDC_P_FeederId_eMfd2 > (hFeeder)->eId)? (BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_0 + (hFeeder)->eId) :  \
	 (BVDC_P_FeederId_eVfd0 > (hFeeder)->eId)? (BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_0 + (hFeeder)->eId) :  \
	 (BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_0 + (hFeeder)->eId - BVDC_P_FeederId_eVfd0))
#else
#define BVDC_P_Feeder_PostMuxValue(hFeeder)  \
    ((BVDC_P_FeederId_eMfd2 > (hFeeder)->eId)? (BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_0 + (hFeeder)->eId) :  \
	 (BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_0 + (hFeeder)->eId - BVDC_P_FeederId_eVfd0))
#endif

#define BVDC_P_feeder_SetRulBuildWinId_isr(hFeeder, eWinId) \
    BVDC_P_SubRul_SetRulBuildWinId_isr(&((hFeeder)->SubRul), eWinId)

/***************************************************************************
 * MPEG/Video feeder private data structures
 ***************************************************************************/
/* Position of chroma pixel component corresponding to luma
 * pixel component in vertical direction */
typedef enum BVDC_P_Feeder_ChromaVertPos
{
	BVDC_P_Feeder_ChromaVertPos_eColorcatedWithLuma = 0,
	BVDC_P_Feeder_ChromaVertPos_eHalfPixelGridBetweenLuma

} BVDC_P_Feeder_ChromaVertPos;


/* Image format */
typedef enum BVDC_P_Feeder_ImageFormat
{
#if (BVDC_P_SUPPORT_MFD_VER == BVDC_P_MFD_VER_6)
	BVDC_P_Feeder_ImageFormat_ePacked = 0,
	BVDC_P_Feeder_ImageFormat_eYUV444,
	BVDC_P_Feeder_ImageFormat_eAVC_MPEG,
#else
	BVDC_P_Feeder_ImageFormat_eAVC_MPEG = 0,
	BVDC_P_Feeder_ImageFormat_ePacked,
	BVDC_P_Feeder_ImageFormat_eYUV444,
#endif

	BVDC_P_Feeder_ImageFormat_ePacked_new,

	BVDC_P_Feeder_ImageFormat_eUnknown

} BVDC_P_Feeder_ImageFormat;

typedef enum BVDC_P_Feeder_VideoFormatMode
{
	BVDC_P_Feeder_VideoFormatMode_e2D = 0,
	BVDC_P_Feeder_VideoFormatMode_e3DLR,
	BVDC_P_Feeder_VideoFormatMode_e3DOU,
	BVDC_P_Feeder_VideoFormatMode_e3DDP,
	BVDC_P_Feeder_VideoFormatMode_e3DMR

} BVDC_P_Feeder_VideoFormatMode;

/* Stripe width settings. */
typedef struct
{
	BAVC_StripeWidth               eStripeWidth;
	uint32_t                       ulStripeWidth;
	uint32_t                       ulShift;

} BVDC_P_Feeder_StripeWidthConfig;

/* Device address settings. */
typedef struct
{
	uint32_t        ulLumaDeviceAddr;
	uint32_t        ulChromaDeviceAddr;

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO)
	uint32_t        ulLumaDeviceAddr_R;
	uint32_t        ulChromaDeviceAddr_R;

#if (BVDC_P_MFD_SUPPORT_3D_VIDEO_MRE)
	uint32_t        ulLumaDeviceAddr_hp;
	uint32_t        ulChromaDeviceAddr_hp;
	uint32_t        ulLumaDeviceAddr_R_hp;
	uint32_t        ulChromaDeviceAddr_R_hp;
#endif

#endif


} BVDC_P_Feeder_MpegDeviceAddrConfig;

typedef struct BVDC_P_FeederContext
{
	BDBG_OBJECT(BVDC_FDR)

	BVDC_Source_Handle             hSource;
	BMEM_Heap_Handle               hMem;
	bool                           bInitial;
	uint32_t                       ulResetMask;
	uint32_t                       ulResetRegAddr;
	uint32_t                       ulVnetResetAddr;
	uint32_t                       ulVnetResetMask;

	BVDC_P_FeederId                eId;
	/* This is the regular offset from BCHP_MFD_0_REG_START */
	uint32_t                       ulRegOffset;
	/* This is the offset from BCHP_VFD_0_REG_START, apply to VFD_x only.
	 * Used for registers in VFD not in MFD, or registers have different
	 * offset in MFD and VFD
	 */
	uint32_t                       ulVfd0RegOffset;
	uint32_t                       aulRegs[BVDC_P_MFD_REGS_COUNT];

	BPXL_Format                    ePxlFormat;
	BVDC_P_Feeder_ImageFormat      eImageFormat;

	BFMT_Orientation               eInputOrientation;
	BFMT_Orientation               eOutputOrientation;

	/* A register handle. */
	BRDC_Handle                    hRdc;
	BREG_Handle                    hRegister;

	/* Keeps track of when ISR was executed */
	BTMR_TimerHandle               hTimer;
	BTMR_TimerRegisters            stTimerReg;
	uint32_t                       ulTimestamp;

#if (!BVDC_P_USE_RDC_TIMESTAMP)
	uint32_t                       ulTimestampRegAddr;
#endif

#if (BVDC_P_MFD_NEED_CRC_WORKAROUND)
	uint32_t                       ulLumaCrcRegAddr;
	uint32_t                       ulChromaCrcRegAddr;
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
	/* Save the TestFeature1 settings for VFD */
	BVDC_P_TestFeature1_Settings   stTestFeature1;
#endif

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext           SubRul;

	/* 422 to 444 up sampler setting */
	BVDC_422To444UpSampler         stUpSampler;

	/* Window associated with this vfd.
	 * note: one vfd can not be shared by more than one window */
	BVDC_Window_Handle             hWindow;

	/* gfx surface manager :
	 * For Video Feeder cores with a minor version earlier than 0x50, the
	 * surface for the video source must have a pixel format of either
	 * BPXL_eY18_Cr8_Y08_Cb8, BPXL_eCr8_Y18_Cb8_Y08, BPXL_eY18_Cb8_Y08_Cr8,
	 * or BPXL_eCb8_Y18_Cr8_Y08; otherwise pink and green video will result.
	 * Cores with minor version 0x50 and later can support all formats.
	 */
	BVDC_P_GfxSurfaceContext       stGfxSurface;
	BVDC_P_SurfaceInfo            *pNewSur;

	bool                           bGfxSrc;
	bool                           b3dSrc;

	/* Used only when VFD is used to display graphics surface */
	BVDC_P_PictureNode             stPicture;

	/* For MFD only */
	uint32_t                       ulThroughput;
	bool                           bSkipFirstLine;
	uint32_t                       ulChromaStride;
	uint32_t                       ulLumaStride;

} BVDC_P_FeederContext;


/***************************************************************************
 * MPEG/Video feeder private functions
 ***************************************************************************/
BERR_Code BVDC_P_Feeder_Create
	( BVDC_P_Feeder_Handle            *phFeeder,
	  BRDC_Handle                      hRdc,
	  const BREG_Handle                hReg,
	  const BVDC_P_FeederId            eFeederId,
	  BTMR_TimerHandle                 hTimer,
	  BVDC_Source_Handle               hSource,
	  BVDC_P_Resource_Handle           hResource,
	  bool                             b3dSrc );

void BVDC_P_Feeder_Destroy
	( BVDC_P_Feeder_Handle             hFeeder );

void BVDC_P_Feeder_Init
	( BVDC_P_Feeder_Handle             hFeeder,
	  BMEM_Heap_Handle                 hMem,
	  bool                             bGfxSrc );

void BVDC_P_Feeder_BuildRul_isr
	( const BVDC_P_Feeder_Handle       hFeeder,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState,
	  BVDC_P_PicComRulInfo            *pPicComRulInfo );

BERR_Code BVDC_P_Feeder_SetMpegInfo_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const uint32_t                   ulMuteColorYCrCb,
	  const BAVC_MVD_Field            *pFieldData,
	  const BVDC_P_Rect               *pScanOutRect);

void BVDC_P_Feeder_SetPlaybackInfo_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  const BVDC_P_PictureNodePtr      pPicture,
	  bool                             bFixedColor,
	  const uint32_t                   ulMuteColorYCrCb );

BERR_Code BVDC_P_Feeder_SetEnable_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  bool                             bEnable );

void BVDC_P_Feeder_GetCrc_isr
	( BVDC_P_Feeder_Handle             hFeeder,
	  BREG_Handle                      hReg,
	  BVDC_Source_CallbackData        *pData );

BERR_Code BVDC_P_Feeder_ValidateChanges
	( BVDC_P_Feeder_Handle             hFeeder,
	  BVDC_Source_PictureCallback_isr  pfPicCallbackFunc );

BERR_Code BVDC_P_Feeder_ApplyChanges_isr
	( BVDC_P_Feeder_Handle             hFeeder );

void BVDC_P_Feeder_AbortChanges
	( BVDC_P_Feeder_Handle             hFeeder );

BERR_Code BVDC_P_Feeder_ValidateGfxSurAndRects
	( BVDC_P_SurfaceInfo                *pSur,
	  const BVDC_P_ClipRect             *pClipRect,
	  const BVDC_P_Rect                 *pSclOutRect );

void BVDC_P_Feeder_HandleIsrGfxSurface_isr
	( BVDC_P_Feeder_Handle         hFeeder,
  	  BVDC_P_Source_Info *         pCurSrcInfo,
	  BAVC_Polarity                eFieldId );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_FEEDER_PRIV_H__ */
/* End of file. */
