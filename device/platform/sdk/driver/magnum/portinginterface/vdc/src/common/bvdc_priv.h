/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/127 $
 * $brcm_Date: 9/10/12 6:26p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_priv.h $
 * 
 * Hydra_Software_Devel/127   9/10/12 6:26p tdo
 * SW7231-837: Remove band gap calibration option
 *
 * Hydra_Software_Devel/126   8/16/12 3:00p vanessah
 * SW7425-2571: change on BVDC_Dbg_MaskBvnErrorCb
 *
 * Hydra_Software_Devel/125   8/15/12 6:20p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/124   7/24/12 3:02p tdo
 * SW7360-29 : Temporarily turn off DAC auto-detection & calibration until
 * it's fully working for 7360 and 7231
 *
 * Hydra_Software_Devel/123   7/5/12 12:02p tdo
 * SW7231-837:  Add option to turn on/off Dac Gain Override
 *
 * Hydra_Software_Devel/122   6/20/12 4:42p pntruong
 * SW7405-5488: Added support for querying display capabilties of vdc
 * supportable format.
 *
 * Hydra_Software_Devel/121   4/26/12 11:34a tdo
 * SW7425-2571: Add VDC API to query BVN module error
 *
 * Hydra_Software_Devel/120   3/1/12 11:46a yuxiaz
 * SW7425-2526, SW7425-1182: Added runtime query capabilities for source
 * in VDC.
 *
 * Hydra_Software_Devel/119   1/13/12 2:51p tdo
 * SW7358-159 , SW7418-55 : Default DAC auto detection ON for
 * 7231/7344/7346Bx
 *
 * Hydra_Software_Devel/118   12/1/11 4:33p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/117   11/18/11 3:09p yuxiaz
 * SW7420-2135: Move MAX number of RULs defines to bvdc_common_priv.h
 *
 * Hydra_Software_Devel/116   10/14/11 2:23p tdo
 * SW7425-1416, SW7358-159: Add feature to control automatic DAC
 * detection.  Default is currently OFF until it's fully functional.
 *
 * Hydra_Software_Devel/115   8/31/11 11:19a syang
 * SW7425-1170: pxlAspRatio passed to ViCE2 is corrected to x<<16 | y
 * format
 *
 * Hydra_Software_Devel/114   8/26/11 5:38p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/26/11 4:22p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/23/11 6:20p vanessah
 * SW7425-1170: Aspect ratio cleanup
 *
 * Hydra_Software_Devel/113   8/4/11 7:20p tdo
 * SW7425-979: Add support for 7425B0 DAC name change
 *
 * Hydra_Software_Devel/112   7/22/11 4:56p tdo
 * SW7422-194: implement 40nm vDAC scaled voltage swing and auto-detection
 * to reduce power dissipation
 *
 * Hydra_Software_Devel/111   2/9/11 3:35p pntruong
 * SW7420-1456: Initial standby power management that used chp's pm
 * functionalities.
 *
 * Hydra_Software_Devel/110   11/12/10 3:56p pntruong
 * SW7425-31: Takes bvn yuv into account for hddvi input.  Fixed bad debug
 * register read on non-mfd source.  Updated scratch registers
 * availabliity.
 *
 * Hydra_Software_Devel/109   10/20/10 4:11p yuxiaz
 * SW7420-1190: Put back generic VDC drian buffer for mosaic mode. Only
 * allocate drain buffer in window if it does not use main VDC heap.
 *
 * Hydra_Software_Devel/108   10/19/10 4:07p yuxiaz
 * SW7420-1190: Make mosaic scratch buffer to be per window base.
 *
 * Hydra_Software_Devel/107   8/26/10 5:27p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/106   6/23/10 4:59p rpan
 * SW7400-2808: Stop enabling BVN while aligning VECs.
 *
 * Hydra_Software_Devel/105   5/7/10 7:19p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/104   4/19/10 10:18p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/103   4/7/10 11:34a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/102   4/5/10 4:13p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/101   8/21/09 2:37p tdo
 * PR57734: Add capability for display to handle DACs re-assignment
 *
 * Hydra_Software_Devel/100   6/25/09 12:11p rpan
 * PR56137, PR56138, PR56139, PR56166, PR56167, PR56168: Support for
 * various orthogonal VEC configurations.
 *
 * Hydra_Software_Devel/99   6/18/09 5:54p syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/98   6/5/09 3:08p pntruong
 * PR54615: [M+T][LCD][VIDEO] Pink gargabe blinks on DTV.  Ensured that
 * the window shutdown process go thru upon destroy.
 *
 * Hydra_Software_Devel/97   2/17/09 2:33p rpan
 * PR52001: Added 7420 DAC connection state for power management.
 *
 * Hydra_Software_Devel/96   2/10/09 4:47p rpan
 * PR52001: Added 7420 DAC power management.
 *
 * Hydra_Software_Devel/95   11/12/08 3:14p tdo
 * PR48642: Provide clipping rect for histo luma region
 *
 * Hydra_Software_Devel/94   9/5/08 4:43p tdo
 * PR46484: Bringup appframework for7420
 *
 * Hydra_Software_Devel/93   8/8/08 3:30p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/92   7/23/08 7:01p tdo
 * PR43508, PR43509: Mapping individual bandgap adjustment for each DAC
 *
 * Hydra_Software_Devel/91   2/29/08 4:19p yuxiaz
 * PR39158: Implement dithering in various BVN components for 3548.
 *
 * Hydra_Software_Devel/90   2/21/08 5:00p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/89   10/23/07 11:16a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/1   10/12/07 10:49a yuxiaz
 * PR29569: Add FGT support on 7405
 *
 * Hydra_Software_Devel/88   2/20/07 2:56p jessem
 * PR25235: Removed the use of rate mask and replaced with actual value of
 * source frame and display refresh rates.
 *
 * Hydra_Software_Devel/87   2/16/07 9:37a pntruong
 * PR15284, PR27951: Graphics shimmering on HD path when video is scaled
 * down (PIG).  HW fixed.  Removed software work-around to avoid bvb
 * error interrupts from window surface.  Make bvdc dbg more portable.
 *
 * Hydra_Software_Devel/86   1/24/07 9:07p albertl
 * PR22237:  Updated BMEM calls to use new BMEM_Heap functions.
 *
 * Hydra_Software_Devel/85   12/19/06 3:56p hongtaoz
 * PR25668: added 7403 support for VDC; further consolidate chip specific
 * ifdefs to bvdc_common_priv.h;
 *
 * Hydra_Software_Devel/84   12/18/06 11:19p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   11/22/06 3:51p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   11/16/06 1:54p hongtaoz
 * PR25668: merge in vdc support for 7403;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   10/23/06 2:44p tdo
 * PR 23384: Increase MAX_ENTRY_PER_RUL to acomodate 10-bits table size
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 6:23p tdo
 * PR 23379, PR 23380, PR 23382, PR 23384: Increase MAX entries per RUL to
 * handle PEP table writes
 *
 * Hydra_Software_Devel/82   8/29/06 11:00a hongtaoz
 * PR23246: add new QDAC support for 3563; centralize some chip-specific
 * display macroes into bvdc_common_priv.h;
 *
 * Hydra_Software_Devel/81   8/22/06 5:36p pntruong
 * PR23674: Support CSC auto detection in HDMI.  Added supporting
 * RGB/YCrCb444/YCrCb422 input colorspace.  Thru eYCbCrType and
 * eMatrixCoefficients in BAVC_VDC_HdDvi_Picture.
 *
 * Hydra_Software_Devel/80   8/18/06 4:24p albertl
 * PR23117:  Added 7440 support.
 *
 * Hydra_Software_Devel/79   8/14/06 1:54p tdo
 * PR 23456: Moving large array of local variables into handle context
 *
 * Hydra_Software_Devel/78   8/7/06 5:32p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/77   7/13/06 10:56a jessem
 * PR 22389: Added BVDC_P_MapSrcFrameRateToVerticalRefreshRate_isr.
 *
 * Hydra_Software_Devel/76   6/15/06 4:45p hongtaoz
 * PR20716, PR21804: support individual mosaic rectangles visibility;
 *
 * Hydra_Software_Devel/75   6/14/06 1:42p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/74   5/10/06 3:26p jessem
 * PR 17530: Added BTMR_Handle and BTMR_TimerHandle to BVDC_P_Context.
 *
 * Hydra_Software_Devel/73   3/7/06 4:03p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/72   2/9/06 4:15p pntruong
 * PR19270: HDCP glitches at wrap of Sarnoff 1080i DYNPICTS.TRP stream.
 * Update hdmi rate manager in vertical blanking to avoid glitches.
 *
 * Hydra_Software_Devel/70   1/17/06 4:19p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/69   1/12/06 1:33p hongtaoz
 * PR18233: added mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/1   12/8/05 6:50p hongtaoz
 * PR18233: add Mosaic mode support (single window working);
 *
 * Hydra_Software_Devel/68   9/20/05 4:39p pntruong
 * PR17153: Request to implement a full SCART solution for European
 * market. Added flag to control vec swap.
 *
 * Hydra_Software_Devel/67   8/18/05 1:18p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/2   8/17/05 5:01p pntruong
 * PR12519: Take in additional fixes from mainline, no need to use crc for
 * drain use vec takeover.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/16/05 2:01p pntruong
 * PR12519: Take in additional fixes from mainline.
 *
 * Hydra_Software_Devel/XVDPhase1/4   5/26/05 5:49p pntruong
 * PR15321, PR15510, PR15560, PR15563: Additional hd_dvi channel change
 * work.
 *
 * Hydra_Software_Devel/XVDPhase1/3   5/6/05 4:00p pntruong
 * PR13121, PR12519: Added support for xvd hd/sd simul mode.
 *
 * Hydra_Software_Devel/XVDPhase1/2   5/3/05 8:34p pntruong
 * PR15084: Additional work on hddvi input format change.
 *
 * Hydra_Software_Devel/XVDPhase1/1   5/2/05 4:54p pntruong
 * PR12519, PR13121, PR15048, PR15084, PR15100: Dynamically re-allocate
 * capture in bvn path to reduce memory consumption and bandwith.
 *
 * Hydra_Software_Devel/1   5/1/05 4:55p pntruong
 * XVDPhase1 branch off.  Temp storage so we can keep track of changes.
 *
 * Hydra_Software_Devel/65   4/8/05 3:38p pntruong
 * PR14018, PR14011, PR14450, PR14648:  Vec to generate fake trigger when
 * source is pulled or not configured correctly or any condition that
 * cause source to lose trigger.  This vec's fake trigger enable the
 * applychanges to go thru, and does not result in timeout.
 *
 * Hydra_Software_Devel/64   3/21/05 3:32p pntruong
 * PR14494: Added reset for bvn blocks at startup.
 *
 * Hydra_Software_Devel/63   3/18/05 6:30p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/62   3/17/05 6:40p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/61   2/22/05 12:53p pntruong
 * PR14046: While in mirror mode with PIP, VDC times out when toggling
 * between analog and digital channel.
 *
 * Hydra_Software_Devel/60   1/26/05 4:53p pntruong
 * PR13450, PR12854, PR13549, PR13617, PR13618, PR13683, PR13321, PR13646,
 * PR13447, PR13429: Disabled vec triggers when vec reset (e.g. format
 * change) to prevent it from continuosly executing same register update
 * list that cause system locked up and/or causing vec errors.
 *
 * Hydra_Software_Devel/59   12/10/04 2:57p pntruong
 * PR1344: Added initial C0 Support for VDC.
 *
 ***************************************************************************/
