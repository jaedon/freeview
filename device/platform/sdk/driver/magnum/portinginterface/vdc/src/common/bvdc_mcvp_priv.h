/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_mcvp_priv.h $
* $brcm_Revision: Hydra_Software_Devel/25 $
* $brcm_Date: 5/7/12 9:47p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/src/core/bvdc_mcvp_priv.h $
* 
* Hydra_Software_Devel/25   5/7/12 9:47p vanessah
* SW7418-96: fix the testfeature propogation error
* 
* Hydra_Software_Devel/24   12/20/11 11:40p vanessah
* SW7425-1992: dynamic allocate mcvp buffer according to its mode
* 
* Hydra_Software_Devel/23   11/10/11 9:18p vanessah
* SW7425-1115: add MCVP/MADR seamless transaction
* 
* Hydra_Software_Devel/22   11/2/11 7:14p vanessah
* SW7425-1115: mcvp code cleanup
* 
* Hydra_Software_Devel/21   9/28/11 4:17p pntruong
* SW7425-969: Refactored mcvp/madr code to be multi-instances friendly.
* Adapt to vnet naming changes.
*
* Hydra_Software_Devel/20   9/23/11 5:20p pntruong
* SW7231-383: Correct programming of game mode and delay callback
* reporting for madr/mcvp.
*
* Hydra_Software_Devel/19   8/26/11 4:15p pntruong
* SW7552-43: Deinterlacer hardware requires 3:2 pulldown to be off if
* content is 50hz base.
*
* Hydra_Software_Devel/18   5/26/11 4:47p yuxiaz
* SW7425-415: Fixed MAD issues during 2d and 3d transition.
*
* Hydra_Software_Devel/17   1/6/11 9:47a pntruong
* SW7422-174: Initialized madr with compression enabled to match up with
* default rts for reference usage.
*
* Hydra_Software_Devel/16   12/28/10 5:02p tdo
* SW7422-148: Refactor MCVP to address chips w/wo MCTF
*
* Hydra_Software_Devel/15   12/26/10 5:05p tdo
* SW7422-148: Refactor MCVP to address chips w/wo MCTF
*
* Hydra_Software_Devel/14   12/23/10 5:13p tdo
* SW7422-148: Refactor MCVP to address chips w/wo MCTF
*
* Hydra_Software_Devel/13   12/13/10 2:25p yuxiaz
* SW7422-141: Rework ANR buffer allocation for 1080p 3D input case.
*
* Hydra_Software_Devel/12   11/24/10 1:25p tdo
* SW7422-52: Bring-up MAD-R
*
* Hydra_Software_Devel/11   11/5/10 3:10p vanessah
* SW7422-52:  Two kinds of deinterlacers in the same chip
*
* Hydra_Software_Devel/10   10/18/10 5:05p vanessah
* SW7422-52:  Two deinterlacer on the same chip
*
* Hydra_Software_Devel/9   8/26/10 2:38p vanessah
* SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
* mode
*
* Hydra_Software_Devel/SW7420-965/1   8/20/10 1:29p vanessah
* SW7420-965:  MCVP game mode support
*
* Hydra_Software_Devel/SW7420-588/1   8/9/10 7:23p vanessah
* SW7420-588:  panscan to zoom video loss
*
* Hydra_Software_Devel/7   6/18/10 4:22p rpan
* SW7400-2763: New BVN resource management.
*
* Hydra_Software_Devel/6   5/7/10 7:19p albertl
* SW7125-364: Changed dirty bits to use union structure to avoid type-pun
* warnings
*
* Hydra_Software_Devel/5   4/19/10 10:17p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic. Move
* BDBG_OBJECT_ID_DECLARE private header files instead of .c.
*
* Hydra_Software_Devel/4   4/7/10 11:32a tdo
* SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
*
* Hydra_Software_Devel/3   4/5/10 4:11p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic
*
* Hydra_Software_Devel/2   1/28/10 11:11a syang
* SW7405-3829: add option for app to shrink src width to use deinterlace
*
* Hydra_Software_Devel/1   3/16/09 11:12p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/1   1/23/09 8:55p syang
* PR 45785, PR 45789: init MCVP implementation
*
***************************************************************************/
#ifndef BVDC_MCVP_PRIV_H__
#define BVDC_MCVP_PRIV_H__

