/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_boxdetect_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/36 $
 * $brcm_Date: 9/2/11 3:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_boxdetect_priv.h $
 * 
 * Hydra_Software_Devel/36   9/2/11 3:55p pntruong
 * SW7425-1165: Updated the number of dnr/lbox/mdi cores for b0.
 *
 * Hydra_Software_Devel/35   9/13/10 4:34p yuxiaz
 * SW7358-4: Added initial VDC support for 7358.
 *
 * Hydra_Software_Devel/34   6/18/10 4:18p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/33   4/7/10 11:24a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/32   4/5/10 3:57p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/31   6/9/09 12:01p syang
 * PR 53683: confirm with 2nd time reading (works for more contents)
 *
 * Hydra_Software_Devel/30   6/3/09 5:10p syang
 * PR 53683: add fast lbox detect
 *
 * Hydra_Software_Devel/29   11/7/07 10:51a pntruong
 * PR 36632: Fixed build errors, and tighten const.
 *
 * Hydra_Software_Devel/28   11/6/07 6:49p syang
 * PR 36632: make box-detect react faster for PC input
 *
 * Hydra_Software_Devel/27   2/28/07 4:49p syang
 * PR 22569: 1). tune anr dynamic seting;  2). add anr demo mode; 3). use
 * hResource in SubRul
 *
 * Hydra_Software_Devel/26   2/23/07 6:27p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/25   2/8/07 1:05p syang
 * PR 27054: 1). add support for stamp detect; 2). stablize cut according
 * to standard case guess.
 *
 * Hydra_Software_Devel/24   2/7/07 12:48p syang
 * PR 27054:  handle lbox disabling without vnet reconfig
 *
 * Hydra_Software_Devel/23   2/1/07 12:09p syang
 * PR 27054: update lbox code to use subrul and new standard sub-module
 * interface func set
 *
 * Hydra_Software_Devel/22   12/18/06 11:34p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   11/22/06 6:03p syang
 * PR 26151: 1st round of debug after re-write shared resource manager.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   11/22/06 3:50p syang
 * PR 26151: re-write shared resource manager. update letter box, mad, dnr
 * shared-scl acquire/release code accordingly
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/15/06 11:56a syang
 * PR 25097: move handle macro to bvdc_common_priv.h
 *
 * Hydra_Software_Devel/20   6/14/06 1:33p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/19   5/26/06 3:36p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/18   3/7/06 4:20p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/17   1/17/06 4:17p hongtaoz
 * PR19082: first compiled for 7400;
 *
 * Hydra_Software_Devel/16   8/18/05 1:11p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/15/05 6:54p pntruong
 * PR 16536: update BCHP_VNET_F_LBOX_0_SRC_SOURCE_* table for 7401.
 *
 * Hydra_Software_Devel/15   8/11/05 11:21a syang
 * PR 16536: correct macro BVDC_P_BOX_DETECT_STATE_RESET messed by last
 * checkin
 *
 * Hydra_Software_Devel/14   8/11/05 10:44a syang
 * PR 16536: update BCHP_VNET_F_LBOX_0_SRC_SOURCE_* table for 7401
 *
 * Hydra_Software_Devel/13   8/10/05 6:20p syang
 * PR 16640: handle RUL drop for lbox init/re-set
 *
 * Hydra_Software_Devel/12   3/17/05 6:35p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/11   12/10/04 3:36p syang
 * PR 13261: re-do the state management to smooth the behaviour  re-
 * enabling
 *
 * Hydra_Software_Devel/10   10/27/04 5:10p syang
 * PR 13106: change  macro BVDC_P_BOX_DETECT_STATE_READ from using "==" to
 * "<="
 *
 * Hydra_Software_Devel/9   10/14/04 6:50p syang
 * PR 10756: Re-organize code so that all window ajustments are done in a
 * central place for better readability and debugablibilty; Added scale
 * factor rounding routine.
 *
 ***************************************************************************/
#ifndef BVDC_BOXDETECT_PRIV_H__
#define BVDC_BOXDETECT_PRIV_H__