#ifndef BVDC_PRIV_H__
#define BVDC_PRIV_H__

#include "bvdc.h"
#include "bkni.h"
#include "bvdc_dbg.h"
#include "bchp_common.h"
#include "bvdc_common_priv.h"
#include "bvdc_resource_priv.h"
#include "bchp_misc.h"
#include "btmr.h"

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BVDC_VDC);
BDBG_OBJECT_ID_DECLARE(BVDC_SRC);
BDBG_OBJECT_ID_DECLARE(BVDC_CMP);
BDBG_OBJECT_ID_DECLARE(BVDC_WIN);
BDBG_OBJECT_ID_DECLARE(BVDC_DSP);

/***************************************************************************
Summary:
	List of BVN errors.

Description:
	This is the enumerated list of BVN module errors.

See Also:
****************************************************************************/
typedef enum
{
	BVDC_BvnError_eRdc,

	BVDC_BvnError_eMfd_0,
	BVDC_BvnError_eMfd_1,
	BVDC_BvnError_eMfd_2,
	BVDC_BvnError_eMfd_3,
	BVDC_BvnError_eMfd_4,
	BVDC_BvnError_eMfd_5,

	BVDC_BvnError_eVfd_0,
	BVDC_BvnError_eVfd_1,
	BVDC_BvnError_eVfd_2,
	BVDC_BvnError_eVfd_3,
	BVDC_BvnError_eVfd_4,
	BVDC_BvnError_eVfd_5,
	BVDC_BvnError_eVfd_6,
	BVDC_BvnError_eVfd_7,

	BVDC_BvnError_eScl_0,
	BVDC_BvnError_eScl_1,
	BVDC_BvnError_eScl_2,
	BVDC_BvnError_eScl_3,
	BVDC_BvnError_eScl_4,
	BVDC_BvnError_eScl_5,
	BVDC_BvnError_eScl_6,
	BVDC_BvnError_eScl_7,

	BVDC_BvnError_eDnr_0,
	BVDC_BvnError_eDnr_1,
	BVDC_BvnError_eDnr_2,
	BVDC_BvnError_eDnr_3,
	BVDC_BvnError_eDnr_4,

	BVDC_BvnError_eMad_0,

	BVDC_BvnError_eMvp_0,
	BVDC_BvnError_eMvp_1,
	BVDC_BvnError_eMvp_2,
	BVDC_BvnError_eMvp_3,
	BVDC_BvnError_eMvp_4,

	BVDC_BvnError_eMcdi_0,
	BVDC_BvnError_eMcdi_1,
	BVDC_BvnError_eMcdi_2,
	BVDC_BvnError_eMcdi_3,
	BVDC_BvnError_eMcdi_4,

	BVDC_BvnError_eMctf_0,

	BVDC_BvnError_eHscl_0,
	BVDC_BvnError_eHscl_1,
	BVDC_BvnError_eHscl_2,
	BVDC_BvnError_eHscl_3,
	BVDC_BvnError_eHscl_4,

	BVDC_BvnError_eCap_0,
	BVDC_BvnError_eCap_1,
	BVDC_BvnError_eCap_2,
	BVDC_BvnError_eCap_3,
	BVDC_BvnError_eCap_4,
	BVDC_BvnError_eCap_5,
	BVDC_BvnError_eCap_6,
	BVDC_BvnError_eCap_7,

	BVDC_BvnError_eGfd_0,
	BVDC_BvnError_eGfd_1,
	BVDC_BvnError_eGfd_2,
	BVDC_BvnError_eGfd_3,
	BVDC_BvnError_eGfd_4,
	BVDC_BvnError_eGfd_5,
	BVDC_BvnError_eGfd_6,

	BVDC_BvnError_eCmp_0_V0,
	BVDC_BvnError_eCmp_1_V0,
	BVDC_BvnError_eCmp_2_V0,
	BVDC_BvnError_eCmp_3_V0,
	BVDC_BvnError_eCmp_4_V0,
	BVDC_BvnError_eCmp_5_V0,
	BVDC_BvnError_eCmp_6_V0,

	BVDC_BvnError_eCmp_0_V1,
	BVDC_BvnError_eCmp_1_V1,

	BVDC_BvnError_eCmp_0_G0,
	BVDC_BvnError_eCmp_1_G0,
	BVDC_BvnError_eCmp_2_G0,
	BVDC_BvnError_eCmp_3_G0,
	BVDC_BvnError_eCmp_4_G0,
	BVDC_BvnError_eCmp_5_G0,
	BVDC_BvnError_eCmp_6_G0,

	BVDC_BvnError_eMaxCount,

	BVDC_BvnError_eInvalid = BVDC_BvnError_eMaxCount      /* must be last */
} BVDC_BvnError;
/* B0 does not support Letterbox Detection and VBI Pass through together */
#define B0_NO_LETTERBOX_DETECTION_AND_VBI_PASS_THROUGH_COMBO (1)