#include "bavc.h"
#include "breg_mem.h"      /* Chip register access (memory mapped). */
#include "bvdc_common_priv.h"
#include "bvdc_bufferheap_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_window_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

	BDBG_OBJECT_ID_DECLARE(BVDC_ANR);
	BDBG_OBJECT_ID_DECLARE(BVDC_MDI);

/***************************************************************************
* {private}
*
* Mcvp Sub-module Overview:
*
*/

/*-------------------------------------------------------------------------
* macro used by mcvp sub-module
*/
#define BVDC_P_MCVP_BUFFER_MAX_COUNT            (4)
#define BVDC_P_ANR_NUM_ACCUM                    (1)

/* MCVP Versions */
#define BVDC_P_MCVP_VER_1                       (1) /* 7420 */
#define BVDC_P_MCVP_VER_2                       (2) /* 7422Ax/7425Ax */
#define BVDC_P_MCVP_VER_3                       (3) /* 7231Ax/7344Ax/7346Ax/7358Ax/7552Ax */


/****************************************************************************
* Mcvp dirty bits to makr RUL building and executing dirty.
*/
typedef union
{
	struct {
		uint32_t                           bSize           : 1;
		uint32_t                           bPrevBypass     : 1;
	} stBits;

	uint32_t aulInts [BVDC_P_DIRTY_INT_ARRAY_SIZE];
} BVDC_P_McvpDirtyBits;


/*-------------------------------------------------------------------------
* mcvp main context
*/
typedef struct BVDC_P_McvpContext
{
	BDBG_OBJECT(BVDC_MVP)

	/* mcvp Id */
	BVDC_P_McvpId                      eId;
	uint32_t                           ulMaxWidth; /* max width limited by line buf size */
	uint32_t                           ulMaxHeight; /* max height limited by RTS */
	uint32_t                           ulRegOffset;

	/* Core & Vnet Channel Reset */
	uint32_t                           ulCoreResetAddr;
	uint32_t                           ulCoreResetMask;
	uint32_t                           ulVnetResetAddr;
	uint32_t                           ulVnetResetMask;
	uint32_t                           ulVnetMuxAddr;
	uint32_t                           ulVnetMuxValue;

	/* static info from creating */
	BREG_Handle                        hRegister;

	/* from acquireConnect */
	BVDC_Heap_Handle                   hHeap;
	BVDC_P_WindowId                    eWinId;

	/* sub-modules */
	BVDC_P_Hscaler_Handle              hHscaler;
	BVDC_P_Anr_Handle                  hAnr;
	BVDC_P_Mcdi_Handle                 hMcdi;

	uint32_t                           ulPrevHSize;
	uint32_t                           ulPrevVSize;
	bool                               bAnr;

	/* buffers */
	uint32_t                           ulBufCnt;
	/*BVDC_P_HeapNodePtr               apHeapNode[BVDC_P_MCVP_BUFFER_MAX_COUNT];*/

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext               SubRul;

	bool                               bBufIsContinuous;

	BVDC_P_VnetMode                    stMcvpMode;
	bool                               bReconf;
} BVDC_P_McvpContext;


/***************************************************************************
* private functions
***************************************************************************/

#define BVDC_P_Mcvp_MuxAddr(hMcvp)      (hMcvp->ulVnetMuxAddr)
#define BVDC_P_Mcvp_PostMuxValue(hMcvp) (hMcvp->ulVnetMuxValue)
#define BVDC_P_Mcvp_SetVnet_isr(hMcvp, ulSrcMuxValue, eVnetPatchMode) \
	BVDC_P_SubRul_SetVnet_isr(&((hMcvp)->SubRul), ulSrcMuxValue, eVnetPatchMode)
