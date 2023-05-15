/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_compositor_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/144 $
 * $brcm_Date: 8/15/12 6:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_compositor_priv.h $
 * 
 * Hydra_Software_Devel/150   4/2/13 3:00p pntruong
 * SW7425-4579: Keep track of mosaic on a per window context.
 *
 * Hydra_Software_Devel/144   8/15/12 6:20p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/143   8/13/12 4:03p tdo
 * SW7425-3734: Fix coverity issues
 *
 * Hydra_Software_Devel/142   8/8/12 4:22p vanessah
 * SW7425-3704: add gfx xcode channelchange flag
 *
 * Hydra_Software_Devel/141   6/29/12 3:30p vanessah
 * SW7425-3359: FNRT flags support to ViCE MBox
 *
 * Hydra_Software_Devel/140   6/6/12 4:51p tdo
 * SW7552-279: video(color or brightness) is abnormal after change channel
 * to NTSC SD program when brightness/contrast/saturation/Hue is not
 * default value
 *
 * Hydra_Software_Devel/139   5/1/12 3:05p vanessah
 * SW7425-2643: add bar data support
 *
 * Hydra_Software_Devel/138   4/12/12 4:11p vanessah
 * SW7425-2831: undo the changes
 *
 * Hydra_Software_Devel/137   4/6/12 8:00p vanessah
 * SW7425-2831: change polarity due to ignore frame flag
 *
 * Hydra_Software_Devel/136   2/23/12 10:17a vanessah
 * SW7425-2431: fix custom fmt stress test
 *
 * Hydra_Software_Devel/135   1/30/12 4:05p vanessah
 * SW7425-2239: add  bChannelChange in VCE Mbox
 *
 * Hydra_Software_Devel/134   1/22/12 2:58p vanessah
 * SW7435-9: fix 7435 compile error
 *
 * Hydra_Software_Devel/133   1/20/12 6:28p vanessah
 * SW7435-9: add CMP4/5 support
 *
 * Hydra_Software_Devel/132   1/16/12 12:58p vanessah
 * SW7425-2084: add bLast flage to VCD Mbox
 *
 * Hydra_Software_Devel/131   11/29/11 11:25a vanessah
 * SW7425-1783: add user data support for transcoding
 *
 * Hydra_Software_Devel/130   10/10/11 3:17p pntruong
 * SW7425-1439: Fixed initialization of cmp reset/mask addr.
 *
 * Hydra_Software_Devel/129   8/31/11 11:18a syang
 * SW7425-1170: pxlAspRatio passed to ViCE2 is corrected to x<<16 | y
 * format
 *
 * Hydra_Software_Devel/128   8/27/11 7:22p hongtaoz
 * SW7425-1132: replaced slip2lock boolean flag with integer counter to
 * fix a timing sensitive hang conditiion when NRT sync-locked window is
 * brought up;
 *
 * Hydra_Software_Devel/127   8/26/11 5:38p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/3   8/26/11 4:21p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/24/11 6:55p vanessah
 * SW7425-1170: ASP code clean
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/23/11 6:16p vanessah
 * SW7425-1170: Aspect ratio cleanup
 *
 * Hydra_Software_Devel/126   8/2/11 7:33p vanessah
 * SW7425-465: optimization of delay counter
 *
 * Hydra_Software_Devel/125   7/22/11 10:59a yuxiaz
 * SW7420-1959: Back out support of non-full content mode for PIP, use
 * mosaic APIs instead.
 *
 * Hydra_Software_Devel/124   7/18/11 5:31p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/123   6/30/11 6:51p yuxiaz
 * SW7420-1959: Added support of non-full content mode for PIP.
 *
 * Hydra_Software_Devel/122   6/24/11 2:58p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/121   6/22/11 3:07p yuxiaz
 * SW7425-592: Added API to get and set display sample aspect ratio
 * settings.
 *
 * Hydra_Software_Devel/120   6/10/11 5:49p hongtaoz
 * SW7425-704: add NRT mode support to VDC; moved the STG meta data
 * passing from window writer isr to reader isr; added bStallStc flag
 * support;
 *
 * Hydra_Software_Devel/119   5/27/11 6:36p vanessah
 * SW7425-465: fix video transcoding resolution switch transition artifact
 *
 * Hydra_Software_Devel/118   5/11/11 3:50p pntruong
 * SW7231-140: Correctly compute the MFD fieldswap flag.
 *
 * Hydra_Software_Devel/117   2/7/11 6:03p vanessah
 * SW7425-46: STG MBox data programming
 *
 * Hydra_Software_Devel/116   2/7/11 5:25p vanessah
 * SW7425-46: add MFD mute to the skip picture to start quality comparison
 *
 * Hydra_Software_Devel/115   11/16/10 6:50p tdo
 * SW7422-92: Need display 3D orientation override function for VDC
 *
 * Hydra_Software_Devel/114   10/22/10 7:02p vanessah
 * SW7425-46:  ViCE2 meta data mailbox register
 *
 * Hydra_Software_Devel/113   10/12/10 5:29p hongtaoz
 * SW7425-30, SW7425-33: added 4th CMP and 3rd MFD support for 7425;
 *
 * Hydra_Software_Devel/112   5/7/10 7:08p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/111   4/7/10 11:28a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/110   4/5/10 4:01p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/109   3/17/10 12:01p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/108   12/21/09 2:44p rpan
 * SW7468-30: 1) When OSCL is enabled for 1080p display, top and bottom
 * RDC slots are used for building RULs.
 * 2) Addressed the video format stored at hCompistor being overwritten
 * issue.
 *
 * Hydra_Software_Devel/107   11/30/09 10:34a rpan
 * SW7468-30: Initial OSCL support.
 *
 * Hydra_Software_Devel/106   12/4/08 3:46p rpan
 * PR45804: Made changes from source follow the new event driven sheme.
 *
 * Hydra_Software_Devel/105   10/23/08 5:44p albertl
 * PR47318, PR47814: Fixed overflow when calculating RGB attenuation
 * offsets.  Rewrote portions of RGB attenuation code and fixed various
 * offset related issues.
 *
 * Hydra_Software_Devel/104   10/9/08 5:27p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/103   10/7/08 4:04p syang
 * PR 41898: 1). move video sur enabling (in RUL) from cmp to win together
 * with other modules in  readerBuilRul;  2). get rid of muteReader; 3).
 * consolidate sur constant color usage impl with vnet start/shut-down
 *
 * Hydra_Software_Devel/102   9/17/08 4:54p albertl
 * PR46491: Fixed additional build errors.  Replaced chip version defines
 * with BVDC_P_SUPPORT_CSC_MAT_COEF_VER.
 *
 * Hydra_Software_Devel/101   9/16/08 5:39p albertl
 * PR46491: Fixed build errors.  Cleaned up 3568 A0 ifdefs.
 *
 * Hydra_Software_Devel/100   8/8/08 3:28p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/99   6/7/08 8:27p albertl
 * PR39336, PR39163:  Fixed bugs in second compositor support.  Added
 * dirty bits mechanism to compositor for colorclip.
 *
 * Hydra_Software_Devel/98   5/21/08 1:54p albertl
 * PR39163:  Updated CMP csc tables to use 10 bit offset values.
 * Corrected to use proper csc tables in conversions involving ntsc, pal,
 * and smpte 240M.  Moved colorclip from window to compositor and changed
 * BVDC_Window_Set/GetColorClip to BVDC_Compositor_Set/GetColorClip.
 *
 * Hydra_Software_Devel/97   4/8/08 6:32p albertl
 * PR39336:  Added register remap workarounds to allow reuse of CMP0 code
 * on CMP1 for 3548 A0.
 *
 * Hydra_Software_Devel/96   4/1/08 3:06p rpan
 * PR39337: Implemented CMP OSD configuration.
 *
 * Hydra_Software_Devel/95   3/27/08 5:07p albertl
 * PR39163:  Implemented non-linear xvYCC csc support for 3548.
 *
 * Hydra_Software_Devel/94   3/3/08 1:59p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/93   2/21/08 5:00p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/92   1/13/08 3:27a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/91   11/28/07 2:00p tdo
 * PR37413: Provide new Api to set offset and size of histogram window and
 * read corresponding parameters from specific window
 *
 * Hydra_Software_Devel/90   10/30/07 7:11p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/89   9/24/07 3:42p hongtaoz
 * PR34955: track and clean up the sync-slipped mpeg PIP source during
 * display format switch;
 *
 * Hydra_Software_Devel/88   6/15/07 5:07p tdo
 * PR30613: Use separate control bit for CSC compute.
 *
 * Hydra_Software_Devel/87   4/5/07 2:01p pntruong
 * PR28538, PR28539, PR28540, PR25462: Average pixel luma and customized
 * deinterlacer support.
 *
 * Hydra_Software_Devel/PR28538/1   4/3/07 9:33p xhuang
 * PR25462: Support APL setting
 *
 * Hydra_Software_Devel/86   12/18/06 11:18p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   11/15/06 5:05p albertl
 * PR24587:  Added downsample source for 7440.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/19/06 6:58p hongtaoz
 * PR23260: add dvo master mode support; add support for cmp/display
 * connection at create time; restructured display code to be scalable
 * for 7400B support;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/29/06 2:17p tdo
 * PR 24608:  Add 240p mode support
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/19/06 1:46p pntruong
 * PR23222, PR23225: VDEC - ATSC & PC input support.
 *
 * Hydra_Software_Devel/84   8/15/06 6:16p hongtaoz
 * PR18233, PR20716: added support for mosaic mode - missing channels;
 * assume picture list's channel id are in ascending order; single
 * channel will not result in full-screen automatically;
 *
 * Hydra_Software_Devel/83   8/7/06 5:26p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/82   6/14/06 6:54p hongtaoz
 * PR20716, PR21804: implement modified version of mosaic mode;
 *
 * Hydra_Software_Devel/81   5/26/06 3:37p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/80   5/19/06 1:46p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/79   4/20/06 5:49p pntruong
 * PR20750: Please update bavc.h to reflect presence of third VEC.
 *
 * Hydra_Software_Devel/78   4/4/06 1:48p hongtaoz
 * PR20403: add triple displays for 7400;
 *
 * Hydra_Software_Devel/77   3/30/06 9:22p hongtaoz
 * PR20520, PR17512: checked RUL execution status when CMP's color space
 * is dirty; count twice of execution status in case it's interlaced
 * slots;
 *
 * Hydra_Software_Devel/76   1/12/06 10:36a yuxiaz
 * PR18476: Added new functions BVDC_Window_SetLumaKey and
 * BVDC_Window_GetLumaKey for luma key support.
 *
 * Hydra_Software_Devel/75   11/23/05 5:28p hongtaoz
 * PR18180: added 7401 support for 2 CMP's;
 *
 * Hydra_Software_Devel/74   10/19/05 3:17p hongtaoz
 * PR17512: added separate flag to build cmp color matrix RUL; window csc
 * dirty bit initiates redetermine window color matrix logic;
 *
 * Hydra_Software_Devel/73   8/29/05 3:30p hongtaoz
 * PR16812, PR15495: build both T/B RULs for sync-slipped playback side
 * muting RUL and not touch window playback side before the new vnet RUL
 * is executed; restore window's current shutdown state without affecting
 * user setting;
 *
 * Hydra_Software_Devel/72   8/18/05 1:14p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/2   8/16/05 2:01p pntruong
 * PR12519: Take in additional fixes from mainline.
 *
 * Hydra_Software_Devel/71   8/10/05 6:30p syang
 * PR 12132, PR 14862, PR 15146: half coding checkin for adding src and
 * dst aspect ratio canvas control
 *
 * Hydra_Software_Devel/70   3/17/05 6:36p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/69   3/11/05 9:21a pntruong
 * PR14112, PR14046: Fixed hd/sd channel change for progressive mode.
 *
 * Hydra_Software_Devel/68   3/9/05 4:50p pntruong
 * PR14112, PR14046: Support out of order destroys for "cloned" windows in
 * VDC. and While in mirror mode with PIP, VDC times out when toggling
 * between analog and digital channel.
 *
 * Hydra_Software_Devel/67   2/28/05 10:54a pntruong
 * PR13575: Window destroy timeout at channel change.
 *
 ***************************************************************************/