/* */
#define BVDC_P_SUPPORT_TDAC_VER_0                            (0) /* 3563 */
#define BVDC_P_SUPPORT_TDAC_VER_1                            (1) /* 7401, 7403, 7118 */
#define BVDC_P_SUPPORT_TDAC_VER_2                            (2) /* 7400 */
#define BVDC_P_SUPPORT_TDAC_VER_3                            (3) /* 7405A0, 7325 */
#define BVDC_P_SUPPORT_TDAC_VER_4                            (4) /* 7405Bx, 7335 */
#define BVDC_P_SUPPORT_TDAC_VER_5                            (5) /* 3548, 3556 */
#define BVDC_P_SUPPORT_TDAC_VER_6                            (6) /* 7420 */
#define BVDC_P_SUPPORT_TDAC_VER_7                            (7) /* 7340, 7342 */
#define BVDC_P_SUPPORT_TDAC_VER_8                            (8) /* 7550: no DAC_BG_CTRL_1 */
#define BVDC_P_SUPPORT_TDAC_VER_9                            (9) /* 7422: add DAC detection */
#define BVDC_P_SUPPORT_TDAC_VER_10                          (10) /* 7360: add recalibrate bit */
#define BVDC_P_SUPPORT_TDAC_VER_11                          (11) /* 7425 B0: new DAC grouping, DAC detection logic not working */
#define BVDC_P_SUPPORT_TDAC_VER_12                          (12) /* DAC detection logic working */
#define BVDC_P_SUPPORT_TDAC_VER_13                          (13) /* 7445 */

