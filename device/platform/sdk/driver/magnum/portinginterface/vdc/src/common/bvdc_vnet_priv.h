/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_vnet_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/82 $
 * $brcm_Date: 8/17/12 2:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_vnet_priv.h $
 * 
 * Hydra_Software_Devel/82   8/17/12 2:06p tdo
 * SW7445-8: Fix window timeout error due to MFD post mux value calculated
 * wrong
 *
 * Hydra_Software_Devel/81   7/13/12 11:24a tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/80   4/27/12 4:02p tdo
 * SW7584-21: Add PI/vdc support for 7584
 *
 * Hydra_Software_Devel/79   11/10/11 9:18p vanessah
 * SW7425-1115: add MCVP/MADR seamless transaction
 *
 * Hydra_Software_Devel/78   10/20/11 6:51p vanessah
 * SW7425-1572: add DNR-2 support
 *
 * Hydra_Software_Devel/77   10/20/11 2:22a hongtaoz
 * SW7425-1549: added two more loopback vnet paths for B0;
 *
 * Hydra_Software_Devel/76   12/16/10 4:02p yuxiaz
 * SW7422-35: Added new DRAIN HW version. Fixed build error on 7358, 7552,
 * 7231, 7346, 7344.
 *
 * Hydra_Software_Devel/75   11/15/10 6:05p pntruong
 * SW7231-2: Initial support to make 7231 compile.
 *
 * Hydra_Software_Devel/74   10/12/10 6:19p hongtaoz
 * SW7425-30: added scaler_4 and MFD_2 to BVN path;
 *
 * Hydra_Software_Devel/73   5/7/10 7:21p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/72   4/19/10 10:24p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/71   4/7/10 11:37a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/70   4/5/10 4:17p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/69   3/19/09 11:36p pntruong
 * PR53429: Corrected the anr/mad/hscl vnet connection.  Removed ununsed
 * bAnrMadCombo flags.
 *
 * Hydra_Software_Devel/68   3/16/09 10:51p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:32p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/67   10/23/08 9:41p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/66   9/5/08 4:46p tdo
 * PR46484: Bringup appframework for7420
 *
 * Hydra_Software_Devel/65   6/17/08 5:28p tdo
 * PR39333: Connect HIST at reader side for VDEC source as a work around
 *
 * Hydra_Software_Devel/64   6/11/08 4:24p hongtaoz
 * PR43460: add HSCL for ANR or MAD usage mode; HSCL seems to shield BVB
 * error going to ANR;
 *
 * Hydra_Software_Devel/63   4/11/08 2:11p tdo
 * PR39333: Properly handle Hist in vnet
 *
 * Hydra_Software_Devel/62   3/21/08 12:10p tdo
 * PR39333: More implementation of HIST block
 *
 * Hydra_Software_Devel/61   2/28/08 6:23p hongtaoz
 * PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
 * fixed non-linear scaling when MAD is on;
 *
 * Hydra_Software_Devel/60   2/21/08 5:02p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/59   2/20/08 4:15p hongtaoz
 * PR39154: added window API to toggle ANR; prepare for combo ANR/MAD
 * feature on 3548;
 *
 * Hydra_Software_Devel/58   2/15/08 1:31p pntruong
 * PR39244: Removed unused code to improve readability.
 *
 * Hydra_Software_Devel/57   10/23/07 2:05p pntruong
 * PR29569, PR36290: Fixed 7401/7403 build errors.
 *
 * Hydra_Software_Devel/56   10/23/07 11:17a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/1   10/12/07 10:49a yuxiaz
 * PR29569: Add FGT support on 7405
 *
 * Hydra_Software_Devel/55   4/4/07 3:46p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 * Hydra_Software_Devel/54   4/2/07 11:32a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/53   2/12/07 1:51p pntruong
 * PR27821:  Remove unused lagacy and work-around code (7038) from
 * mainline.
 *
 * Hydra_Software_Devel/52   1/11/07 7:04p syang
 * PR 22569: 1st time check in after anr building vnet correctly and not
 * hang the sys. But display is still static
 *
 * Hydra_Software_Devel/51   1/10/07 4:49p yuxiaz
 * PR 22569: Fix compile error on chipset without ANR.
 *
 * Hydra_Software_Devel/50   1/10/07 3:31p syang
 * PR 22569:  check in old vnet code that support one anr
 *
 * Hydra_Software_Devel/49   1/5/07 4:26p syang
 * PR 22569: almost completed coding for new vnet build/shutdown algorithm
 * (by each sub-modules)
 *
 * Hydra_Software_Devel/48   1/3/07 3:36p yuxiaz
 * PR 25963: Fixed compile error for vnet.
 *
 * Hydra_Software_Devel/47   12/22/06 11:57a syang
 * PR 22569: add anr related vnetMode
 *
 * Hydra_Software_Devel/46   12/18/06 11:31p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   12/15/06 3:33p yuxiaz
 * PR26637: Clean up vnet tables.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   11/28/06 3:36p yuxiaz
 * PR23638: Rewrite vnet drain code by using shared resource manager.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   11/16/06 1:53p hongtaoz
 * PR25668: merge in vdc support for 7403;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/15/06 12:01p syang
 * PR 25097: change to use BVDC_P_SUPPORT_MFD to comply convension
 *
 * Hydra_Software_Devel/44   8/23/06 9:25a yuxiaz
 * PR23307, PR22801: Added Release drain in BVDC_Source_Destroy.
 *
 * Hydra_Software_Devel/43   8/7/06 5:38p pntruong
 * PR22577: Initial bringup of VDC.
 *
 * Hydra_Software_Devel/42   8/4/06 4:52p yuxiaz
 * PR23307: Share vnet drain between 656 and HD_DVI sources.
 *
 * Hydra_Software_Devel/41   5/26/06 3:44p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/39   2/22/06 12:14p hongtaoz
 * PR19082: added shared DNR support for 7400;
 *
 * Hydra_Software_Devel/38   1/31/06 5:20p pntruong
 * PR17778: Take in changes for new hddvi.
 *
 * Hydra_Software_Devel/34   10/31/05 6:26p hongtaoz
 * PR17514: add DNR support in VDC;
 *
 * Hydra_Software_Devel/33   9/6/05 7:02p hongtaoz
 * PR16812, PR15888: mute writer blocks and muxe earlier; window blocks
 * shutdown and normal programming order are reverse; not to re-init
 * window shared resource at create time;
 *
 * Hydra_Software_Devel/32   9/1/05 6:23p jasonh
 * PR 16967: Removing XVD source
 *
 * Hydra_Software_Devel/31   8/31/05 3:42p hongtaoz
 * PR16812, PR15495: assure MAD disconnect RUL got executed;
 *
 * Hydra_Software_Devel/30   8/22/05 5:25p hongtaoz
 * PR12192, PR15888: make MAD sharable between the two video windows of
 * main display; reduced unnecessary window shutdown initiation if it's
 * already in the middle of shutdown; build both slots RULs for analog
 * progressive source format during shutdown to workaround unexpected
 * trigger pattern;
 *
 * Hydra_Software_Devel/29   8/19/05 2:51p hongtaoz
 * PR15888: adding support for dynamic switch on/off shared scaler on
 * bypass window;
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/19/05 2:28p hongtaoz
 * PR15888: adding support for dynamic switch on/off of shared scaler with
 * Bypass window;
 *
 * Hydra_Software_Devel/28   8/4/05 4:58p yuxiaz
 * PR16182: Fixed vnet settings for 7401.
 *
 * Hydra_Software_Devel/27   6/28/05 11:00a jasonh
 * PR 16012: Adding support for 7401
 *
 * Hydra_Software_Devel/26   6/15/05 4:22p hongtaoz
 * PR14510, PR15163, PR15743, PR15785: vnet switch shall be carefully
 * programmed such that the vnet_f to be connected with non-feeder source
 * is always set at last to avoid garbage data being temporarily injected
 * into the BVN back switch;
 *
 * Hydra_Software_Devel/25   3/18/05 6:31p pntruong
 * PR14494: Basic vdc up and running.  Background and passed output format
 * switch test.
 *
 * Hydra_Software_Devel/24   3/17/05 6:41p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/23   1/25/05 2:17p yuxiaz
 * PR13569: Use CRC to drain HD_DVI data when XVD tells VDC to switch
 * format.
 *
 * Hydra_Software_Devel/22   1/21/05 9:55a yuxiaz
 * PR13733: Need to disable CRC after bringup HD_DVI to get duall HD_DVI
 * work.
 *
 * Hydra_Software_Devel/21   1/20/05 11:39a yuxiaz
 * PR13733: Use CRC to bring up HD_DVI.
 *
 ***************************************************************************/