#ifndef BVDC_COMPOSITOR_PRIV_H__
#define BVDC_COMPOSITOR_PRIV_H__

#include "bvdc.h"
#include "bchp_common.h"
#include "bvdc_common_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_display_priv.h"
#include "bchp_cmp_0.h"
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER == 2)
#include "bchp_cmp_1.h"
#endif

#if BVDC_P_SUPPORT_DOWNSAMPLE
#include "bvdc_downsample_priv.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Private register cracking macros
 ***************************************************************************/
#define BVDC_P_CMP_GET_REG_IDX(reg) \
	((BCHP##_##reg - BCHP_CMP_0_REG_START) / sizeof(uint32_t))

/* Get/Set reg data */
#define BVDC_P_CMP_GET_REG_DATA(reg) \
	(hCompositor->aulRegs[BVDC_P_CMP_GET_REG_IDX(reg)])
#define BVDC_P_CMP_SET_REG_DATA(reg, data) \
	(BVDC_P_CMP_GET_REG_DATA(reg) = (uint32_t)(data))

#define BVDC_P_CMP_GET_REG_DATA_I(idx, reg) \
	(hCompositor->aulRegs[BVDC_P_CMP_GET_REG_IDX(reg) + (idx)])

/* Get field */
#define BVDC_P_CMP_GET_FIELD_NAME(reg, field) \
	(BVDC_P_GET_FIELD(BVDC_P_CMP_GET_REG_DATA(reg), reg, field))

/* Compare field */
#define BVDC_P_CMP_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_COMPARE_FIELD_DATA(BVDC_P_CMP_GET_REG_DATA(reg), reg, field, (data)))
#define BVDC_P_CMP_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_COMPARE_FIELD_NAME(BVDC_P_CMP_GET_REG_DATA(reg), reg, field, name))

/* This macro does a write into RUL (write, addr, data). 3 dwords. */
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER == 2)
#define BVDC_P_CMP_WRITE_TO_RUL(reg, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BVDC_P_Compositor_GetCmpRegAddr_isr(hCompositor->eId, BCHP##_##reg) + hCompositor->ulRegOffset); \
	*addr_ptr++ = BVDC_P_CMP_GET_REG_DATA(reg); \
}
#else
#define BVDC_P_CMP_WRITE_TO_RUL(reg, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BCHP##_##reg + hCompositor->ulRegOffset); \
	*addr_ptr++ = BVDC_P_CMP_GET_REG_DATA(reg); \
}
#endif

