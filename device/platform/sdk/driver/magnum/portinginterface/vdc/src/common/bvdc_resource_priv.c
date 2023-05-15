/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_resource_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/67 $
 * $brcm_Date: 8/16/12 2:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_resource_priv.c $
 * 
 * Hydra_Software_Devel/67   8/16/12 2:42p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/66   8/15/12 6:22p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/65   8/15/12 10:46a yuxiaz
 * SW7550-802: Fixed segfault on 7400: only create FGT resource when FGT
 * support is enabled.
 *
 * Hydra_Software_Devel/64   3/21/12 1:38p syang
 * SW7425-2699: put mpaa under the control of vdc resource manager; handle
 * correctly both order of enabling/disabling mpaa and turning on/off
 * hdmi/component output
 *
 * Hydra_Software_Devel/63   2/29/12 4:34p tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/62   11/30/11 1:47p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/61   9/28/11 4:17p pntruong
 * SW7425-969: Refactored mcvp/madr code to be multi-instances friendly.
 * Adapt to vnet naming changes.
 *
 * Hydra_Software_Devel/60   6/21/11 11:02p pntruong
 * SW7425-337: Temporary restricted 2nd deinterlacer (madr) for encoder
 * path only to match 7425 usages.
 *
 * Hydra_Software_Devel/59   12/28/10 3:05p vanessah
 * SW7422-52:
 * Two deinterlacers support.
 *
 * Hydra_Software_Devel/58   12/27/10 3:08p vanessah
 * SW7422-52:
 * Fix MCDI issue for 7425
 *
 * Hydra_Software_Devel/57   11/15/10 6:00p pntruong
 * SW7231-2: Initial support to make 7231 compile.
 *
 * Hydra_Software_Devel/56   11/10/10 1:41p tdo
 * SW7422-13: Fix IT version for 7422, Fix bus error and segfault with
 * MCVP enable.
 *
 * Hydra_Software_Devel/55   11/5/10 5:33p vanessah
 * SW7422-52:  2 deinterlacers support
 *
 * Hydra_Software_Devel/54   10/15/10 12:42p tdo
 * SW7425-33: VDC: Add the 4th Compositor Support for 7425
 *
 * Hydra_Software_Devel/53   9/28/10 7:29p tdo
 * SW7550-561: Return err without trace if query
 *
 * Hydra_Software_Devel/52   9/27/10 11:52a vanessah
 * SW7425-29: STG support for transcode path
 *
 * Hydra_Software_Devel/51   9/23/10 10:29a vanessah
 * SW7425-29:  STG support for transcoding
 *
 * Hydra_Software_Devel/50   6/18/10 4:22p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/49   6/8/10 11:57a rpan
 * SW7400-2763: Print a warning message when resource is not available
 * instead of doing assertion.
 *
 * Hydra_Software_Devel/48   4/19/10 10:19p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/47   4/7/10 11:34a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/46   4/5/10 4:13p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/45   10/12/09 4:37p rpan
 * SW7405-3159: Moved writer/reader time stamp register from CAP/VFD to
 * RDC scratch registers.
 *
 * Hydra_Software_Devel/44   6/25/09 1:30p rpan
 * PR56137: Removed unused resource type CMP.
 *
 * Hydra_Software_Devel/43   6/25/09 12:12p rpan
 * PR56137, PR56138, PR56139, PR56166, PR56167, PR56168: Support for
 * various orthogonal VEC configurations.
 *
 * Hydra_Software_Devel/42   6/11/09 5:28p darnstein
 * PR55219: fix error in previous check-in
 *
 * Hydra_Software_Devel/41   6/11/09 4:04p darnstein
 * PR55219: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/40   3/16/09 10:49p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:30p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/39   1/21/09 10:14a rpan
 * PR45804: More HDMI output work.
 *
 * Hydra_Software_Devel/38   12/19/08 2:00p rpan
 * PR50391: Cleaned up some comments.
 *
 * Hydra_Software_Devel/37   12/19/08 12:11p rpan
 * PR50391: Diable analog chan 1. Also not use Secam_0.
 *
 * Hydra_Software_Devel/36   12/15/08 9:09p tdo
 * PR50391: Basic VDC bringup
 *
 * Hydra_Software_Devel/35   12/8/08 5:43p rpan
 * PR45804: Switched to VDC generic resource management lib.
 *
 * Hydra_Software_Devel/34   9/24/08 1:16p tdo
 * PR46482: Change FIR programming for SCL block for 3548 B0
 *
 * Hydra_Software_Devel/33   9/19/08 3:56p tdo
 * PR46481: Add dejagging to SCL implementation for 3548 B0
 *
 * Hydra_Software_Devel/32   9/16/08 10:17p pntruong
 * PR46494: B0: Remove 3rd output dac.
 *
 * Hydra_Software_Devel/31   9/5/08 4:46p tdo
 * PR46484: Bringup appframework for7420
 *
 * Hydra_Software_Devel/30   9/4/08 8:31p pntruong
 * PR46502: Moving 7403 vdc dedicated branch.
 *
 * Hydra_Software_Devel/29   3/26/08 7:03p pntruong
 * PR40448: Re-arraged resource info table and added place holder for
 * pcpll resource.
 *
 * Hydra_Software_Devel/28   3/19/08 5:44p tdo
 * PR39333: Implement HIST core
 *
 * Hydra_Software_Devel/27   2/28/08 6:22p hongtaoz
 * PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
 * fixed non-linear scaling when MAD is on;
 *
 * Hydra_Software_Devel/26   2/13/08 3:11p pntruong
 * PR39421: Removed unused code to improve readability.
 *
 * Hydra_Software_Devel/25   12/10/07 4:02p pntruong
 * PR38046: Added work-around by preventing HD_DVI_1 connecting to drain 1
 * and 2.
 *
 * Hydra_Software_Devel/24   12/6/07 3:23p tdo
 * PR37550: Fix coverity issues
 *
 * Hydra_Software_Devel/23   11/20/07 11:07p tdo
 * PR36889: Remove strictly alias warnings
 *
 * Hydra_Software_Devel/22   11/2/07 7:34p albertl
 * PR36701:  Added MUD functionality for 7440 C0.
 *
 * Hydra_Software_Devel/21   10/30/07 2:30p rpan
 * PR34798: Removed support for obsoleted versions of ANR.
 *
 * Hydra_Software_Devel/20   10/23/07 11:17a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/1   10/12/07 10:49a yuxiaz
 * PR29569: Add FGT support on 7405
 *
 * Hydra_Software_Devel/19   8/30/07 4:25p darnstein
 * PR33711: declare static tables to be const. This is required by
 * Broadcom's programming standards.
 *
 * Hydra_Software_Devel/18   8/2/07 2:08p yuxiaz
 * PR27644: 7405 Brutus bring up.
 *
 * Hydra_Software_Devel/17   5/10/07 9:58a syang
 * PR 28895, PR 28898, PR 29846 , PR 28550: 1). add frame for C0 ANR PI
 * support; 2). move MAD/ANR buf alloc/free to MAD/ANR; 3). remove
 * drainVnet state
 *
 * Hydra_Software_Devel/16   5/7/07 6:04p pntruong
 * PR28899: C0: BVN: VFD 10-bit 444 to 422 Mode Transition Requires Soft
 * Reset.
 *
 * Hydra_Software_Devel/15   4/19/07 1:48p yuxiaz
 * PR 26052: Add CSC support in MFD on 7400 B0.
 *
 * Hydra_Software_Devel/14   4/2/07 11:29a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/13   2/27/07 9:01p tdo
 * PR 27970: Rename pContext to pResource for more readability
 *
 * Hydra_Software_Devel/12   2/23/07 6:30p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/11   2/8/07 3:39p maivu
 * PR 27558: Fixed warnings for release build
 *
 * Hydra_Software_Devel/10   1/12/07 2:55p yuxiaz
 * PR26151: Fix a typo.
 *
 * Hydra_Software_Devel/9   1/12/07 2:45p yuxiaz
 * PR26151: Fix the problem that only one resource can be acquired from
 * one resource type.
 *
 * Hydra_Software_Devel/8   1/12/07 11:50a syang
 * PR 27054: add error mesg
 *
 * Hydra_Software_Devel/7   1/5/07 4:25p syang
 * PR 22569: almost completed coding for new vnet build/shutdown algorithm
 * (by each sub-modules)
 *
 * Hydra_Software_Devel/6   1/2/07 4:53p yuxiaz
 * PR25995: Added code to reserve a resource in resource manager.
 *
 * Hydra_Software_Devel/5   12/18/06 11:34p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/8   11/29/06 4:27p syang
 * PR 26151: release shred resource after it is not used by HW
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/7   11/28/06 4:38p yuxiaz
 * PR23638: Do not use CRC for vnet drain on 3563. It'll be used by bypass
 * compositor.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/6   11/28/06 11:48a yuxiaz
 * PR23638: Fixed compile error for 3563.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   11/28/06 11:42a yuxiaz
 * PR23638: Added new type BVDC_P_ResourceType_eDrainCrc for vnet drain.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   11/27/06 10:48a yuxiaz
 * PR 26151: More debug for shared resource manager.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   11/22/06 6:03p syang
 * PR 26151: 1st round of debug after re-write shared resource manager.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/22/06 3:52p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/4   6/14/06 1:43p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/3   8/19/05 2:54p hongtaoz
 * PR15888: adding support for dynamic switch on/off shared scaler on
 * bypass window;
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/19/05 2:28p hongtaoz
 * PR15888: adding support for dynamic switch on/off of shared scaler with
 * Bypass window;
 *
 * Hydra_Software_Devel/2   10/6/04 3:43p pntruong
 * PR12728: Fixed -pdantic build warnings and errors.
 *
 * Hydra_Software_Devel/1   7/8/04 2:44p jasonh
 * PR 11867: Adding resource control to VDC
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bvdc_resource_priv.h"
#include "bvdc_priv.h"
#include "bchp_scl_0.h"
#include "bchp_vnet_b.h"
#include "bvdc_capture_priv.h"
#include "bvdc_scaler_priv.h"
#include "bvdc_hscaler_priv.h"
#include "bvdc_mad_priv.h"
#include "bvdc_mud_priv.h"
#include "bvdc_dnr_priv.h"
#include "bvdc_anr_priv.h"
#include "bvdc_feeder_priv.h"
#include "bvdc_boxdetect_priv.h"
#include "bvdc_fgt_priv.h"
#include "bvdc_hist_priv.h"
#include "bvdc_mcvp_priv.h"