#ifndef BVDC_VNET_PRIV_H__
#define BVDC_VNET_PRIV_H__

#include "bkni.h"
#include "bvdc.h"
#include "bchp_common.h"
#include "bvdc_common_priv.h"
#include "bvdc_resource_priv.h"
#include "bchp_vnet_f.h"
#include "bchp_vnet_b.h"

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(BVDC_RES);

/* VNET_F/VNET_B versioning! */
#define BVDC_P_VNET_VER_0                          (0) /* pre-3548 tapeout */
#define BVDC_P_VNET_VER_1                          (1) /* 3548x/3556x */
#define BVDC_P_VNET_VER_2                          (2) /* 7422x, 7425x */
#define BVDC_P_VNET_VER_3                          (3) /* 7358, 7552, 7231, 7346, 7344 */

/* Check if this mode uses the capture. */
#define BVDC_P_VNET_USED_CAPTURE(stVnetMode) \
    ((stVnetMode).stBits.bUseCap)

/* Check if this mode uses the playback. */
#define BVDC_P_VNET_USED_PLAYBACK(stVnetMode) \
    ((stVnetMode).stBits.bUseCap)

/* Check if this mode uses the scaler. */
#define BVDC_P_VNET_USED_SCALER(stVnetMode) \
    ((stVnetMode).stBits.bUseScl)