/* This macro does a block write into RUL */
#define BVDC_P_CMP_BLOCK_WRITE_TO_RUL(from, to, pulCurrent) \
do { \
	uint32_t ulBlockSize = \
		BVDC_P_REGS_ENTRIES(from, to);\
	*pulCurrent++ = BRDC_OP_IMMS_TO_REGS( ulBlockSize ); \
	*pulCurrent++ = BRDC_REGISTER(BCHP##_##from + hCompositor->ulRegOffset); \
	BKNI_Memcpy((void*)pulCurrent, \
		(void*)&(hCompositor->aulRegs[BVDC_P_CMP_GET_REG_IDX(from)]), \
		ulBlockSize * sizeof(uint32_t)); \
	pulCurrent += ulBlockSize; \
} while(0)

/* This macro does a block write into RUL */
#define BVDC_P_CMP_RECT_BLOCK_WRITE_TO_RUL(from, cnt, pulCurrent) \
do { \
	*pulCurrent++ = BRDC_OP_IMMS_TO_REGS( cnt ); \
	*pulCurrent++ = BRDC_REGISTER(BCHP##_##from + hCompositor->ulRegOffset); \
	BKNI_Memcpy((void*)pulCurrent, \
		(void*)&(hCompositor->aulRegs[BVDC_P_CMP_GET_REG_IDX(from)]), \
		(cnt) * sizeof(uint32_t)); \
	pulCurrent += cnt; \
} while(0)

#define BVDC_P_CMP_REGS_COUNT \
	BVDC_P_REGS_ENTRIES(CMP_0_REG_START, CMP_0_REG_END)

/* get register offset base on id. */
#ifdef BCHP_CMP_6_REG_START
#define BVDC_P_CMP_GET_REG_OFFSET(cmp_id) \
	((BVDC_CompositorId_eCompositor6==(cmp_id)) ? (BCHP_CMP_6_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor5==(cmp_id)) ? (BCHP_CMP_5_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor4==(cmp_id)) ? (BCHP_CMP_4_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor3==(cmp_id)) ? (BCHP_CMP_3_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor2==(cmp_id)) ? (BCHP_CMP_2_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor1==(cmp_id)) ? (BCHP_CMP_1_REG_START - BCHP_CMP_0_REG_START) \
	:(0))
#else
#ifdef BCHP_CMP_5_REG_START
#define BVDC_P_CMP_GET_REG_OFFSET(cmp_id) \
	((BVDC_CompositorId_eCompositor5==(cmp_id)) ? (BCHP_CMP_5_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor4==(cmp_id)) ? (BCHP_CMP_4_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor3==(cmp_id)) ? (BCHP_CMP_3_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor2==(cmp_id)) ? (BCHP_CMP_2_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor1==(cmp_id)) ? (BCHP_CMP_1_REG_START - BCHP_CMP_0_REG_START) \
	:(0))
#else
#ifdef BCHP_CMP_4_REG_START
#define BVDC_P_CMP_GET_REG_OFFSET(cmp_id) \
	((BVDC_CompositorId_eCompositor4==(cmp_id)) ? (BCHP_CMP_4_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor3==(cmp_id)) ? (BCHP_CMP_3_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor2==(cmp_id)) ? (BCHP_CMP_2_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor1==(cmp_id)) ? (BCHP_CMP_1_REG_START - BCHP_CMP_0_REG_START) \
	:(0))
#else
#ifdef BCHP_CMP_3_REG_START
#define BVDC_P_CMP_GET_REG_OFFSET(cmp_id) \
	((BVDC_CompositorId_eCompositor3==(cmp_id)) ? (BCHP_CMP_3_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor2==(cmp_id)) ? (BCHP_CMP_2_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor1==(cmp_id)) ? (BCHP_CMP_1_REG_START - BCHP_CMP_0_REG_START) \
	:(0))
#else
#ifdef BCHP_CMP_2_REG_START
#define BVDC_P_CMP_GET_REG_OFFSET(cmp_id) \
	((BVDC_CompositorId_eCompositor2==(cmp_id)) ? (BCHP_CMP_2_REG_START - BCHP_CMP_0_REG_START) \
	:(BVDC_CompositorId_eCompositor1==(cmp_id)) ? (BCHP_CMP_1_REG_START - BCHP_CMP_0_REG_START) \
	:(0))
#else
#ifdef BCHP_CMP_1_REG_START
#define BVDC_P_CMP_GET_REG_OFFSET(cmp_id) \
	((BVDC_CompositorId_eCompositor1==(cmp_id)) ? (BCHP_CMP_1_REG_START - BCHP_CMP_0_REG_START) \
	:(0))
#else
#define BVDC_P_CMP_GET_REG_OFFSET(cmp_id)           (0)
#endif /* CMP_1 */
#endif /* CMP_2 */
#endif /* CMP_3 */
#endif /* CMP_4 */
#endif /* CMP_5 */
#endif /* CMP_6 */

#define BVDC_P_CMP_GET_LIST_IDX(polarity_id, idx) \
	(BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT * (polarity_id) + (idx))

/* For RUL multi-buffering. */
#define BVDC_P_CMP_NEXT_RUL(hCompositor, polarity_id) \
	((hCompositor)->aulRulIdx[(polarity_id)] = \
	BVDC_P_NEXT_RUL_IDX((hCompositor)->aulRulIdx[(polarity_id)]))

/* Get the current list pointed by aulRulIdx[field]. */
#define BVDC_P_CMP_GET_LIST(hCompositor, polarity_id) \
	((hCompositor)->ahList[BVDC_P_CMP_GET_LIST_IDX((polarity_id), \
		(hCompositor)->aulRulIdx[(polarity_id)])])

/* Compositor only uses T/B slot. */
#define BVDC_P_CMP_MAX_SLOT_COUNT \
	(2)

#define BVDC_P_CMP_MAX_LIST_COUNT \
	(BVDC_P_CMP_MAX_SLOT_COUNT * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT)

/* Get the current slot pointed by field. */
#define BVDC_P_CMP_GET_SLOT(hCompositor, polarity_id) \
	((hCompositor)->ahSlot[(polarity_id)])

#define BVDC_P_CMP_GET_DISP_TOP_TRIGGER(hCompositor) \
	(BVDC_P_DISP_GET_TOP_TRIGGER((hCompositor)->hDisplay))

#define BVDC_P_CMP_GET_DISP_BOT_TRIGGER(hCompositor) \
	(BVDC_P_DISP_GET_BOT_TRIGGER((hCompositor)->hDisplay))

#define BVDC_P_CMP_GET_DOWNSAMPLE(hCompositor) \
	((hCompositor)->hDownSample)

/* Number of blender availables. */
#define BVDC_P_CMP_MAX_BLENDER        (BVDC_Z_ORDER_MAX + 1)

/* Compositor feature entry */
typedef struct
{
	uint32_t                          ulMaxVideoWindow;
	uint32_t                          ulMaxGfxWindow;
	uint32_t                          ulMaxWindow;

} BVDC_P_Compositor_Features;

/* Compositor dirty bits */
typedef union
{
	struct
	{
		uint32_t                          bColorClip : 1; /* new colorclip settings */
#if	BVDC_P_SUPPORT_OSCL
		uint32_t						  bOScl: 1; /* enable/disable OSCL */
#endif
	} stBits;

	uint32_t aulInts[BVDC_P_DIRTY_INT_ARRAY_SIZE];

} BVDC_P_Compositor_DirtyBits;

/* Compositor infos */
typedef struct
{
	uint32_t                          ulBgColorYCrCb;
	uint8_t                           ucRed;
	uint8_t                           ucGreen;
	uint8_t                           ucBlue;
	const BFMT_VideoInfo             *pFmtInfo;
	bool                              bLumaRectUserSet;
	BVDC_LumaSettings                 stLumaRect;
	BVDC_ColorClipSettings            stColorClipSettings;
	BFMT_Orientation                  eOrientation;

#if BVDC_P_SUPPORT_OSCL
	uint32_t                          bEnableOScl; /* enable OSCL for 1080i to 1080p convertion */
#endif

	/* dirty bits */
	BVDC_P_Compositor_DirtyBits       stDirty;
} BVDC_P_Compositor_Info;


/***************************************************************************
 * Compositor Context
 ***************************************************************************/
typedef struct BVDC_P_CompositorContext
{
	BDBG_OBJECT(BVDC_CMP)

	/* flag initial state, requires reset; */
	bool                              bIsBypass;
	bool                              bInitial;
	uint32_t                          ulCoreResetAddr;
	uint32_t                          ulCoreResetMask;

	/* public fields that expose thru API. */
	BVDC_P_Compositor_Info            stNewInfo;
	BVDC_P_Compositor_Info            stCurInfo;

	uint32_t                          ulDelayCnt;

	/* for display to infom window ApplyChange */
	bool                              bDspAspRatDirty;

	/* Set to true when new & old validated by apply changes  These
	 * flags get updated at applychanges. */
	bool                              bUserAppliedChanges;
	BVDC_Window_Handle                hSyncLockWin; /* window locked to this compositor */
	BVDC_Source_Handle                hSyncLockSrc; /* source locked to this compositor */
	BVDC_Source_Handle                hForceTrigPipSrc;
	BVDC_Source_Handle                hSrcToBeLocked;
	uint32_t                          ulSlip2Lock;

	/* RUL use for this compositor & display, (not created by compositor) */
	uint32_t                          aulRulIdx[BVDC_P_CMP_MAX_SLOT_COUNT];
	BRDC_Slot_Handle                  ahSlot[BVDC_P_CMP_MAX_SLOT_COUNT];
	BRDC_List_Handle                  ahList[BVDC_P_CMP_MAX_LIST_COUNT];
	BINT_CallbackHandle               ahCallback[BVDC_P_CMP_MAX_SLOT_COUNT];

	/* shadowed registers */
	BVDC_CompositorId                 eId;
	BVDC_P_State                      eState;
	uint32_t                          ulCmpCount;
	uint32_t                          ulRegOffset; /* CMP_0, CMP_1, and etc. */
	uint32_t                          aulRegs[BVDC_P_CMP_REGS_COUNT];

	/* Compositor features. */
	const BVDC_P_Compositor_Features *pFeatures;

	/* Computed value */
	uint32_t                          ulActiveVideoWindow;
	uint32_t                          ulActiveGfxWindow;
	bool                              abBlenderUsed[BVDC_P_CMP_MAX_BLENDER];


	/* Compositor output to Display STG*/

	/* DW-1 MBOX Original PTS */
	uint32_t                          ulOrigPTS;
	int32_t                           uiHorizontalPanScan;   /* MPEG-2 Data format*/
	int32_t                           uiVerticalPanScan;     /* Same as above*/
	uint32_t                          ulDisplayHorizontalSize;
	uint32_t                          ulDisplayVerticalSize;
	uint32_t                          ulPicId;
	BAVC_USERDATA_PictureCoding       ePictureType;
	uint32_t                          ulChannelId;
	BAVC_Polarity                     eSourcePolarity;
	bool                              bPictureRepeatFlag;      /*picture repeat due to cadence detection or frame rate conversion*/
	bool                              bIgnorePicture;          /* from DM*/
	bool                              bStallStc;               /* from DM*/
	bool                              bLast;                   /* from DM*/
	bool                              bChannelChange;          /* from DM*/
	bool                              bGfxChannelChange;       /* GFX window indicator, maintained by vdc*/
	uint32_t                          ulStgPxlAspRatio_x_y;    /* PxlAspRatio_x<<16 | PxlAspRatio_y */
	uint32_t                          ulDecodePictureId;
	bool                              bValidAfd;
	uint32_t                          ulAfd;
	BAVC_BarDataType                  eBarDataType;
	uint32_t                          ulTopLeftBarValue;
	uint32_t                          ulBotRightBarValue;
	/*  Fast non real time (FNRT) meta data support */
	bool                              bPreChargePicture;
	bool                              bEndofChunk;
	uint32_t                          ulChunkId;

	/* Ouptut to VEC. */
	BVDC_P_CmpColorSpace              eCmpColorSpace;
	uint32_t                          ulVdecPhase;
	bool                              b240pMode;
	BVDC_PhaseAdjustMode              ePhaseAdjustMode;
	BAVC_FrameRateCode                eSrcFRateCode;
	bool                              bFullRate;

	/* active windows (declare max).  Could also be dynamically allocated
	 * to BVDC_P_CMP_X_MAX_WINDOW_COUNT.  But this is pretty much fix. */
	BVDC_Window_Handle                ahWindow[BVDC_P_MAX_WINDOW_COUNT];
	uint32_t                          ulCscAdjust[BVDC_P_MAX_WINDOW_COUNT];
	bool                              bCscCompute[BVDC_P_MAX_WINDOW_COUNT];
	bool                              bCscDemoCompute[BVDC_P_MAX_WINDOW_COUNT];
	uint32_t                          ulLumaKeyAdjust[BVDC_P_MAX_WINDOW_COUNT];
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	uint32_t                          ulMosaicAdjust[BVDC_P_MAX_WINDOW_COUNT];
#else
	uint32_t                          ulMosaicAdjust;
#endif

	/* Dither */
	BVDC_P_DitherSetting              stV0Dither;
	BVDC_P_DitherSetting              stV1Dither;

	/* Associated w/ this display handle. */
	BVDC_Display_Handle               hDisplay;

	/* Associated w/ this downsample handle. */
	BVDC_P_DownSample_Handle          hDownSample;

	 /* Created from this vdc */
	BVDC_Handle                       hVdc;
} BVDC_P_CompositorContext;


/***************************************************************************
 * Compositor private functions
 ***************************************************************************/
BERR_Code BVDC_P_Compositor_Create
	( BVDC_Handle                      hVdc,
	  BVDC_Compositor_Handle          *phCompositor,
	  BVDC_CompositorId                eCompositorId );

void BVDC_P_Compositor_Destroy
	( BVDC_Compositor_Handle           hCompositor );

void BVDC_P_Compositor_Init
	( BVDC_Compositor_Handle           hCompositor );

BERR_Code BVDC_P_Compositor_ValidateChanges
	( const BVDC_Compositor_Handle     ahCompositor[] );

void BVDC_P_Compositor_ApplyChanges_isr
	( BVDC_Compositor_Handle           hCompositor );

void BVDC_P_Compositor_AbortChanges
	( BVDC_Compositor_Handle           hCompositor );

void BVDC_P_Compositor_BuildSyncLockRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId );

bool BVDC_P_Compositor_BuildSyncSlipRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId,
	  bool                             bBuildCanvasCtrl );