#define BVDC_P_SUPPORT_QDAC_VER_0                            (0)
#define BVDC_P_SUPPORT_QDAC_VER_1                            (1) /* 7400, 3563 */

/* Display cracking macros. */
#define BVDC_P_DISP_GET_REG_IDX(reg) \
	((BCHP##_##reg - BCHP_MISC_REG_START) / sizeof(uint32_t))

/* Get/Set reg data */
#define BVDC_P_VDC_GET_MISC_REG_DATA(reg) \
	(hVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(reg)])

#define BVDC_P_DISP_GET_REG_DATA(reg) \
	(hDisplay->hVdc->aulMiscRegs[BVDC_P_DISP_GET_REG_IDX(reg)])
#define BVDC_P_DISP_SET_REG_DATA(reg, data) \
	(BVDC_P_DISP_GET_REG_DATA(reg) = (uint32_t)(data))

/* Get field */
#define BVDC_P_DISP_GET_FIELD_NAME(reg, field) \
	(BVDC_P_GET_FIELD(BVDC_P_DISP_GET_REG_DATA(reg), reg, field))

/* Compare field */
#define BVDC_P_DISP_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_COMPARE_FIELD_DATA(BVDC_P_DISP_GET_REG_DATA(reg), reg, field, (data)))
#define BVDC_P_DISP_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_COMPARE_FIELD_NAME(BVDC_P_DISP_GET_REG_DATA(reg), reg, field, name))

/* number of registers in one block. */
#define BVDC_P_DISP_REGS_COUNT \
	BVDC_P_REGS_ENTRIES(MISC_REG_START, MISC_REG_END)

/* This macro does a write into RUL (write, addr, data). 3 dwords. */
#define BVDC_P_DISP_WRITE_TO_RUL(reg, addr_ptr) \
do { \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BCHP##_##reg); \
	*addr_ptr++ = BVDC_P_DISP_GET_REG_DATA(reg); \
} while(0)

/* Max length for BVN error msg */
#define BVDC_P_ERROR_MAX_MSG_LENGTH    (256)

#define BVDC_P_CLIPRECT_PERCENT        (10000)

#ifndef BVDC_UINT32_ONLY
#define uintAR_t                       uint64_t
#define BVDC_P_ASPR_FRAC_BITS_NUM      (40)
#else
#define uintAR_t                       uint32_t
	/* for well bounded value such as pixel and full aspect ratio value */
#define BVDC_P_ASPR_FRAC_BITS_NUM      (16)
#endif

/***************************************************************************
 * VDC Internal data structures
 ***************************************************************************/
/* Table for interrupt name and callback. */
typedef struct
{
	BVDC_BvnError                      eBvnError;
	BINT_Id                            ErrIntId;
	uint32_t                           ulL2ClearReg;
	uint32_t                           ulL2ClearMask;
	BINT_CallbackFunc                  pfCallback;
	const char                        *pchInterruptName;
} BVDC_P_IntCbTbl;

typedef struct BVDC_P_Context
{
	BDBG_OBJECT(BVDC_VDC)

	/* public fields */
	BVDC_Settings                  stSettings;

	/* handed down from app. */
	BCHP_Handle                    hChip;
	BREG_Handle                    hRegister;
	BMEM_Heap_Handle               hMemory;
	BINT_Handle                    hInterrupt;
	BRDC_Handle                    hRdc;
	BTMR_Handle                    hTmr;

	/* Created during BVDC_Open via a call to BVDC_P_InitTimer */
	BTMR_TimerHandle               hTimer;

	/* These handle get defer allocation until used by app. */
	BVDC_Source_Handle             ahSource[BVDC_P_MAX_SOURCE_COUNT];
	BVDC_Display_Handle            ahDisplay[BVDC_P_MAX_DISPLAY_COUNT];
	BVDC_Compositor_Handle         ahCompositor[BVDC_P_MAX_COMPOSITOR_COUNT];

	BVDC_P_BufferHeap_Handle       hBufferHeap;

	/* Allocated hardware resources */
	const BVDC_P_Features         *pFeatures;
	BVDC_P_Resource_Handle         hResource;

	/* Swap compositor/vec!  cmp_0 -> prim vs. cmp_0 -> sec */
	bool                           bSwapVec;

	/* Store other var that is global to VDC here.  If it's has window,
	 * compositor, display, or source scope store it in respectives context. */
	BVDC_Compositor_Handle         hCmpCheckSource;

	/* Misc register (VEC's top-level registers) */
	uint32_t                       aulMiscRegs[BVDC_P_DISP_REGS_COUNT];
	uint32_t                       ulInsideCs;

	BINT_CallbackHandle            ahBvnErrHandlerCb[BVDC_BvnError_eMaxCount];
	uint32_t                       aulBvnErrCnt[BVDC_BvnError_eMaxCount];
	bool                           abBvnErrMask[BVDC_BvnError_eMaxCount];

	/* HD_DVI register shared between BVDC_P_HdDviId_eHdDvi0 and
	 * BVDC_P_HdDviId_eHdDvi1. Need to keep track of it here to
	 * work around the RDC read/modify/write problem */
	uint32_t                       ulHdDviBvbReg;
	uint32_t                       ulHdDviChMapReg;

#if BVDC_P_SUPPORT_MOSAIC_MODE
	/* capture drain buffer for mosaic mode, shared by all captures; */
	void                          *pvVdcNullBufAddr;
	uint32_t                       ulVdcNullBufOffset;
#endif

	/* FGT Noise Pixel Pattern Table */
	uint8_t                       *pucFgtPatTblAddr;

	/* Store the generic BVN error msg */
	char                           achBuf[BVDC_P_ERROR_MAX_MSG_LENGTH];
	uint32_t                       ulApplyCnt;
	bool                           bForcePrint;

#if BVDC_P_ORTHOGONAL_VEC
	BVDC_DacOutput                 aDacOutput[BVDC_P_MAX_DACS];
	uint32_t                       aDacDisplay[BVDC_P_MAX_DACS];
	bool                           bCalibrated;
	bool                           bDacDetectionEnable;
	const uint32_t                *aulDacGrouping;
#endif

	/* Standby state */
	bool                           bStandby;

} BVDC_P_Context;


/***************************************************************************
 * VDC private functions
 ***************************************************************************/
void BVDC_P_CompositorDisplay_isr
	( void                            *pvParam1,
	  int                              iParam2 );

void BVDC_P_BuildNoOpsRul_isr
	( BRDC_List_Handle                 hList );

void BVDC_P_ReadListInfo_isr
	( BVDC_P_ListInfo                 *pList,
	  BRDC_List_Handle                 hList );

void BVDC_P_WriteListInfo_isr
	( const BVDC_P_ListInfo           *pList,
	  BRDC_List_Handle                 hList );

void BVDC_P_Dither_Init
	( BVDC_P_DitherSetting            *pDitherSetting,
	  uint32_t                         ulLfsrCtrlT0,
	  uint32_t                         ulLfsrCtrlT1,
	  uint32_t                         ulLfsrCtrlT2,
	  uint32_t                         ulLfsrValue );

void BVDC_P_BvnErrorHandler_isr
	( void                            *pvhVdc,
	  int                              iIdx );

const BVDC_P_IntCbTbl *BVDC_P_GetBvnErrorCb
	( BVDC_BvnError                    eBvnErrId );

void BVDC_P_CalculateRect_isr
	( const BVDC_ClipRect             *pClipRect,
	  uint32_t                         ulWidth,
	  uint32_t                         ulHeight,
	  bool                             bInterlaced,
	  BVDC_P_Rect                     *pRect );

bool BVDC_P_CbIsDirty
	(void                           *pDirty,
	 uint32_t                        ulSize );

void BVDC_P_CalcuPixelAspectRatio_isr(
	BFMT_AspectRatio				 eFullAspectRatio,	   /* full asp ratio enum */
	uint32_t						 ulSampleAspectRatioX, /* width of one sampled src pixel */
	uint32_t						 ulSampleAspectRatioY, /* height of one sampled src pixel */
	uint32_t						 ulFullWidth,		   /* full asp ratio width */
	uint32_t                         ulFullHeight,         /* full asp ratio height */
	const BVDC_P_ClipRect*           pAspRatCnvsClip,	   /* asp rat cnvs clip */
	uintAR_t *						 pulPxlAspRatio,       /* PxlAspR_int.PxlAspR_frac */
	uint32_t *                       pulPxlAspRatio_x_y);  /* PxlAspR_x<<16 | PxlAspR_y */

bool  BVDC_P_IsPxlfmtSupported
	(BPXL_Format                       ePxlFmt);

bool  BVDC_P_IsVidfmtSupported
	( BFMT_VideoFmt                    eVideoFmt);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_PRIV_H__ */
/* End of file. */