/* Check if this mode uses the scaler. */
#define BVDC_P_VNET_USED_SCALER_AT_READER(stVnetMode) \
    (((stVnetMode).stBits.bUseScl) && \
	 !(((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap)))

/* Check if this mode uses the scaler. */
#define BVDC_P_VNET_USED_SCALER_AT_WRITER(stVnetMode) \
    (((stVnetMode).stBits.bUseScl) && \
	 ((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap))

/* Check if this mode uses the mcvp. */
#define BVDC_P_VNET_USED_MCVP(stVnetMode) \
    ((stVnetMode).stBits.bUseMcvp)

/* Check if this mode uses the mcvp at reader. */
#define BVDC_P_VNET_USED_MCVP_AT_READER(stVnetMode) \
    (((stVnetMode).stBits.bUseMcvp) && \
	 !(((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap)))

/* Check if this mode uses the mcvp at writer. */
#define BVDC_P_VNET_USED_MCVP_AT_WRITER(stVnetMode) \
    (((stVnetMode).stBits.bUseMcvp) && \
	 ((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap))
/* Check if this mode bypass the mcvp. */
#define BVDC_P_VNET_BYPASS_MCVP(stVnetMode) \
    ((stVnetMode).stBits.bUseMvpBypass)
/* Check if this mode uses the mcvp Bypass mode at reader. */
#define BVDC_P_VNET_BYPASS_MCVP_AT_READER(stVnetMode) \
    (((stVnetMode).stBits.bUseMvpBypass) && \
	 !(((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap)))
/* Check if this mode uses the mcvp at writer. */
#define BVDC_P_VNET_BYPASS_MCVP_AT_WRITER(stVnetMode) \
    (((stVnetMode).stBits.bUseMvpBypass) && \
	 ((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap))
/* Check if this mode uses the mad32. */
#define BVDC_P_VNET_USED_MAD(stVnetMode) \
    ((stVnetMode).stBits.bUseMad)

/* Check if this mode uses the mad32 at reader. */
#define BVDC_P_VNET_USED_MAD_AT_READER(stVnetMode) \
    (((stVnetMode).stBits.bUseMad) && \
	 !(((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap)))

/* Check if this mode uses the mad32 at writer. */
#define BVDC_P_VNET_USED_MAD_AT_WRITER(stVnetMode) \
    (((stVnetMode).stBits.bUseMad) && \
	 ((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap))

/* Check if this mode uses the HSCL (tied to MAD usage). */
#define BVDC_P_VNET_USED_HSCL(stVnetMode) \
    ((stVnetMode).stBits.bUseHscl)

/* Check if this mode uses the HSCL at reader. */
#define BVDC_P_VNET_USED_HSCL_AT_READER(stVnetMode) \
    (((stVnetMode).stBits.bUseHscl) && \
	 !(((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap)))

/* Check if this mode uses the HSCL at writer. */
#define BVDC_P_VNET_USED_HSCL_AT_WRITER(stVnetMode) \
    (((stVnetMode).stBits.bUseHscl) && \
	 ((stVnetMode).stBits.bUseCap) && ((stVnetMode).stBits.bSclBeforeCap))

/* Check if this mode uses the DNR. */
#define BVDC_P_VNET_USED_DNR(stVnetMode) \
    ((stVnetMode).stBits.bUseDnr)

/* Check if this mode uses the DNR at reader. */
#define BVDC_P_VNET_USED_DNR_AT_READER(stVnetMode) \
    (((stVnetMode).stBits.bUseDnr) && \
     !((stVnetMode).stBits.bUseCap))

/* Check if this mode uses the DNR at writer. */
#define BVDC_P_VNET_USED_DNR_AT_WRITER(stVnetMode) \
    (((stVnetMode).stBits.bUseDnr) && \
	 ((stVnetMode).stBits.bUseCap))

/* Check if this mode uses the ANR. */
#define BVDC_P_VNET_USED_ANR(stVnetMode) \
    ((stVnetMode).stBits.bUseAnr)

/* Check if this mode uses the ANR at reader. */
#define BVDC_P_VNET_USED_ANR_AT_READER(stVnetMode) \
    (((stVnetMode).stBits.bUseAnr) && \
	!(((stVnetMode).stBits.bUseCap) && \
	  ((stVnetMode).stBits.bSclBeforeCap)))

/* Check if this mode uses the ANR at writer. */
#define BVDC_P_VNET_USED_ANR_AT_WRITER(stVnetMode) \
    (((stVnetMode).stBits.bUseAnr) && \
	 ((stVnetMode).stBits.bUseCap) && \
	 ((stVnetMode).stBits.bSclBeforeCap))

/* Check if this mode uses the FGT. */
#define BVDC_P_VNET_USED_FGT(stVnetMode) \
    ((stVnetMode).stBits.bUseFgt)

/* FGT is at reader for Post-Process Mode */
/* Check if this mode uses the FGT at reader. */
#define BVDC_P_VNET_USED_FGT_AT_READER(stVnetMode) \
    ((stVnetMode).stBits.bUseFgt)

/* FGT is at reader for Post-Process Mode */
/* Check if this mode uses the FGT at writer. */
#define BVDC_P_VNET_USED_FGT_AT_WRITER(stVnetMode)  (0)

/* Check if this mode uses the ANR. */
#define BVDC_P_VNET_USED_HIST(stVnetMode) \
    ((stVnetMode).stBits.bUseHist)

/* HIST is at writer */
/* Check if this mode uses the HIST at reader */
#define BVDC_P_VNET_USED_HIST_AT_READER(stVnetMode)  \
    ((stVnetMode).stBits.bUseHist && !(stVnetMode).stBits.bUseHistAtSrc)

/* HIST is at writer */
/* Check if this mode uses the HIST at writer */
#define BVDC_P_VNET_USED_HIST_AT_WRITER(stVnetMode) \
    ((stVnetMode).stBits.bUseHist && (stVnetMode).stBits.bUseHistAtSrc)

/* Check if this mode is invalid */
#define BVDC_P_VNET_IS_INVALID(stVnetMode) \
    ((stVnetMode).stBits.bInvalid)

/* Make things look cleaner */
#define BVDC_P_VNET_F(vnet_f_enum) \
	BCHP_VNET_F_SCL_0_SRC_SOURCE_##vnet_f_enum
#define BVDC_P_VNET_B(vnet_b_enum) \
	BCHP_VNET_B_CAP_0_SRC_SOURCE_##vnet_b_enum

#define BVDC_P_DRN_INVALID_OFFSET                   UINT32_C(-1)
#define BVDC_P_DRN_HAS_DEBUG(pDrain) \
	(BVDC_P_DRN_INVALID_OFFSET != pDrain->ulDbgOffset)

#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_2
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_2 BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_3
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_3 BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_4
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_4 BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_5
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_5 BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_6
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_6 BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_7
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Video_Feeder_7 BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif

#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_HD_DVI_0
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_HD_DVI_0 BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_HD_DVI_1
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_HD_DVI_1 BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif

/* If there isn't _1 make it equal to _0 */
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656_1
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656_1 BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656_0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_1
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_1 BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_0
#endif

#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_1
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_1 BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_2
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_2 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_3
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_3 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_4
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_4 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_5
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_5 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_6
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_6 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_7
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_7 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_8
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_8 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_9
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_9 0
#endif

#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_1
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_1 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_2
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_2 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_3
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_3 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_4
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_4 0
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_5
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_MPEG_Feeder_5 0
#endif

/* Make the 0 equal to _0 */
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656 BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656_0
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656_0 BCHP_VNET_F_SCL_0_SRC_SOURCE_CCIR_656
#endif

#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC BCHP_VNET_F_SCL_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_0
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_0 BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC
#endif
#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_1
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC_1 BCHP_VNET_F_SCL_0_SRC_SOURCE_VDEC
#endif

#ifndef BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_0
#define BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback_0 BCHP_VNET_F_SCL_0_SRC_SOURCE_Loopback
#endif
#ifndef BCHP_VNET_B_LOOPBACK_SRC
#define BCHP_VNET_B_LOOPBACK_SRC BCHP_VNET_B_LOOPBACK_0_SRC
#endif

#ifndef BCHP_VNET_F_MAD_0_SRC
#define BCHP_VNET_F_MAD_0_SRC BCHP_VNET_F_MAD32_SRC
#endif

#ifndef BCHP_VNET_F_MAD_0_SRC_SOURCE_Output_Disabled
#define BCHP_VNET_F_MAD_0_SRC_SOURCE_MASK  BCHP_VNET_F_MAD32_SRC_SOURCE_MASK
#define BCHP_VNET_F_MAD_0_SRC_SOURCE_SHIFT BCHP_VNET_F_MAD32_SRC_SOURCE_SHIFT
#define BCHP_VNET_F_MAD_0_SRC_SOURCE_Output_Disabled BCHP_VNET_F_MAD32_SRC_SOURCE_Output_Disabled
#endif

#ifndef BCHP_VNET_F_DNR_0_SRC
#define BCHP_VNET_F_DNR_0_SRC BCHP_VNET_F_DNR_SRC
#endif

#ifndef BCHP_VNET_F_DNR_0_SRC_SOURCE_Output_Disabled
#define BCHP_VNET_F_DNR_0_SRC_SOURCE_MASK  BCHP_VNET_F_DNR_SRC_SOURCE_MASK
#define BCHP_VNET_F_DNR_0_SRC_SOURCE_SHIFT BCHP_VNET_F_DNR_SRC_SOURCE_SHIFT
#define BCHP_VNET_F_DNR_0_SRC_SOURCE_Output_Disabled BCHP_VNET_F_DNR_SRC_SOURCE_Output_Disabled
#endif

#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD32
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD32 0
#endif

#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD_0
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD_0 BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD32
#endif

#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD_1
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_MAD_1 0
#endif

#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_0
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_0 BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_1
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_1 0
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_2
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_2 0
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_3
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_3 0
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_4
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_DNR_4 0
#endif

#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_ANR_0
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_ANR_0 0
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_ANR_1
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_ANR_1 0
#endif

#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_2
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_2 0
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_3
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_3 0
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_4
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_4 0
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_5
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_5 0
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_6
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_6 0
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_7
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_7 0
#endif

#ifndef BCHP_VNET_B_DRAIN_0_SRC_SOURCE_MAD_0
#define BCHP_VNET_B_DRAIN_0_SRC_SOURCE_MAD_0 BCHP_VNET_B_DRAIN_0_SRC_SOURCE_MAD32
#endif

#ifndef BCHP_VNET_B_DRAIN_0_SRC_SOURCE_DNR_0
#define BCHP_VNET_B_DRAIN_0_SRC_SOURCE_DNR_0 BCHP_VNET_B_DRAIN_0_SRC_SOURCE_DNR
#endif

#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_FGT_0
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_FGT_0 0
#endif

#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_3
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_3 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_4
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_4 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_5
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_5 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_6
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_6 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_7
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_7 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_8
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_8 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_9
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_9 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_10
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_10 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_11
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_11 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_12
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_12 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_13
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_Free_Ch_13 BCHP_VNET_B_CAP_0_SRC_SOURCE_Output_Disabled
#endif

/* Enumeration that defines the front and back muxes. */
typedef enum
{
	BVDC_P_VnetF_eMpeg_0    = BVDC_P_VNET_F(MPEG_Feeder_0),
	BVDC_P_VnetF_eMpeg_1    = BVDC_P_VNET_F(MPEG_Feeder_1),
	BVDC_P_VnetF_eMpeg_2    = BVDC_P_VNET_F(MPEG_Feeder_2),
	BVDC_P_VnetF_eMpeg_3    = BVDC_P_VNET_F(MPEG_Feeder_3),
	BVDC_P_VnetF_eMpeg_4    = BVDC_P_VNET_F(MPEG_Feeder_4),
	BVDC_P_VnetF_eMpeg_5    = BVDC_P_VNET_F(MPEG_Feeder_5),
	BVDC_P_VnetF_eVideo_0   = BVDC_P_VNET_F(Video_Feeder_0),
	BVDC_P_VnetF_eVideo_1   = BVDC_P_VNET_F(Video_Feeder_1),
	BVDC_P_VnetF_eVideo_2   = BVDC_P_VNET_F(Video_Feeder_2),
	BVDC_P_VnetF_eVideo_3   = BVDC_P_VNET_F(Video_Feeder_3),
	BVDC_P_VnetF_eVideo_4   = BVDC_P_VNET_F(Video_Feeder_4),
	BVDC_P_VnetF_eVideo_5   = BVDC_P_VNET_F(Video_Feeder_5),
	BVDC_P_VnetF_eVideo_6   = BVDC_P_VNET_F(Video_Feeder_6),
	BVDC_P_VnetF_eVideo_7   = BVDC_P_VNET_F(Video_Feeder_7),
	BVDC_P_VnetF_eHdDvi_0   = BVDC_P_VNET_F(HD_DVI_0),
	BVDC_P_VnetF_eHdDvi_1   = BVDC_P_VNET_F(HD_DVI_1),
	BVDC_P_VnetF_eCcir656_0 = BVDC_P_VNET_F(CCIR_656_0),
	BVDC_P_VnetF_eCcir656_1 = BVDC_P_VNET_F(CCIR_656_1),
	BVDC_P_VnetF_eAnalog_0  = BVDC_P_VNET_F(VDEC_0),
	BVDC_P_VnetF_eAnalog_1  = BVDC_P_VNET_F(VDEC_1),
	BVDC_P_VnetF_eLoopback_0  = BVDC_P_VNET_F(Loopback_0),
	BVDC_P_VnetF_eLoopback_1  = BVDC_P_VNET_F(Loopback_1),
	BVDC_P_VnetF_eLoopback_2  = BVDC_P_VNET_F(Loopback_2),
	BVDC_P_VnetF_eLoopback_3  = BVDC_P_VNET_F(Loopback_3),
	BVDC_P_VnetF_eLoopback_4  = BVDC_P_VNET_F(Loopback_4),
	BVDC_P_VnetF_eLoopback_5  = BVDC_P_VNET_F(Loopback_5),
	BVDC_P_VnetF_eLoopback_6  = BVDC_P_VNET_F(Loopback_6),
	BVDC_P_VnetF_eLoopback_7  = BVDC_P_VNET_F(Loopback_7),
	BVDC_P_VnetF_eLoopback_8  = BVDC_P_VNET_F(Loopback_8),
	BVDC_P_VnetF_eLoopback_9  = BVDC_P_VNET_F(Loopback_9),
	BVDC_P_VnetF_eDisabled  = BVDC_P_VNET_F(Output_Disabled),
	BVDC_P_VnetF_eInvalid   = -1
} BVDC_P_VnetF;

typedef enum
{
	BVDC_P_VnetB_eScaler_0  = BVDC_P_VNET_B(Scaler_0),
	BVDC_P_VnetB_eScaler_1  = BVDC_P_VNET_B(Scaler_1),
	BVDC_P_VnetB_eScaler_2  = BVDC_P_VNET_B(Scaler_2),
	BVDC_P_VnetB_eScaler_3  = BVDC_P_VNET_B(Scaler_3),
	BVDC_P_VnetB_eScaler_4  = BVDC_P_VNET_B(Scaler_4),
	BVDC_P_VnetB_eScaler_5  = BVDC_P_VNET_B(Scaler_5),
	BVDC_P_VnetB_eScaler_6  = BVDC_P_VNET_B(Scaler_6),
	BVDC_P_VnetB_eScaler_7  = BVDC_P_VNET_B(Scaler_7),
#if (BCHP_CHIP != 7420)
	BVDC_P_VnetB_eMad32_0   = BVDC_P_VNET_B(MAD_0),
	BVDC_P_VnetB_eMad32_1   = BVDC_P_VNET_B(MAD_1),
#else
	BVDC_P_VnetB_eMad32_0   = BVDC_P_VNET_B(MCVP_0),
	BVDC_P_VnetB_eMad32_1   = BVDC_P_VNET_B(MCVP_1),
#endif
	BVDC_P_VnetB_eChannel_0 = BVDC_P_VNET_B(Free_Ch_0),
	BVDC_P_VnetB_eChannel_1 = BVDC_P_VNET_B(Free_Ch_1),
	BVDC_P_VnetB_eChannel_2 = BVDC_P_VNET_B(Free_Ch_2),
	BVDC_P_VnetB_eChannel_3 = BVDC_P_VNET_B(Free_Ch_3),
	BVDC_P_VnetB_eChannel_4 = BVDC_P_VNET_B(Free_Ch_4),
	BVDC_P_VnetB_eChannel_5 = BVDC_P_VNET_B(Free_Ch_5),
	BVDC_P_VnetB_eChannel_6 = BVDC_P_VNET_B(Free_Ch_6),
	BVDC_P_VnetB_eChannel_7 = BVDC_P_VNET_B(Free_Ch_7),
	BVDC_P_VnetB_eChannel_8 = BVDC_P_VNET_B(Free_Ch_8),
	BVDC_P_VnetB_eChannel_9 = BVDC_P_VNET_B(Free_Ch_9),
	BVDC_P_VnetB_eChannel_10 = BVDC_P_VNET_B(Free_Ch_10),
	BVDC_P_VnetB_eChannel_11 = BVDC_P_VNET_B(Free_Ch_11),
	BVDC_P_VnetB_eChannel_12 = BVDC_P_VNET_B(Free_Ch_12),
	BVDC_P_VnetB_eChannel_13 = BVDC_P_VNET_B(Free_Ch_13),
	BVDC_P_VnetB_eDnr_0     = BVDC_P_VNET_B(DNR_0),
	BVDC_P_VnetB_eDnr_1     = BVDC_P_VNET_B(DNR_1),
	BVDC_P_VnetB_eDnr_2     = BVDC_P_VNET_B(DNR_2),
	BVDC_P_VnetB_eDnr_3     = BVDC_P_VNET_B(DNR_3),
	BVDC_P_VnetB_eDnr_4     = BVDC_P_VNET_B(DNR_4),
	BVDC_P_VnetB_eAnr_0     = BVDC_P_VNET_B(ANR_0),
	BVDC_P_VnetB_eAnr_1     = BVDC_P_VNET_B(ANR_1),
	BVDC_P_VnetB_eFgt_0     = BVDC_P_VNET_B(FGT_0),
	BVDC_P_VnetB_eDisabled  = BVDC_P_VNET_B(Output_Disabled),
	BVDC_P_VnetB_eInvalid   = -1
} BVDC_P_VnetB;

/* Vnet drain configuration */
typedef struct
{
	/* VNET_F */
	BVDC_P_ResourceType       eVnetFDrainType;    /* vnet_f drain type */
	uint32_t                  ulVnetFDrainId;     /* vnet_f drain id */
	BVDC_P_VnetF              eVnetFDrainSrc;     /* vnet_f src */

	/* VNET_B */
	BVDC_P_ResourceType       eVnetBDrainType;    /* vnet_b drain type */
	uint32_t                  ulVnetBDrainId;     /* vnet_b drain id */
	BVDC_P_VnetB              eVnetBDrainSrc;     /* vnet_b src */

	/* Drain Debug */
	uint32_t                  ulDbgOffset;        /* offset from BCHP_VNET_F_DRAIN_0_PIC_SIZE_CNT */

} BVDC_P_DrainContext;


/***************************************************************************
 * VNET Front and back context.
 ***************************************************************************/
typedef struct BVDC_P_VnetContext
{
	BDBG_OBJECT(BVDC_VNT)

	BREG_Handle                        hRegister;

} BVDC_P_VnetContext;


/***************************************************************************
 * Build RUL for VNET front & back
 ***************************************************************************/
BERR_Code BVDC_P_Vnet_Create
	( BVDC_P_Vnet_Handle              *phVnet,
	  BREG_Handle                      hRegister );

void BVDC_P_Vnet_Destroy
	( BVDC_P_Vnet_Handle               hVnet );

BERR_Code BVDC_P_Drain_Acquire
	( BVDC_P_DrainContext             *pDrain,
	  BVDC_P_ResourceContext          *pResource,
	  BAVC_SourceId                    eSourceId );

void BVDC_P_Drain_Release
	( const BVDC_P_DrainContext       *pDrain,
	  BVDC_P_ResourceContext          *pResource );

void BVDC_P_Drain_BuildRul_isr
	( const BVDC_P_DrainContext       *pDrain,
	  BVDC_P_ListInfo                 *pList );

void BVDC_P_Drain_BuildFormatRul_isr
	( const BVDC_P_DrainContext       *pDrain,
	  const BVDC_P_Rect               *pScanOut,
	  const BFMT_VideoInfo            *pFmtInfo,
	  BVDC_P_ListInfo                 *pList );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_VNET_PRIV_H__ */

/* End of file. */