#include "bavc.h"
#include "breg_mem.h"      /* Chip register access (memory mapped). */
#include "bvdc_common_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_source_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * {private}
 *
 * BoxDetect Sub-module Overview:
 *
 * When a video content with aspect ratio 16:9 is conveyed on a video stream
 * signal with aspect ratio 4:3, or when 4:3 content is conveyed on 16:9
 * signal, letter box or pillar box algorithm could be used. They patch two
 * black regions on top-bottom or left-right respectively.
 *
 * BoxDetect sub-module is used to detect whether there is letter box or
 * pillar box in the source video stream signal, and the location and size.
 * It is enabled by BVDC_Window_EnableBoxDetect.
 *
 * As other BVDC API functions, box detect enabling and disabling to a
 * window will take effect after BVDC_ApplyChanges, which should call
 * BVDC_P_BoxDetect_ApplyChanges. However, the hardware module is accupied
 * right after BVDC_Window_EnableBoxDetect, and is freed after
 * BVDC_P_BoxDetect_ApplyChanges. BVDC_ApplyChanges does NOT need to call
 * any "validatition" routine for BoxDetect, because after box-detect
 * enabling succeeded for a window, no thing could cause validation failure
 * from BoxDetect sub-module. Two windows that share the same source should
 * use the same box detect module.
 *
 * BVDC_P_BoxDetect_Do_isr is called by window as adjusting the rect before
 * RUL is built at each vsync. It outputs the box black cut rect. It is up
 * to the window to decide whether it is used. BVDC_P_BoxDetect_Do_isr also
 * output BoxDetectInfo and ulCallBckCntr that is needed for call-back.
 * Window should involve call-back when ulCallBckCntr changes.
 *
 * At each source vsync interrupt handler, BVDC_P_BoxDetect_BuildRul_isr
 * should be called when RUL is built. It is OK to called with all window's
 * writer that share the same box detect.
 */

/*-------------------------------------------------------------------------
 * macro used by box-detect sub-module
 */
#define BVDC_P_BOX_DETECT_ID_BIT(id)              (1<<id)
#define BVDC_P_BOX_DETECT_MAX_EDGE                (2047)

#define BVDC_P_BOX_DETECT_FAST_DETECT             1

#ifdef BVDC_P_BOX_DETECT_FAST_DETECT
#define BVDC_P_BOX_DETECT_NUM_ACCUM_VIDEO         2
#define BVDC_P_BOX_DETECT_BREAK_THRESH_VIDEO     20
#define BVDC_P_BOX_DETECT_ERR_THRESH             32
#else
#define BVDC_P_BOX_DETECT_NUM_ACCUM_VIDEO        60
#define BVDC_P_BOX_DETECT_BREAK_THRESH_VIDEO      3
#define BVDC_P_BOX_DETECT_ERR_THRESH              3
#endif
#define BVDC_P_BOX_DETECT_NUM_ACCUM_PC            2
#define BVDC_P_BOX_DETECT_BREAK_THRESH_PC         4

#define BVDC_P_BoxDetect_MuxAddr(hBoxDetect)   (BCHP_VNET_F_LBOX_0_SRC + (hBoxDetect)->eId * sizeof(uint32_t))

#define BVDC_P_BoxDetect_SetVnet_isr(hBoxDetect, ulSrcMuxValue, eVnetPatchMode) \
   BVDC_P_SubRul_SetVnet_isr(&((hBoxDetect)->SubRul), ulSrcMuxValue, eVnetPatchMode)
#define BVDC_P_BoxDetect_UnsetVnet_isr(hBoxDetect) \
   BVDC_P_SubRul_UnsetVnet_isr(&((hBoxDetect)->SubRul))

/*-------------------------------------------------------------------------
 * box detect main context
 */
typedef struct BVDC_P_BoxDetectContext
{
	BDBG_OBJECT(BVDC_BOX)

	/* box detect Id */
	BVDC_P_BoxDetectId               eId;
	uint32_t                         ulRegOffset;
	uint32_t                         ulResetAddr;
	uint32_t                         ulResetMask;

	/* static info from creating */
	BREG_Handle                      hRegister;

	/* */
	BAVC_SourceId                    eSrcId;
	uint32_t                         ulCallBckCntr;
	const BVDC_P_Source_Info        *pCurSrcInfo;

	/* a threshhold of continuous box breaking => real break */
	uint32_t                         ulBreakThresh;
	uint32_t                         ulHorzBreakCnt;
	uint32_t                         ulVertBreakCnt;
	uint32_t                         ulHorzErrCnt;
	uint32_t                         ulVertErrCnt;

	/* current original src size */
	uint32_t                         ulCurSrcWidth;
	uint32_t                         ulCurSrcHeight;

	/* result read from hw and pass to user */
	BVDC_BoxDetectInfo               Box;

	/* cut rectangle if bAutoCutBlack, max if not */
	BVDC_P_Rect                      Cut;

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext             SubRul;

} BVDC_P_BoxDetectContext;