#define BVDC_P_Mcvp_UnsetVnet_isr(hMcvp) \
	BVDC_P_SubRul_UnsetVnet_isr(&((hMcvp)->SubRul))


/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_Create
*
* called by BVDC_Open only
*/
BERR_Code BVDC_P_Mcvp_Create
	( BVDC_P_Mcvp_Handle *           phMcvp,
	BVDC_P_McvpId                    eMcvpId,
	BREG_Handle                      hRegister,
	BVDC_P_Resource_Handle           hResource );

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_Destroy
*
* called by BVDC_Close only
*/
BERR_Code BVDC_P_Mcvp_Destroy
	( BVDC_P_Mcvp_Handle               hMcvp );

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_AcquireConnect
*
* It is called by BVDC_Window_Validate after changing from disable mcvp to
* enable mcvp.
*/
BERR_Code BVDC_P_Mcvp_AcquireConnect
	( BVDC_P_Mcvp_Handle               hMcvp,
	BVDC_Heap_Handle                   hHeap,
	BVDC_P_WindowId                    eWinId,
	BVDC_P_Window_Info                *pCurInfo,
	const BVDC_Anr_Settings           *pAnrSetting,
	BVDC_P_Source_InfoPtr              pCurSrcInfo);

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_ReleaseConnect_isr
*
* It is called after window decided that mcvp is no-longer used by HW in its
* vnet mode (i.e. it is really shut down and teared off from vnet).
*/
BERR_Code BVDC_P_Mcvp_ReleaseConnect_isr
	( BVDC_P_Mcvp_Handle              *phMcvp );

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_SetVnetAllocBuf_isr
*
* Called by BVDC_P_*_BuildRul_isr to setup for joinning into vnet (including
* optionally acquiring loop-back) and allocate buffers
*/
void BVDC_P_Mcvp_SetVnetAllocBuf_isr
	( BVDC_P_Mcvp_Handle               hMcvp,
	  uint32_t                         ulSrcMuxValue,
	  BVDC_P_VnetPatch                 eVnetPatchMode,
	  BVDC_P_BufferHeapId              ePixelBufHeapId,
	  BVDC_P_BufferHeapId              eQmBufHeapId,
	  uint32_t                         ulBufCnt,
	  BVDC_P_VnetMode                 *pMcvpMode,
	  bool                             bRfcgVnet);

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_UnsetVnetFreeBuf_isr
*
* called by BVDC_P_Window_UnsetWriter(Reader)Vnet_isr to to release the
* potentially used loop-back, and free buffers
*/
void BVDC_P_Mcvp_UnsetVnetFreeBuf_isr
	( BVDC_P_Mcvp_Handle                hMcvp );

/***************************************************************************
* {private}
*
* BVDC_P_MCVP_SetInfo_isr
*
* called by BVDC_P_Window_Writer(Reader)_isr to to detect size difference between
* two continuous rul
*/
void BVDC_P_MCVP_SetInfo_isr
	(BVDC_P_Mcvp_Handle                 hMcvp,
	BVDC_Window_Handle                 hWindow,
	BVDC_P_PictureNode                *pPicture);

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_BuildRul_isr
*
* called by BVDC_Window_BuildRul_isr at every src or vec vsync (depending on
* whether reader side or writer side is using this module)
*
* Input:
*    eVnetState - reader or writer window/vnet state
*    pPicComRulInfo - the PicComRulInfo that is the shared Picture info by
*      all sub-modules when they build rul.
*/
void BVDC_P_Mcvp_BuildRul_isr(
	BVDC_P_Mcvp_Handle                 hMcvp,
	BVDC_P_ListInfo                   *pList,
	BVDC_P_State                       eVnetState,
	BVDC_P_WindowContext              *pWindow,
	BVDC_P_PictureNode                *pPicture );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_MCVP_PRIV_H__ */
/* End of file. */