BDBG_MODULE(BVDC_RESOURCE);
BDBG_OBJECT_ID(BVDC_RES);

/*--------------------------------------------------------------------*/
#define BVDC_P_RSRC_END_ON_FAIL(result) \
if ( BERR_SUCCESS != BERR_TRACE(result)) \
{\
	goto Done;  \
}

/* macro for "the number of shared HW modules of the shared resource type",
 * "the HW ID of the first shared HW module of a shared resource type",
 * Note: assume the HW ID of the shared HW modules of the same type are
 * contiguous */
#define BVDC_P_NUM_SHARED_VFD          0
#define BVDC_P_NUM_SHARED_CAP          0
#define BVDC_P_NUM_SHARED_ANR          BVDC_P_SUPPORT_ANR
#define BVDC_P_NUM_SHARED_MAD          BVDC_P_SUPPORT_MAD
#define BVDC_P_NUM_SHARED_MCVP         BVDC_P_SUPPORT_MCVP
#define BVDC_P_NUM_SHARED_MUD          BVDC_P_SUPPORT_MUD
#define BVDC_P_NUM_SHARED_DNR          BVDC_P_SUPPORT_DNR
#define BVDC_P_NUM_SHARED_FGT          BVDC_P_SUPPORT_FGT
#define BVDC_P_NUM_SHARED_HSCL         BVDC_P_SUPPORT_HSCL
#define BVDC_P_NUM_SHARED_HIST         BVDC_P_SUPPORT_HIST
#define BVDC_P_NUM_SHARED_BOX          BVDC_P_SUPPORT_BOX_DETECT
#if (BCHP_CHIP==7400) || (BCHP_CHIP==7405) || (BCHP_CHIP==7420) || (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7435)
#define BVDC_P_NUM_SHARED_SCL          1
#define BVDC_P_ID0_SHARED_SCL          BVDC_P_ScalerId_eScl3
#else
#define BVDC_P_NUM_SHARED_SCL          0
#define BVDC_P_ID0_SHARED_SCL          BVDC_P_ScalerId_eUnknown
#endif
#define BVDC_P_NUM_SHARED_FCH          BVDC_P_SUPPORT_FREE_CHANNEL
#define BVDC_P_ID0_SHARED_FCH          BVDC_P_FreeChId_eCh0
#define BVDC_P_NUM_SHARED_LPBK         BVDC_P_SUPPORT_LOOP_BACK
#define BVDC_P_NUM_SHARED_DRN_C        BVDC_P_SUPPORT_DRAIN_CRC
#define BVDC_P_NUM_SHARED_DRN_F        BVDC_P_SUPPORT_DRAIN_F
#define BVDC_P_NUM_SHARED_DRN_B        BVDC_P_SUPPORT_DRAIN_B

/* Make nicer formating. */
#define BVDC_P_NUM_SHARED_NONE         0
#define BVDC_P_NUM_SHARED_PCPLL        1

/* capabilities are or-ed during acquiring.
 * if all hw module of a type has the same capability set, s_ul*Able is not
 * defined, and 0 is used for acquiring */