void BVDC_P_Compositor_BuildConvasCtrlRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_P_ListInfo                 *pList );

/* Miscellaneous access functions */
#if !BVDC_P_ORTHOGONAL_VEC
BERR_Code BVDC_P_Compositor_GetOutputInfo_isr
	( const BVDC_Compositor_Handle     hCompositor,
	  bool                            *pbFullRate,
	  BVDC_PhaseAdjustMode            *pePhaseAdjustMode,
	  uint32_t                        *pulVdecPhase,
	  bool                            *pb240pMode );
#endif

BERR_Code BVDC_P_Compositor_AssignTrigger_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BRDC_Trigger                     eTopTrigger,
	  BRDC_Trigger                     eBotTrigger );

void BVDC_P_Compositor_WindowsReader_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BAVC_Polarity                    eNextFieldId,
	  BVDC_P_ListInfo                 *pList );

void BVDC_P_Compositor_GetCscTable_isr
	( BVDC_P_CscCoeffs               **ppCscCoeffsTable,
	  bool                             bCscRgbMatching,
	  BAVC_MatrixCoefficients          eInputColorSpace,
	  BVDC_P_CmpColorSpace             eOutputColorSpace,
	  bool                             bInputXvYcc );

void BVDC_P_Compositor_GetCscTablesNonLinear_isr
	( BVDC_P_CscCoeffs               **pCscCoeffsTableA,
	  BVDC_P_CscCoeffs               **pCscCoeffsTableB,
	  BVDC_P_CscCoeffs               **pCscCoeffsTableC,
	  BAVC_MatrixCoefficients          eInputColorSpace,
	  BVDC_P_CmpColorSpace             eOutputColorSpace,
	  bool                             bInputXvYcc );

uint32_t BVDC_P_Compositor_GetCmpRegAddr_isr
	( BVDC_CompositorId                eId,
	  uint32_t                         ulRegAddr);

void BVDC_P_Compositor_SetMBoxMetaData_isr
	(
	const BVDC_P_PictureNode              *pPicture,
	BVDC_Compositor_Handle                hCompositor);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_COMPOSITOR_PRIV_H__ */
/* End of file. */