/***************************************************************************
 * private functions
***************************************************************************/
/***************************************************************************
 * {private}
 *
 * BVDC_P_BoxDetect_Create
 *
 * called by BVDC_Open only
 */
BERR_Code BVDC_P_BoxDetect_Create
	( BVDC_P_BoxDetect_Handle *         phBoxDetect,
	  BVDC_P_BoxDetectId                eBoxDetectId,
	  BREG_Handle                       hRegister,
	  BVDC_P_Resource_Handle            hResource );

/***************************************************************************
 * {private}
 *
 * BVDC_P_BoxDetect_Destroy
 *
 * called by BVDC_Close only
 */
BERR_Code BVDC_P_BoxDetect_Destroy
	( BVDC_P_BoxDetect_Handle          hBoxDetect );

/***************************************************************************
 * {private}
 *
 * BVDC_P_BoxDetect_AcquireConnect
 *
 * It is called by BVDC_Window_Validate after changing from diableBox to
 * enablingBox .
 */
BERR_Code BVDC_P_BoxDetect_AcquireConnect
	( BVDC_P_BoxDetect_Handle           hBoxDetect,
	  BAVC_SourceId                     eSrcId,
	  const BVDC_P_Source_Info         *pCurSrcInfo );

/***************************************************************************
 * {private}
 *
 * BVDC_P_BoxDetect_ReleaseConnect_isr
 *
 * It is called after window decided that box-detect is no-longer used by HW in
 * its vnet mode (i.e. it is really shut down and teared off from vnet).
 */
BERR_Code BVDC_P_BoxDetect_ReleaseConnect_isr
	( BVDC_P_BoxDetect_Handle         *phBoxDetect );

/***************************************************************************
 * {private}
 *
 * BVDC_P_BoxDetect_GetStatis_isr
 *
 * BVDC_P_BoxDetect_GetStatis_isr is called by window as adjusting the rect
 * before RUL is built at each vsync. It outputs the box black cut rect by
 * pBoxCut. It also outputs BoxDetectInfo and ulCallBckCntr that is needed
 * for call-back. Window should involve call-back when ulCallBckCntr changes.
 *
 * pBoxCut->ulWidht / ulHeight should contain the full frame src size as
 * input when this function is called.
 */
void BVDC_P_BoxDetect_GetStatis_isr
	( BVDC_P_BoxDetect_Handle           hBoxDetect,
	  BVDC_P_Rect                      *pBoxCut, /* in and out */
	  const BVDC_BoxDetectInfo        **ppBoxInfo, /* out */
	  uint32_t                         *pulCallBckCntr ); /* out */

/***************************************************************************
 * {private}
 *
 * BVDC_P_BoxDetect_BuildRul_isr
 *
 * called by BVDC_Window_BuildRul_isr at every src vsync. It builds RUL for
 * box detect HW module, including initial config and union-white-box-edge
 * resetting, and per-vsync pixel-accept-enabling. It reads HW box detect
 * statistics registers and process the statistics info at every multiple of
 * BVDC_P_BOX_DETECT_NUM_ACCUM vsync.
 *
 * It will reset *phBoxDetect to NULL if the HW module is no longer used by
 * any window.
 *
 * Input:
 *    eVnetState - reader or writer window/vnet state
 *    pPicComRulInfo - the PicComRulInfo that is the shared Picture info by
 *      all sub-modules when they build rul.
 *    bEnable - Whether currently BoxDetect is enabled for the window. This
 *      is passed to handle the case of disabling BoxDetect without vnet
 *      reconfigure.
 */
void BVDC_P_BoxDetect_BuildRul_isr
	( BVDC_P_BoxDetect_Handle       *phBoxDetect,
	  BVDC_P_ListInfo               *pList,
	  BVDC_P_State                   eVnetState,
	  BVDC_P_PicComRulInfo          *pPicComRulInfo,
	  const BVDC_P_Rect             *pSrcOut,
	  bool                           bEnable );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_BOXDETECT_PRIV_H__ */
/* End of file. */