#if (BVDC_P_NUM_SHARED_VFD > 0)
static uint32_t s_ulVfdAbleFlags[] =
{
	/* BVDC_P_FeederId_eVfd0 */   (BVDC_P_Able_eMem0),
	/* BVDC_P_FeederId_eVfd1 */   (BVDC_P_Able_eMem0),
	/* BVDC_P_FeederId_eVfd2 */   (BVDC_P_Able_eMem0),
	/* BVDC_P_FeederId_eVfd3 */   (BVDC_P_Able_eMem0),
	/* BVDC_P_FeederId_eVfd4 */   (BVDC_P_Able_eMem0),
	/* BVDC_P_FeederId_eVfd5 */   (BVDC_P_Able_eMem0),
	/* BVDC_P_FeederId_eVfd6 */   (BVDC_P_Able_eMem0),
	/* BVDC_P_FeederId_eVfd7 */   (BVDC_P_Able_eMem0),
	/* BVDC_P_FeederId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_CAP > 0)
static const uint32_t s_ulCaptureAbleFlags[] =
{
	/* BVDC_P_CaptureId_eCap0 */  (BVDC_P_Able_eMem0),
	/* BVDC_P_CaptureId_eCap1 */  (BVDC_P_Able_eMem0),
	/* BVDC_P_CaptureId_eCap2 */  (BVDC_P_Able_eMem0),
	/* BVDC_P_CaptureId_eCap3 */  (BVDC_P_Able_eMem0),
	/* BVDC_P_CaptureId_eCap4 */  (BVDC_P_Able_eMem0),
	/* BVDC_P_CaptureId_eCap5 */  (BVDC_P_Able_eMem0),
	/* BVDC_P_CaptureId_eCap6 */  (BVDC_P_Able_eMem0),
	/* BVDC_P_CaptureId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_SCL > 0)
static const uint32_t s_ulScalerAbleFlags[] =
{
#if (BVDC_P_SUPPORT_SCL >= 2)
	/* BVDC_P_ScalerId_eScl0 */   (BVDC_P_Able_eHd),
#else
	/* BVDC_P_ScalerId_eScl0 */   (0),
#endif
#if (BVDC_P_SUPPORT_SCL >= 3)
	/* BVDC_P_ScalerId_eScl1 */   (BVDC_P_Able_eHd),
#else
	/* BVDC_P_ScalerId_eScl1 */   (0),
#endif
	/* BVDC_P_ScalerId_eScl2 */   (0),
	/* BVDC_P_ScalerId_eScl3 */   (0),
	/* BVDC_P_ScalerId_eScl4 */   (0),
	/* BVDC_P_ScalerId_eScl5 */   (0),
	/* BVDC_P_ScalerId_eScl6 */   (0),
	/* BVDC_P_ScalerId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_MAD > 0)
static const uint32_t s_ulMadAbleFlags[] =
{
#if BVDC_P_SUPPORT_HD_MAD
	/* BVDC_P_MadId_eMad0 */      (BVDC_P_Able_eHd | BVDC_P_Able_eMem0),
#else
	/* BVDC_P_MadId_eMad0 */      (BVDC_P_Able_eMem0),
#endif
	/* BVDC_P_MadId_eMad1 */      (BVDC_P_Able_eHd | BVDC_P_Able_eMem1),
	/* BVDC_P_MadId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_MCVP > 0)
static const uint32_t s_ulMcvpAbleFlags[] =
{
	/* BVDC_P_McvpId_eMcvp0 */    (BVDC_P_Able_eHd),
	/* BVDC_P_McvpId_eMcvp1 */    (BVDC_P_Able_eHd | BVDC_P_Able_eXcode),
	/* BVDC_P_McvpId_eMcvp2 */    (BVDC_P_Able_eHd | BVDC_P_Able_eXcode),
	/* BVDC_P_McvpId_eMcvp3 */    (BVDC_P_Able_eHd | BVDC_P_Able_eXcode),
	/* BVDC_P_McvpId_eMcvp4 */    (BVDC_P_Able_eHd | BVDC_P_Able_eXcode),
	/* BVDC_P_McvpId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_MUD > 0)
static const uint32_t s_ulMudAbleFlags[] =
{
	/* BVDC_P_MudId_eMud0 */      (0),
	/* BVDC_P_MudId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_DNR > 0)
static const uint32_t s_ulDnrAbleFlags[] =
{
	/* BVDC_P_DnrId_eDnr0 */      (0),
	/* BVDC_P_DnrId_eDnr1 */      (0),
	/* BVDC_P_DnrId_eDnr2 */      (0),
	/* BVDC_P_DnrId_eDnr3 */      (0),
	/* BVDC_P_DnrId_eDnr4 */      (0),
	/* BVDC_P_DnrId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_FGT > 0)
static const uint32_t s_ulFgtAbleFlags[] =
{
	/* BVDC_P_FgtId_eFgt0 */      (0),
	/* BVDC_P_FgtId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_ANR > 0)
static const uint32_t s_ulAnrAbleFlags[] =
{
	/* BVDC_P_AnrId_eAnr0 */      (BVDC_P_Able_eHd),
	/* BVDC_P_AnrId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_HSCL > 0)
static const uint32_t s_ulHsclAbleFlags[] =
{
	/* BVDC_P_HsclId_eHscl0 */    (0),
	/* BVDC_P_HsclId_eHscl1 */    (0),
	/* BVDC_P_HsclId_eHscl2 */    (0),
	/* BVDC_P_HsclId_eHscl3 */    (0),
	/* BVDC_P_HsclId_eHscl4 */    (0),
	/* BVDC_P_HsclId_eUnknown */
};
#endif

#if (BVDC_P_NUM_SHARED_HIST > 0)
static const uint32_t s_ulHistAbleFlags[] =
{
	/* BVDC_P_HistId_eHist0 */    (0),
	/* BVDC_P_HistId_eUnknown */
};
#endif

static const uint32_t s_ulDrnFrnAbleFlags[] =
{
	/* BVDC_P_DrainFrnId_eDrain0  */ (BVDC_P_Able_eAllSrc),
	/* BVDC_P_DrainFrnId_eDrain1  */ (0),
	/* BVDC_P_DrainFrnId_eDrain2  */ (0),
	/* BVDC_P_DrainFrnId_eDrain3  */ (BVDC_P_Able_eAllSrc),
	/* BVDC_P_DrainFrnId_eDrain4  */ (BVDC_P_Able_eAllSrc),
	/* BVDC_P_DrainFrnId_eDrain5  */ (BVDC_P_Able_eAllSrc),
	/* BVDC_P_DrainFrnId_eDrain6  */ (BVDC_P_Able_eAllSrc),
	/* BVDC_P_DrainFrnId_eUnknown */ (BVDC_P_Able_eAllSrc)
};

/* Short hande to make an entry. */
#define BVDC_P_MAKE_RES(resource, count, fisrt_entry)                \
{                                                                    \
	(BVDC_P_NUM_##count),                                            \
	(fisrt_entry),                                                   \
	(#resource),                                                     \
	(BVDC_P_ResourceType_##resource)                                 \
}

typedef struct
{
	uint32_t                           ulCount;
	uint32_t                           ulFirstId;
	const char                        *pchName;
	BVDC_P_ResourceType                eResource;

} BVDC_P_ResourceInfoEntry;

/* Table contains number of available resource and 1st id available */
static const BVDC_P_ResourceInfoEntry s_aResInfoTbl[] =
{
	/* shared resources represented by handle */
	BVDC_P_MAKE_RES(eVfd,        SHARED_VFD,   BVDC_P_FeederId_eVfd0         ),
	BVDC_P_MAKE_RES(eCap,        SHARED_CAP,   BVDC_P_CaptureId_eCap0        ),
	BVDC_P_MAKE_RES(eAnr,        SHARED_ANR,   BVDC_P_AnrId_eAnr0            ),
	BVDC_P_MAKE_RES(eMad,        SHARED_MAD,   BVDC_P_MadId_eMad0            ),
	BVDC_P_MAKE_RES(eHscl,       SHARED_HSCL,  BVDC_P_HscalerId_eHscl0       ),
	BVDC_P_MAKE_RES(eMcvp,       SHARED_MCVP,  BVDC_P_McvpId_eMcvp0          ),
	BVDC_P_MAKE_RES(eMud,        SHARED_MUD,   BVDC_P_MudId_eMud0            ),
	BVDC_P_MAKE_RES(eDnr,        SHARED_DNR,   BVDC_P_DnrId_eDnr0            ),
	BVDC_P_MAKE_RES(eBoxDetect,  SHARED_BOX,   BVDC_P_BoxDetectId_eBoxDetect0),
	BVDC_P_MAKE_RES(eScl,        SHARED_SCL,   BVDC_P_ID0_SHARED_SCL         ),
	BVDC_P_MAKE_RES(eFgt,        SHARED_FGT,   BVDC_P_FgtId_eFgt0            ),
	BVDC_P_MAKE_RES(eHist,       SHARED_HIST,  BVDC_P_HistId_eHist0          ),

	/* separator between handle resources and HwId resources */
	BVDC_P_MAKE_RES(eHandleCntr, SHARED_NONE,  0                             ),

	/* shared resources idendified by Hw Id */
	BVDC_P_MAKE_RES(eFreeCh,     SHARED_FCH,   BVDC_P_ID0_SHARED_FCH         ),
	BVDC_P_MAKE_RES(eLpBck,      SHARED_LPBK,  BVDC_P_LpBckId_eLp0           ),
	BVDC_P_MAKE_RES(eDrainFrn,   SHARED_DRN_F, BVDC_P_DrainFrnId_eDrain0     ),
	BVDC_P_MAKE_RES(eDrainBck,   SHARED_DRN_B, BVDC_P_DrainBckId_eDrain0     ),
	BVDC_P_MAKE_RES(eDrainCrc,   SHARED_DRN_C, BVDC_P_DrainCrcId_eCrc0       ),

#if BVDC_P_ORTHOGONAL_VEC
	BVDC_P_MAKE_RES(e656,        SHARED_656,       0                         ),
	BVDC_P_MAKE_RES(eDvi,        SHARED_DVI,       0                         ),
	BVDC_P_MAKE_RES(eStg,        SHARED_STG,       0                         ),
	BVDC_P_MAKE_RES(eRf,         SHARED_RF,        0                         ),
	BVDC_P_MAKE_RES(eIt,         SHARED_IT,        0                         ),
	BVDC_P_MAKE_RES(eVf,         SHARED_VF,        0                         ),
	BVDC_P_MAKE_RES(eSecam,      SHARED_SECAM,     0                         ),
	BVDC_P_MAKE_RES(eSecam_HD,   SHARED_SECAM_HD,  0                         ),
	BVDC_P_MAKE_RES(eSdsrc,      SHARED_SDSRC,     0                         ),
	BVDC_P_MAKE_RES(eHdsrc,      SHARED_HDSRC,     0                         ),
	BVDC_P_MAKE_RES(eDac,        SHARED_DAC,       0                         ),
	BVDC_P_MAKE_RES(eMpaa,       SHARED_MPAA,      0                         ),
#endif

	BVDC_P_MAKE_RES(ePcPll,      SHARED_PCPLL, BVDC_P_PcPllId_ePll0          ),
	/* Must be last */
	BVDC_P_MAKE_RES(eInvalid,    SHARED_NONE,      0                         )
};

/***************************************************************************
 * {private}
 *
 */
BERR_Code  BVDC_P_Resource_Create
	( BVDC_P_Resource_Handle           *phResource,
	  BVDC_Handle                       hVdc )
{
	BVDC_P_ResourceContext  *pResource;
	BVDC_P_ResourceEntry *pResourceRecords, *pEntry;
	uint32_t  *pulIndex1stEntry;
	BVDC_P_ResourceType  eType;
	uint32_t   ii, ulNumEntries;
	BERR_Code eResult;

	BDBG_ENTER(BVDC_P_Resource_Create);
	BDBG_ASSERT(phResource);
	*phResource = NULL;

	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* Alloc the context and basic init. */
	pResource = (BVDC_P_ResourceContext*)
		(BKNI_Malloc(sizeof(BVDC_P_ResourceContext)));
	if(!pResource)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BKNI_Memset((void*)pResource, 0x0, sizeof(BVDC_P_ResourceContext));
	BDBG_OBJECT_SET(pResource, BVDC_RES);

	/* alloc and init the array for the index of 1st entry of each type */
	eResult = BERR_OUT_OF_SYSTEM_MEMORY;
	pulIndex1stEntry = (uint32_t *)
		(BKNI_Malloc(sizeof(uint32_t)*BVDC_P_ResourceType_eInvalid));
	if(NULL == pulIndex1stEntry)
	{
		goto Done;
	}
	pResource->pulIndex1stEntry = pulIndex1stEntry;

	ulNumEntries = 0;
	for (eType = 0; eType < BVDC_P_ResourceType_eInvalid; eType++)
	{
		*(pulIndex1stEntry+eType) = ulNumEntries;
		ulNumEntries += s_aResInfoTbl[eType].ulCount;
	}

	/* alloc the array for acquiring record of each type */
	pResourceRecords = (BVDC_P_ResourceEntry *)
		(BKNI_Malloc(sizeof(BVDC_P_ResourceEntry) * ulNumEntries));
	if(NULL == pResourceRecords)
	{
		goto Done;
	}
	BKNI_Memset((void*)pResourceRecords, 0x0,
		sizeof(BVDC_P_ResourceEntry) * ulNumEntries);
	pResource->pResourceRecords = pResourceRecords;

	eResult = BERR_SUCCESS;

	/* init acquiring record for each shared resource, including create
	 * handle for each shared resource represented by a handle */
	pEntry = pResourceRecords;
	for (eType = 0; eType < BVDC_P_ResourceType_eInvalid; eType++)
	{
		BDBG_ASSERT(s_aResInfoTbl[eType].eResource == eType);

		for (ii = 0; ii < s_aResInfoTbl[eType].ulCount; ii++)
		{
			/* pEntry->ulAcquireCntr = 0; */
			/* pEntry->ulCapabilities = 0; */
			pEntry->eType = eType;
			pEntry->ulAcquireId = BVDC_P_ACQUIRE_ID_INVALID;
			pEntry->bAcquireLock = false;

			switch (eType)
			{
#if (BVDC_P_NUM_SHARED_VFD > 0)
			case BVDC_P_ResourceType_eVfd:
				eResult = BVDC_P_Feeder_Create(
					(BVDC_P_Feeder_Handle *) (void *)&(pEntry->Id.pvHandle), hVdc->hRdc, hVdc->hRegister,
					(BVDC_P_FeederId) (s_aResInfoTbl[eType].ulFirstId + ii), NULL, hVdc->hTimer);
				pEntry->ulCapabilities = s_ulVfdAbleFlags[ii];
				break;
#endif

#if (BVDC_P_NUM_SHARED_CAP > 0)
			case BVDC_P_ResourceType_eCap:
				eResult = BVDC_P_Capture_Create(
					(BVDC_P_Capture_Handle *) (void *)&(pEntry->Id.pvHandle), hVdc->hRdc, hVdc->hRegister,
					(BVDC_P_CaptureId) (s_aResInfoTbl[eType].ulFirstId + ii), hVdc->hTimer,
					pResource);
				pEntry->ulCapabilities = s_ulCaptureAbleFlags[ii];
				break;
#endif

#if (BVDC_P_NUM_SHARED_ANR > 0)
			case BVDC_P_ResourceType_eAnr:
				eResult = BVDC_P_Anr_Create(
					(BVDC_P_Anr_Handle *) (void *)&(pEntry->Id.pvHandle),
					(BVDC_P_AnrId) (s_aResInfoTbl[eType].ulFirstId + ii), hVdc->hRegister,
					pResource);
				BVDC_P_RSRC_END_ON_FAIL(eResult);
				pEntry->ulCapabilities = s_ulAnrAbleFlags[ii];
				break;
#endif

#if (BVDC_P_NUM_SHARED_MAD > 0)
			case BVDC_P_ResourceType_eMad:
				eResult = BVDC_P_Mad_Create(
					(BVDC_P_Mad_Handle *) (void *)&(pEntry->Id.pvHandle), hVdc->hRegister,
					(BVDC_P_MadId) (s_aResInfoTbl[eType].ulFirstId + ii), pResource);
				BVDC_P_RSRC_END_ON_FAIL(eResult);
				pEntry->ulCapabilities = s_ulMadAbleFlags[ii];
				break;
#endif

#if (BVDC_P_NUM_SHARED_HSCL > 0)
			case BVDC_P_ResourceType_eHscl:
				eResult = BVDC_P_Hscaler_Create(
					(BVDC_P_Hscaler_Handle *) (void *)&(pEntry->Id.pvHandle),
					(BVDC_P_HscalerId) (s_aResInfoTbl[eType].ulFirstId + ii),
					pResource, hVdc->hRegister);
				BVDC_P_RSRC_END_ON_FAIL(eResult);
				pEntry->ulCapabilities = s_ulHsclAbleFlags[ii];
				break;
#endif

#if (BVDC_P_NUM_SHARED_MCVP > 0)
			case BVDC_P_ResourceType_eMcvp:
				eResult = BVDC_P_Mcvp_Create(
					(BVDC_P_Mcvp_Handle *) (void *)&(pEntry->Id.pvHandle),
					(BVDC_P_McvpId) (s_aResInfoTbl[eType].ulFirstId + ii),
					hVdc->hRegister, pResource);
				BVDC_P_RSRC_END_ON_FAIL(eResult);
				pEntry->ulCapabilities = s_ulMcvpAbleFlags[ii];
				break;
#endif

#if (BVDC_P_NUM_SHARED_MUD > 0)
			case BVDC_P_ResourceType_eMud:
				eResult = BVDC_P_Mud_Create(
					(BVDC_P_Mud_Handle *) (void *)&(pEntry->Id.pvHandle),
					(BVDC_P_MudId) (s_aResInfoTbl[eType].ulFirstId + ii));
				BVDC_P_RSRC_END_ON_FAIL(eResult);
				pEntry->ulCapabilities = s_ulMudAbleFlags[ii];
				break;
#endif

#if (BVDC_P_NUM_SHARED_DNR > 0)
			case BVDC_P_ResourceType_eDnr:
				eResult = BVDC_P_Dnr_Create(
					(BVDC_P_Dnr_Handle *) (void *)&(pEntry->Id.pvHandle),
					(BVDC_P_DnrId) (s_aResInfoTbl[eType].ulFirstId + ii), pResource);
				BVDC_P_RSRC_END_ON_FAIL(eResult);
				pEntry->ulCapabilities = s_ulDnrAbleFlags[ii];
				break;
#endif

#if (BVDC_P_NUM_SHARED_BOX > 0)
			case BVDC_P_ResourceType_eBoxDetect:
				eResult = BVDC_P_BoxDetect_Create(
					(BVDC_P_BoxDetect_Handle *) (void *)&(pEntry->Id.pvHandle),
					(BVDC_P_BoxDetectId) (s_aResInfoTbl[eType].ulFirstId + ii),
					hVdc->hRegister, pResource);
				BVDC_P_RSRC_END_ON_FAIL(eResult);
				break;
#endif

#if (BVDC_P_NUM_SHARED_SCL > 0)
			case BVDC_P_ResourceType_eScl:
				eResult = BVDC_P_Scaler_Create(
					(BVDC_P_Scaler_Handle *) (void *)&(pEntry->Id.pvHandle),
					(BVDC_P_ScalerId) (s_aResInfoTbl[eType].ulFirstId + ii),
					pResource, hVdc->hRegister);
				BVDC_P_RSRC_END_ON_FAIL(eResult);
				pEntry->ulCapabilities = s_ulScalerAbleFlags[ii];
				break;
#endif

#if (BVDC_P_NUM_SHARED_FGT > 0)
			case BVDC_P_ResourceType_eFgt:
				if(hVdc->stSettings.bEnableFgt)
				{
					eResult = BVDC_P_Fgt_Create(
						(BVDC_P_Fgt_Handle *) (void *)&(pEntry->Id.pvHandle),
						(BVDC_P_FgtId) (s_aResInfoTbl[eType].ulFirstId + ii),
						hVdc->hRegister, hVdc->hMemory, hVdc, pResource);
					BVDC_P_RSRC_END_ON_FAIL(eResult);
					pEntry->ulCapabilities = s_ulFgtAbleFlags[ii];
				}
				break;
#endif

#if (BVDC_P_NUM_SHARED_HIST > 0)
			case BVDC_P_ResourceType_eHist:
				eResult = BVDC_P_Hist_Create(
					(BVDC_P_Hist_Handle *) (void *)&(pEntry->Id.pvHandle),
					(BVDC_P_HistId) (s_aResInfoTbl[eType].ulFirstId + ii),
					hVdc->hRegister, pResource);
				BVDC_P_RSRC_END_ON_FAIL(eResult);
				pEntry->ulCapabilities = s_ulHistAbleFlags[ii];
				break;
#endif

			/* Front drain have different capabilities (or limiations).
			 * HD_DVI_1 can not use VNET_F_DRAIN_1_SRC and VNET_F_DRAIN_2_SRC */
			case BVDC_P_ResourceType_eDrainFrn:
				pEntry->Id.ulHwId = s_aResInfoTbl[eType].ulFirstId + ii;
				pEntry->ulCapabilities = s_ulDrnFrnAbleFlags[ii];
				break;

			case BVDC_P_ResourceType_eFreeCh:
			case BVDC_P_ResourceType_eLpBck:
			case BVDC_P_ResourceType_eDrainBck:
			case BVDC_P_ResourceType_eDrainCrc:
			case BVDC_P_ResourceType_ePcPll:
#if BVDC_P_ORTHOGONAL_VEC
			case BVDC_P_ResourceType_e656:
			case BVDC_P_ResourceType_eDvi:
			case BVDC_P_ResourceType_eStg:
			case BVDC_P_ResourceType_eRf:
			case BVDC_P_ResourceType_eIt:
			case BVDC_P_ResourceType_eVf:
			case BVDC_P_ResourceType_eSecam:
			case BVDC_P_ResourceType_eSecam_HD:
			case BVDC_P_ResourceType_eSdsrc:
			case BVDC_P_ResourceType_eHdsrc:
			case BVDC_P_ResourceType_eDac:
			case BVDC_P_ResourceType_eMpaa:
#endif
				pEntry->Id.ulHwId = s_aResInfoTbl[eType].ulFirstId + ii;
				break;

			default:
				BDBG_ASSERT(BVDC_P_ResourceType_eHandleCntr == eType);
			}

			pEntry++;
		}
	}

	*phResource = (BVDC_P_Resource_Handle)pResource;

  Done:
	if (BERR_SUCCESS != eResult)
	{
		BVDC_P_Resource_Destroy((BVDC_P_Resource_Handle)pResource);
	}

	BDBG_LEAVE(BVDC_P_Resource_Create);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 */
void  BVDC_P_Resource_Destroy
	( BVDC_P_Resource_Handle            hResource )
{
	BVDC_P_ResourceEntry *pEntry;
	BVDC_P_ResourceType  eType;
	uint32_t   ii;

	BDBG_ENTER(BVDC_P_Resource_Destroy);
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);

	pEntry = hResource->pResourceRecords;
	if (NULL != pEntry)
	{
		/* destroy all shared resource's handle */
		for (eType = 0; eType < BVDC_P_ResourceType_eHandleCntr; eType++)
		{
			for (ii = 0; ii < s_aResInfoTbl[eType].ulCount; ii++)
			{
				if (NULL == pEntry->Id.pvHandle)
					break;

				BDBG_ASSERT(pEntry->eType == eType);
				switch (eType)
				{
#if (BVDC_P_NUM_SHARED_VFD > 0)
				case BVDC_P_ResourceType_eVfd:
					BVDC_P_Feeder_Destroy(
						(BVDC_P_Feeder_Handle) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_CAP > 0)
				case BVDC_P_ResourceType_eCap:
					BVDC_P_Capture_Destroy(
						(BVDC_P_Capture_Handle) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_ANR > 0)
				case BVDC_P_ResourceType_eAnr:
					BVDC_P_Anr_Destroy(
						(BVDC_P_Anr_Handle) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_MAD > 0)
				case BVDC_P_ResourceType_eMad:
					BVDC_P_Mad_Destroy(
						(BVDC_P_Mad_Handle) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_HSCL > 0)
				case BVDC_P_ResourceType_eHscl:
					BVDC_P_Hscaler_Destroy(
						(BVDC_P_HscalerContext *) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_MCVP > 0)
				case BVDC_P_ResourceType_eMcvp:
					BVDC_P_Mcvp_Destroy(
						(BVDC_P_McvpContext *) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_MUD > 0)
				case BVDC_P_ResourceType_eMud:
					BVDC_P_Mud_Destroy(
						(BVDC_P_Mud_Handle) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_DNR > 0)
				case BVDC_P_ResourceType_eDnr:
					BVDC_P_Dnr_Destroy(
						(BVDC_P_Dnr_Handle) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_BOX > 0)
				case BVDC_P_ResourceType_eBoxDetect:
					BVDC_P_BoxDetect_Destroy(
						(BVDC_P_BoxDetect_Handle) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_SCL > 0)
				case BVDC_P_ResourceType_eScl:
					BVDC_P_Scaler_Destroy(
						(BVDC_P_Scaler_Handle) (pEntry->Id.pvHandle));
					break;
#endif

#if (BVDC_P_NUM_SHARED_FGT > 0)
			case BVDC_P_ResourceType_eFgt:
				BVDC_P_Fgt_Destroy(
					(BVDC_P_Fgt_Handle)(pEntry->Id.pvHandle));
				break;
#endif

#if (BVDC_P_NUM_SHARED_HIST > 0)
			case BVDC_P_ResourceType_eHist:
				BVDC_P_Hist_Destroy(
					(BVDC_P_Hist_Handle)(pEntry->Id.pvHandle));
				break;
#endif

				default:
					BDBG_ASSERT(false);
				}

				pEntry++;
			}
		}

		BKNI_Free(hResource->pResourceRecords);
	}

	if (NULL != hResource->pulIndex1stEntry)
	{
		BKNI_Free(hResource->pulIndex1stEntry);
	}

	BDBG_OBJECT_DESTROY(hResource, BVDC_RES);
	BKNI_Free(hResource);

	BDBG_LEAVE(BVDC_P_Resource_Destroy);
}

/* if the resource type has already be acquired for an acquire ID successfully,
 * we give it the same resource module, and increase the acquiring cntr;
 * otherwise, we try to find a perfect match for capabilities; if we can not
 * find a perfect match, the first module that satisfies the capability
 * requirement will be used. */
static BERR_Code  BVDC_P_Resource_Acquire_isr
	( BVDC_P_ResourceEntry             *p1stEntry,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulCapabilities,
	  uint32_t                          ulAcquireId,
	  void                            **ppvResourceHandle,
	  uint32_t                         *pulHwId,
	  bool								bQuery)
{
	BVDC_P_ResourceEntry  *pEntry, *pCandidate;
	uint32_t  ii;

	BDBG_ASSERT((NULL != ppvResourceHandle) || (NULL != pulHwId));

	/* if this type has already be acquired for this Id, give the same module
	 * to it, and increase the ulAcquireId */
	pEntry = p1stEntry;
	for (ii = 0; ii < s_aResInfoTbl[eType].ulCount; ii++)
	{
		BDBG_ASSERT(pEntry->eType == eType);
		if ((ulAcquireId == pEntry->ulAcquireId) &&
			(false == pEntry->bAcquireLock))
		{
			BDBG_ASSERT(ulCapabilities ==
						(ulCapabilities & pEntry->ulCapabilities));
			BDBG_ASSERT(pEntry->ulAcquireCntr > 0);
			if (bQuery)
			{
				if (NULL != ppvResourceHandle)
					*ppvResourceHandle = (void *)BVDC_P_RESOURCE_ID_AVAIL;
				else
					*pulHwId = BVDC_P_RESOURCE_ID_AVAIL;

			}
			else
			{
				pEntry->ulAcquireCntr++;
				if (NULL != ppvResourceHandle)
					*ppvResourceHandle = pEntry->Id.pvHandle;
				else
					*pulHwId = pEntry->Id.ulHwId;
			}
			return BERR_SUCCESS;
		}
		pEntry++;
	}

	/* if this type has already be acquired for this Id, give the same module
	 * to it, and increase the ulAcquireId */
	pEntry = p1stEntry;
	pCandidate = NULL;
	for (ii = 0; ii < s_aResInfoTbl[eType].ulCount; ii++)
	{
		/* already used by some one? */
		if (BVDC_P_ACQUIRE_ID_INVALID != pEntry->ulAcquireId)
		{
			BDBG_ASSERT(pEntry->ulAcquireCntr > 0);
			pEntry++;
			continue;
		}
		BDBG_ASSERT((0 == pEntry->ulAcquireCntr) &&
					(false == pEntry->bAcquireLock));

		/* all required capabilities are satisfied? */
		if (ulCapabilities == (ulCapabilities & pEntry->ulCapabilities))
		{
			if (ulCapabilities == pEntry->ulCapabilities)
			{
				if (bQuery)
				{
					if (NULL != ppvResourceHandle)
						*ppvResourceHandle = (void *)BVDC_P_RESOURCE_ID_AVAIL;
					else
						*pulHwId = BVDC_P_RESOURCE_ID_AVAIL;
				}
				else
				{
					/* perfect match, use this module */
					pEntry->ulAcquireId = ulAcquireId;
					pEntry->ulAcquireCntr = 1;
					if (NULL != ppvResourceHandle)
						*ppvResourceHandle = pEntry->Id.pvHandle;
					else
						*pulHwId = pEntry->Id.ulHwId;
				}
				return BERR_SUCCESS;
			}
			else if (NULL == pCandidate)
			{
				/* the 1st found one will be used */
				pCandidate = pEntry;
			}
		}
		pEntry++;
	}

	if (NULL != pCandidate)
	{
		if (bQuery)
		{
			if (NULL != ppvResourceHandle)
				*ppvResourceHandle = (void *)BVDC_P_RESOURCE_ID_AVAIL;
			else
				*pulHwId = BVDC_P_RESOURCE_ID_AVAIL;
		}
		else
		{
			/* not perfect match, but the requirement is satisfied, use it */
			pCandidate->ulAcquireId = ulAcquireId;
			pCandidate->ulAcquireCntr = 1;
			if (NULL != ppvResourceHandle)
				*ppvResourceHandle = pCandidate->Id.pvHandle;
			else
				*pulHwId = pCandidate->Id.ulHwId;
		}
		return BERR_SUCCESS;
	}
	else
	{
		if (NULL != ppvResourceHandle)
			*ppvResourceHandle = NULL;
		else
			*pulHwId = BVDC_P_HW_ID_INVALID;
		return bQuery ? BVDC_ERR_RESOURCE_NOT_AVAILABLE : BERR_TRACE(BVDC_ERR_RESOURCE_NOT_AVAILABLE);
	}
}

/***************************************************************************
 *
 */
static BERR_Code  BVDC_P_Resource_Reserve_isr
	( BVDC_P_ResourceEntry             *p1stEntry,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulResourceIndex,
	  uint32_t                          ulAcquireId,
	  void                            **ppvResourceHandle,
	  uint32_t                         *pulHwId )
{
	uint32_t  i = 0;
	BVDC_P_ResourceEntry  *pEntry = p1stEntry;

#if !BDBG_DEBUG_BUILD
	BSTD_UNUSED(eType);
#endif

	BDBG_ASSERT((NULL != ppvResourceHandle) || (NULL != pulHwId));
	BDBG_ASSERT(ulResourceIndex < s_aResInfoTbl[eType].ulCount);

	/* Find the entry */
	while(i++ < ulResourceIndex)
		pEntry++;

	BDBG_ASSERT(pEntry->eType == eType);

	if(pEntry->ulAcquireCntr > 0)
	{
		BDBG_ERR(("Resource is already acquired by others"));
		return BERR_TRACE(BVDC_ERR_RESOURCE_NOT_AVAILABLE);
	}

	pEntry->ulAcquireId = ulAcquireId;
	pEntry->ulAcquireCntr = 1;
	if (NULL != ppvResourceHandle)
		*ppvResourceHandle = pEntry->Id.pvHandle;
	else
		*pulHwId = pEntry->Id.ulHwId;

	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code  BVDC_P_Resource_ReserveHandle_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulResourceIndex,
	  uint32_t                          ulAcquireId,
	  void                            **ppvResourceHandle )
{
	BERR_Code eResult;

	BDBG_ENTER(BVDC_P_Resource_ReserveHandle_isr);

	BDBG_ASSERT(eType < BVDC_P_ResourceType_eHandleCntr);
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);

	eResult = BVDC_P_Resource_Reserve_isr(
		hResource->pResourceRecords + hResource->pulIndex1stEntry[eType],
		eType, ulResourceIndex, ulAcquireId, ppvResourceHandle, NULL);

	if (NULL == *ppvResourceHandle)
		BDBG_WRN(("Resource handle [type %d] not available.", (int)eType));

	BDBG_LEAVE(BVDC_P_Resource_ReserveHandle_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * Module assignment rule:
 *    if the resource type has already be acquired for the acquire ID
 * successfully, assign it the same resource module, and increase the
 * acquiring cntr; otherwise, we try to find a perfect match for capabilities;
 * if we can not find a perfect match, the first module that satisfies the
 * capability requirement will be used.
 */
BERR_Code  BVDC_P_Resource_AcquireHandle_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulCapabilities,
	  uint32_t                          ulAcquireId,
	  void                            **ppvResourceHandle,
	  bool								bQuery)
{
	BERR_Code eResult;

	BDBG_ENTER(BVDC_P_Resource_AcquireHandle_isr);

	BDBG_ASSERT(eType < BVDC_P_ResourceType_eHandleCntr);
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);

	eResult = BVDC_P_Resource_Acquire_isr(
		hResource->pResourceRecords + hResource->pulIndex1stEntry[eType],
		eType, ulCapabilities, ulAcquireId, ppvResourceHandle, NULL, bQuery);

	if (NULL == *ppvResourceHandle)
		BDBG_WRN(("Resource handle [type %d, cap %d] not available.", (int)eType, ulCapabilities));

	BDBG_LEAVE(BVDC_P_Resource_AcquireHandle_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * Module assignment rule:
 *    if the resource type has already be acquired for the acquire ID
 * successfully, assign it the same resource module, and increase the
 * acquiring cntr; otherwise, we try to find a perfect match for capabilities;
 * if we can not find a perfect match, the first module that satisfies the
 * capability requirement will be used.
 */
BERR_Code  BVDC_P_Resource_AcquireHwId_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulCapabilities,
	  uint32_t                          ulAcquireId,
	  uint32_t                         *pulHwId,
	  bool                              bQuery)
{
	BERR_Code eResult;

	BDBG_ENTER(BVDC_P_Resource_AcquireHwId_isr);

	BDBG_ASSERT(eType > BVDC_P_ResourceType_eHandleCntr);
	BDBG_ASSERT(eType < BVDC_P_ResourceType_eInvalid);
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);

	eResult = BVDC_P_Resource_Acquire_isr(
		hResource->pResourceRecords + hResource->pulIndex1stEntry[eType],
		eType, ulCapabilities, ulAcquireId, NULL, pulHwId, bQuery);

	BDBG_LEAVE(BVDC_P_Resource_AcquireHwId_isr);
	return bQuery ? eResult : BERR_TRACE(eResult);
}

/* when an acquiring record entry with the handle is found, its ulAcquireCntr
 * is decreased by 1. When ulAcquireCntr reaches 0, the module is really
 * released to be acquired with new acquire ID.
 */
static BERR_Code  BVDC_P_Resource_Release_isr
	( BVDC_P_ResourceEntry             *p1stEntry,
	  BVDC_P_ResourceType               eType,
	  void                             *pvResourceHandle,
	  uint32_t                          ulHwId )
{
	BVDC_P_ResourceEntry  *pEntry;
	uint32_t  ii;
	bool  bFound;

	BDBG_ASSERT((NULL != pvResourceHandle) || (BVDC_P_HW_ID_INVALID != ulHwId));

	/* if this type has already be acquired for this Id, give the same module
	 * to it, and increase the ulAcquireCntr */
	pEntry = p1stEntry;
	for (ii = 0; ii < s_aResInfoTbl[eType].ulCount; ii++)
	{
		BDBG_ASSERT(eType == pEntry->eType);

		bFound = (eType < BVDC_P_ResourceType_eHandleCntr)?
			(pvResourceHandle == pEntry->Id.pvHandle):
			(ulHwId == pEntry->Id.ulHwId);
		if (bFound)
		{
			BDBG_ASSERT(BVDC_P_ACQUIRE_ID_INVALID != pEntry->ulAcquireId);
			BDBG_ASSERT(pEntry->ulAcquireCntr > 0);
			if (pEntry->ulAcquireCntr > 0)
			{
				pEntry->ulAcquireCntr--;
			}
			else
			{
				return BERR_TRACE(BVDC_ERR_RESOURCE_NOT_ACQUIRED);
			}

			/* really free it when ulAcquireCntr reaches 0 */
			if (0 == pEntry->ulAcquireCntr)
			{
				pEntry->bAcquireLock = false;
				pEntry->ulAcquireId = BVDC_P_ACQUIRE_ID_INVALID;
			}
			return BERR_SUCCESS;
		}

		pEntry++;
	}

	/* if we come here, we did not find a match */
	if (pvResourceHandle)
		BDBG_ERR(("Release eType %d, Handle 0x%x", eType, pvResourceHandle));
	else
		BDBG_ERR(("Release eType %d, ulHwId %d", eType, ulHwId));
	/*BDBG_ERR(("Lets cause a core dump %d to see who cause this", 1 / 0));*/
	return BERR_TRACE(BVDC_ERR_RESOURCE_NOT_RECORDED);
}

/***************************************************************************
 * {private}
 *
 * This func is used to release a resource that is represented by a handle.
 *
 * Module realease rule:
 *    when an acquiring record entry with the handle is found, its ulAcquireCntr
 * is decreased by 1. When ulAcquireCntr reaches 0, the module is really
 * released to be acquired with new acquire ID.
 */
BERR_Code  BVDC_P_Resource_ReleaseHandle_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  void                             *pvResourceHandle )
{
	BERR_Code eResult;

	BDBG_ENTER(BVDC_P_Resource_ReleaseHandle_isr);

	BDBG_ASSERT(eType < BVDC_P_ResourceType_eHandleCntr);
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);

	eResult = BVDC_P_Resource_Release_isr(
		hResource->pResourceRecords + hResource->pulIndex1stEntry[eType],
		eType, pvResourceHandle, BVDC_P_HW_ID_INVALID);

	BDBG_LEAVE(BVDC_P_Resource_ReleaseHandle_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * This func is used to release a resource that is NOT represented by a handle.
 *
 * Module realease rule:
 *    when an acquiring record entry with the handle is found, its ulAcquireCntr
 * is decreased by 1. When ulAcquireCntr reaches 0, the module is really
 * released to be acquired with new acquire ID.
 */
BERR_Code  BVDC_P_Resource_ReleaseHwId_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulHwId )
{
	BERR_Code eResult;

	BDBG_ENTER(BVDC_P_Resource_ReleaseHwId_isr);

	BDBG_ASSERT(eType > BVDC_P_ResourceType_eHandleCntr);
	BDBG_ASSERT(eType < BVDC_P_ResourceType_eInvalid);
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);

	eResult = BVDC_P_Resource_Release_isr(
		hResource->pResourceRecords + hResource->pulIndex1stEntry[eType],
		eType, NULL, ulHwId);

	BDBG_LEAVE(BVDC_P_Resource_ReleaseHwId_isr);
	return BERR_TRACE(eResult);
}


/***************************************************************************
 * {private}
 *
 */
uint32_t  BVDC_P_Resource_GetHandleAcquireCntr_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  void                             *pvResourceHandle )
{
	BVDC_P_ResourceEntry  *pEntry;
	uint32_t  ii;

	BDBG_ENTER(BVDC_P_Resource_GetHandleAcquireCntr_isr);

	BDBG_ASSERT(eType < BVDC_P_ResourceType_eHandleCntr);
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);

	pEntry = hResource->pResourceRecords + hResource->pulIndex1stEntry[eType];
	for (ii = 0; ii < s_aResInfoTbl[eType].ulCount; ii++)
	{
		BDBG_ASSERT(eType == pEntry->eType);

		if (pvResourceHandle == pEntry->Id.pvHandle)
		{
			BDBG_LEAVE(BVDC_P_Resource_GetHandleAcquireCntr_isr);
			return pEntry->ulAcquireCntr;
		}

		pEntry++;
	}

	BDBG_LEAVE(BVDC_P_Resource_GetHandleAcquireCntr_isr);
	return 0;
}

/***************************************************************************
 * {private}
 *
 */
uint32_t  BVDC_P_Resource_GetHwIdAcquireCntr_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulHwId )
{
	BVDC_P_ResourceEntry  *pEntry;
	uint32_t  ii;

	BDBG_ENTER(BVDC_P_Resource_GetHwIdAcquireCntr_isr);

	if(eType <= BVDC_P_ResourceType_eHandleCntr)
	{
		BDBG_ASSERT(eType > BVDC_P_ResourceType_eHandleCntr);
		return 0;
	}
	if(eType >= BVDC_P_ResourceType_eInvalid)
	{
		BDBG_ASSERT(eType < BVDC_P_ResourceType_eInvalid);
		return 0;
	}
	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);

	pEntry = hResource->pResourceRecords + hResource->pulIndex1stEntry[eType];
	for (ii = 0; ii < s_aResInfoTbl[eType].ulCount; ii++)
	{
		BDBG_ASSERT(eType == pEntry->eType);

		if (ulHwId == pEntry->Id.ulHwId)
		{
			BDBG_LEAVE(BVDC_P_Resource_GetHwIdAcquireCntr_isr);
			return pEntry->ulAcquireCntr;
		}
		pEntry++;
	}

	BDBG_LEAVE(BVDC_P_Resource_GetHwIdAcquireCntr_isr);
	return 0;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code  BVDC_P_Resource_LockHandle_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  void                             *pvResourceHandle )
{
	BVDC_P_ResourceEntry  *pEntry;
	uint32_t  ii;
	BERR_Code eResult = BVDC_ERR_RESOURCE_NOT_ACQUIRED;

	BDBG_ENTER(BVDC_P_Resource_LockHandle_isr);

	BDBG_ASSERT(eType < BVDC_P_ResourceType_eHandleCntr);

	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);
	pEntry = hResource->pResourceRecords + hResource->pulIndex1stEntry[eType];
	for (ii = 0; ii < s_aResInfoTbl[eType].ulCount; ii++)
	{
		BDBG_ASSERT(eType == pEntry->eType);

		if ((1 <= pEntry->ulAcquireCntr) &&
			(pvResourceHandle == pEntry->Id.pvHandle))
		{
			pEntry->bAcquireLock = true;
			eResult = BERR_SUCCESS;
			break;
		}

		pEntry++;
	}

	BDBG_LEAVE(BVDC_P_Resource_LockHandle_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 */
uint32_t  BVDC_P_Resource_LockHwId_isr
	( BVDC_P_Resource_Handle            hResource,
	  BVDC_P_ResourceType               eType,
	  uint32_t                          ulHwId )
{
	BVDC_P_ResourceEntry  *pEntry;
	uint32_t  ii;
	BERR_Code eResult = BVDC_ERR_RESOURCE_NOT_ACQUIRED;

	BDBG_ENTER(BVDC_P_Resource_GetHwIdAcquireCntr_isr);

	if(eType <= BVDC_P_ResourceType_eHandleCntr)
	{
		BDBG_ASSERT(eType > BVDC_P_ResourceType_eHandleCntr);
		return BERR_TRACE(eResult);
	}
	if(eType >= BVDC_P_ResourceType_eInvalid)
	{
		BDBG_ASSERT(eType < BVDC_P_ResourceType_eInvalid);
		return BERR_TRACE(eResult);
	}

	BDBG_OBJECT_ASSERT(hResource, BVDC_RES);
	pEntry = hResource->pResourceRecords + hResource->pulIndex1stEntry[eType];
	for (ii = 0; ii < s_aResInfoTbl[eType].ulCount; ii++)
	{
		BDBG_ASSERT(eType == pEntry->eType);

		if ((1 <= pEntry->ulAcquireCntr) &&
			(ulHwId == pEntry->Id.ulHwId))
		{
			pEntry->bAcquireLock = true;
			eResult = BERR_SUCCESS;
			break;
		}

		pEntry++;
	}

	BDBG_LEAVE(BVDC_P_Resource_LockHandle_isr);
	return BERR_TRACE(eResult);
}

/* End of file. */
