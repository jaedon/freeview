/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_mcvp_priv.c $
* $brcm_Revision: Hydra_Software_Devel/64 $
* $brcm_Date: 9/13/12 9:57a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_mcvp_priv.c $
* 
* Hydra_Software_Devel/64   9/13/12 9:57a tdo
* SW7420-2351: remove allocation of QM buffer for 7420
*
* Hydra_Software_Devel/63   8/6/12 1:43p vanessah
* SW7425-3462: sync anr mode
*
* Hydra_Software_Devel/62   7/26/12 4:27p vanessah
* SW7425-3462: ANR syncup with mcvp
*
* Hydra_Software_Devel/61   7/23/12 3:15p yuxiaz
* SW7344-344: Fixed BVDC_P_Mcvp_AllocBuf_is call.
*
* Hydra_Software_Devel/60   7/17/12 2:06p vanessah
* SW7435-265: program EoP_mask
*
* Hydra_Software_Devel/59   5/11/12 1:49p vanessah
* SW7346-819: mcdi xchroma setting depends on source
*
* Hydra_Software_Devel/58   5/7/12 9:47p vanessah
* SW7418-96: fix the testfeature propogation error
*
* Hydra_Software_Devel/57   5/7/12 6:23p vanessah
* SW7418-96: fix the testfeature propogation error
*
* Hydra_Software_Devel/56   5/3/12 9:24p vanessah
* SW7425-2873: transcode path fmt stress test
*
* Hydra_Software_Devel/55   3/26/12 10:56a yuxiaz
* SW7425-2753: Fixed BVN errors for 3d conversion.
*
* Hydra_Software_Devel/54   1/24/12 4:35p vanessah
* SW7358-219: release Mcdi buffer when bypassMcvp
*
* Hydra_Software_Devel/53   1/16/12 9:54p vanessah
* SW7552-178: SW7552-182: remove breconfig and pixel fmt restriction on
* SW_INIT
*
* Hydra_Software_Devel/52   1/13/12 11:54a vanessah
* SW7552-182: fix the stress test failure
*
* Hydra_Software_Devel/51   1/12/12 5:40p vanessah
* SW7425-2162: change 7425 MADR QM buffer allocation
*
* Hydra_Software_Devel/50   1/12/12 10:32a vanessah
* SW7552-182: SW7552-178: add more SW_INIT needed case
*
* Hydra_Software_Devel/49   1/9/12 11:31p vanessah
* SW7552-178: adding condition for init start
*
* Hydra_Software_Devel/48   1/6/12 2:00p vanessah
* SW7552-182: SW7552-178: add SW_INIT on MADR mode switch from bypass to
* mdi_only
*
* Hydra_Software_Devel/47   12/20/11 11:40p vanessah
* SW7425-1992: dynamic allocate mcvp buffer according to its mode
*
* Hydra_Software_Devel/46   11/22/11 9:34a vanessah
* SW7425-1115: SW7358-180: alignment on submodule usage varibles.
*
* Hydra_Software_Devel/45   11/10/11 9:18p vanessah
* SW7425-1115: add MCVP/MADR seamless transaction
*
* Hydra_Software_Devel/44   11/2/11 7:13p vanessah
* SW7425-1115: mcvp code cleanup
*
* Hydra_Software_Devel/43   9/28/11 4:17p pntruong
* SW7425-969: Refactored mcvp/madr code to be multi-instances friendly.
* Adapt to vnet naming changes.
*
* Hydra_Software_Devel/42   9/23/11 5:19p pntruong
* SW7231-383: Correct programming of game mode and delay callback
* reporting for madr/mcvp.
*
* Hydra_Software_Devel/41   8/26/11 4:15p pntruong
* SW7552-43: Deinterlacer hardware requires 3:2 pulldown to be off if
* content is 50hz base.
*
* Hydra_Software_Devel/40   6/20/11 5:49p tdo
* SW7425-104: Rename the old repeat field flag to match avc structure and
* use the new added bRepeatField flag for ANR.
*
* Hydra_Software_Devel/39   5/26/11 4:47p yuxiaz
* SW7425-415: Fixed MAD issues during 2d and 3d transition.
*
* Hydra_Software_Devel/38   5/19/11 1:56p yuxiaz
* SW7422-446: Fixed source orientation override.
*
* Hydra_Software_Devel/37   4/18/11 8:21a vanessah
* SW7425-374: turn on osd for Mcdi debug
*
* Hydra_Software_Devel/36   4/13/11 3:37p tdo
* SW7231-104: Video lost on HDMI/Component outputs when playback a stream
* with Box Detect option
*
* Hydra_Software_Devel/35   4/4/11 4:15p tdo
* SW7425-104: Not reseting noise buffer when pause
*
* Hydra_Software_Devel/34   3/29/11 1:51p tdo
* SW7344-36: Playback trick modes cause Video flicker and bands
*
* Hydra_Software_Devel/33   1/12/11 4:34p yuxiaz
* SW7335-1133: Added more debug messages for VDC heap debugging.
*
* Hydra_Software_Devel/32   1/7/11 4:47p yuxiaz
* SW7422-174: Added DCX support to MCVP and MADR.
*
* Hydra_Software_Devel/31   1/6/11 9:47a pntruong
* SW7422-174: Initialized madr with compression enabled to match up with
* default rts for reference usage.
*
* Hydra_Software_Devel/30   12/28/10 5:02p tdo
* SW7422-148: Refactor MCVP to address chips w/wo MCTF
*
* Hydra_Software_Devel/29   12/23/10 7:12p tdo
* SW7422-148: Fix compiling error
*
* Hydra_Software_Devel/28   12/23/10 5:12p tdo
* SW7422-148: Refactor MCVP to address chips w/wo MCTF
*
* Hydra_Software_Devel/27   12/16/10 1:09p vanessah
* SW7425-58:  New MCDI feature support
*
* Hydra_Software_Devel/26   12/13/10 2:24p yuxiaz
* SW7422-141: Rework ANR buffer allocation for 1080p 3D input case.
*
* Hydra_Software_Devel/25   12/8/10 5:44p tdo
* SW7422-36: Correctly program MCVP 3D mode based on source and display
* orientation
*
* Hydra_Software_Devel/24   11/24/10 1:24p tdo
* SW7422-52: Bring-up MAD-R
*
* Hydra_Software_Devel/23   11/15/10 4:41p tdo
* SW7422-36: Handle "left only" method for BVN internal data path
*
* Hydra_Software_Devel/22   11/12/10 11:50a hongtaoz
* SW7422-13: program MCDI/ANR only when enabled;
*
* Hydra_Software_Devel/21   11/11/10 12:44p tdo
* SW7422-13: Only turn on ANR when mode is set to enable
*
* Hydra_Software_Devel/20   11/10/10 11:40a tdo
* SW7422-13: Fix IT version for 7422, Fix bus error and segfault with
* MCVP enable.
*
* Hydra_Software_Devel/19   11/5/10 3:09p vanessah
* SW7422-52:  Two kinds of deinterlacers in the same chip
*
* Hydra_Software_Devel/18   11/3/10 10:24p tdo
* SW7420-1224: Add trick mode setting to MCDI
*
* Hydra_Software_Devel/17   10/28/10 4:13p tdo
* SW7420-1203: Video delay is not changed when we toggle the MCVP between
* enable/disable state
*
* Hydra_Software_Devel/16   10/25/10 7:08p tdo
* SW7420-1203: Video delay is not changed when we toggle the MCVP between
* enable/disable state
*
* Hydra_Software_Devel/15   10/18/10 5:04p vanessah
* SW7422-52:  Two deinterlacer on the same chip
*
* Hydra_Software_Devel/14   9/2/10 1:37p vanessah
* SW7422-52:  MAD + MCVP both on 7422
*
* Hydra_Software_Devel/13   8/26/10 2:38p vanessah
* SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
* mode
*
* Hydra_Software_Devel/SW7420-965/1   8/20/10 1:29p vanessah
* SW7420-965:  MCVP game mode support
*
* Hydra_Software_Devel/SW7420-588/1   8/9/10 7:23p vanessah
* SW7420-588:  panscan to zoom video loss
*
* Hydra_Software_Devel/11   6/18/10 4:22p rpan
* SW7400-2763: New BVN resource management.
*
* Hydra_Software_Devel/10   5/7/10 7:19p albertl
* SW7125-364: Changed dirty bits to use union structure to avoid type-pun
* warnings
*
* Hydra_Software_Devel/9   4/19/10 10:17p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic. Move
* BDBG_OBJECT_ID_DECLARE private header files instead of .c.
*
* Hydra_Software_Devel/8   4/7/10 11:32a tdo
* SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
*
* Hydra_Software_Devel/7   4/5/10 4:11p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic
*
* Hydra_Software_Devel/6   1/28/10 11:11a syang
* SW7405-3829: add option for app to shrink src width to use deinterlace
*
* Hydra_Software_Devel/5   9/3/09 10:32p pntruong
* SW3548-2373: Rolled back PR56964.
*
* Hydra_Software_Devel/4   7/21/09 2:33p syang
* PR 56964: take off the "connect drainer, en and then dis" step  in
* vnet-drain code
*
* Hydra_Software_Devel/3   4/17/09 11:58a pntruong
* PR54064: Refactored common code for defered callback til bvn shutdown.
* Improved readability and extensibility of dirty bits.
*
* Hydra_Software_Devel/2   3/17/09 1:05a tdo
* PR45785, PR45789: Fix compiling errors
*
* Hydra_Software_Devel/1   3/16/09 11:11p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/2   2/18/09 3:40p syang
* PR 45785: fix 7400 comipling
*
* Hydra_Software_Devel/7420_mcvp/1   1/23/09 8:56p syang
* PR 45785, PR 45789: init MCVP implementation
*
***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "brdc.h"
#include "bvdc.h"
#include "bchp_vnet_f.h"
#include "bvdc_resource_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_mcvp_priv.h"
#include "bvdc_hscaler_priv.h"
#include "bvdc_anr_priv.h"
#include "bvdc_mcdi_priv.h"
#include "bchp_mmisc.h"
#include "bvdc_vnet_priv.h"

#if (BVDC_P_SUPPORT_MCVP)
#if (BVDC_P_SUPPORT_MCVP_VER<=1)
#include "bchp_dmisc.h"
#include "bchp_mcvp_top_0.h"
#else
#include "bchp_mvp_top_0.h"
#endif

BDBG_MODULE(BVDC_MCVP);
BDBG_OBJECT_ID(BVDC_MVP);

/***************************************************************************
 * The followings are exported to other sub-modules inside VDC
 ***************************************************************************/
#if (BVDC_P_SUPPORT_MCVP_VER > BVDC_P_MCVP_VER_1) /* beyond 7420 */
#define BVDC_P_MAKE_MVP(pMcvp, id)                                                \
{                                                                                 \
	(pMcvp)->ulCoreResetAddr = BCHP_MMISC_SW_INIT;                                \
	(pMcvp)->ulCoreResetMask = BCHP_MMISC_SW_INIT_MVP_##id##_MASK;                \
	(pMcvp)->ulVnetResetAddr = BCHP_MMISC_VNET_B_CHANNEL_SW_INIT;                 \
	(pMcvp)->ulVnetResetMask = BCHP_MMISC_VNET_B_CHANNEL_SW_INIT_MVP_##id##_MASK; \
	(pMcvp)->ulVnetMuxAddr   = BCHP_VNET_F_MVP_##id##_SRC;                        \
	(pMcvp)->ulVnetMuxValue  = BCHP_VNET_B_CAP_0_SRC_SOURCE_MVP_##id;             \
}
#else
#define BVDC_P_MAKE_MVP(pMcvp, id)                                                \
{                                                                                 \
	(pMcvp)->ulCoreResetAddr = BCHP_DMISC_SOFT_RESET;                             \
	(pMcvp)->ulCoreResetMask = BCHP_DMISC_SOFT_RESET_MCVP_0_MASK;                 \
	(pMcvp)->ulVnetResetAddr = BCHP_MMISC_VNET_B_CHANNEL_RESET;                   \
	(pMcvp)->ulVnetResetMask = BCHP_MMISC_VNET_B_CHANNEL_RESET_MCVP_0_RESET_MASK; \
	(pMcvp)->ulVnetMuxAddr   = BCHP_VNET_F_MCVP_0_SRC;                            \
	(pMcvp)->ulVnetMuxValue  = BCHP_VNET_B_CAP_0_SRC_SOURCE_MCVP_0;               \
}
#endif

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_Create
*
* called by BVDC_Open only
*/
BERR_Code BVDC_P_Mcvp_Create
	( BVDC_P_Mcvp_Handle *             phMcvp,
	  BVDC_P_McvpId                    eMcvpId,
	  BREG_Handle                      hRegister,
	  BVDC_P_Resource_Handle           hResource )
{
	BVDC_P_McvpContext  *pMcvp;
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Mcvp_Create);

	/* in case creation failed */
	BDBG_ASSERT(phMcvp);
	*phMcvp = NULL;

	pMcvp = (BVDC_P_McvpContext *) (BKNI_Malloc(sizeof(BVDC_P_McvpContext)));
	if( pMcvp )
	{
		/* init the context */
		BKNI_Memset((void*)pMcvp, 0x0, sizeof(BVDC_P_McvpContext));
		BDBG_OBJECT_SET(pMcvp, BVDC_MVP);
		pMcvp->eId = eMcvpId;
		pMcvp->hRegister = hRegister;
		pMcvp->eWinId = BVDC_P_WindowId_eUnknown;
		pMcvp->ulMaxWidth  = BFMT_1080I_WIDTH;
		pMcvp->ulMaxHeight = BFMT_1080I_HEIGHT;
		pMcvp->bReconf = false;

#if (BVDC_P_SUPPORT_MCVP > 1)
		pMcvp->ulRegOffset = ((pMcvp->eId - BVDC_P_McvpId_eMcvp0) *
			(BCHP_MVP_TOP_1_REG_START - BCHP_MVP_TOP_0_REG_START));
#endif

		switch(pMcvp->eId)
		{
#if (BVDC_P_SUPPORT_MCVP > 0)
		case BVDC_P_McvpId_eMcvp0:
			BVDC_P_MAKE_MVP(pMcvp, 0);
			break;
#endif
#if (BVDC_P_SUPPORT_MCVP > 1)
		case BVDC_P_McvpId_eMcvp1:
			BVDC_P_MAKE_MVP(pMcvp, 1);
			break;
#endif
#if (BVDC_P_SUPPORT_MCVP > 2)
		case BVDC_P_McvpId_eMcvp2:
			BVDC_P_MAKE_MVP(pMcvp, 2);
			break;
#endif
#if (BVDC_P_SUPPORT_MCVP > 3)
		case BVDC_P_McvpId_eMcvp3:
			BVDC_P_MAKE_MVP(pMcvp, 3);
			break;
#endif
#if (BVDC_P_SUPPORT_MCVP > 4)
		case BVDC_P_McvpId_eMcvp4:
			BVDC_P_MAKE_MVP(pMcvp, 4);
			break;
#endif
		default:
			BDBG_ERR(("Need to handle BVDC_P_McvpId_eMcvp%d", pMcvp->eId));
			BDBG_ASSERT(0);
			break;
		}

		/* init the SubRul sub-module */
		BVDC_P_SubRul_Init(&(pMcvp->SubRul), BVDC_P_Mcvp_MuxAddr(pMcvp),
			BVDC_P_Mcvp_PostMuxValue(pMcvp), BVDC_P_DrainMode_eBack,
			BVDC_P_ANR_NUM_ACCUM, hResource);

		/* create sub-modules of MCVP */
		eResult  = BERR_TRACE(BVDC_P_Hscaler_Create(&pMcvp->hHscaler,
			(BVDC_P_HscalerId)eMcvpId, hResource, hRegister));
#if (BVDC_P_SUPPORT_MANR)
		eResult |= BERR_TRACE(BVDC_P_Anr_Create(&pMcvp->hAnr,
			(BVDC_P_AnrId)eMcvpId, hRegister, hResource));
#endif
		eResult |= BERR_TRACE(BVDC_P_Mcdi_Create(&pMcvp->hMcdi,
			(BVDC_P_McdiId)eMcvpId, hRegister, hResource));

		if (BERR_SUCCESS == eResult)
		{
			*phMcvp = pMcvp;
		}
		else
		{
			BVDC_P_Mcvp_Destroy(pMcvp);
		}
	}
	else
	{
		eResult = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	BDBG_LEAVE(BVDC_P_Mcvp_Create);
	return eResult;
}

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_Destroy
*
* called by BVDC_Close only
*/
BERR_Code BVDC_P_Mcvp_Destroy
	( BVDC_P_Mcvp_Handle               hMcvp )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Mcvp_Destroy);
	BDBG_OBJECT_ASSERT(hMcvp, BVDC_MVP);

	/* destroy sub-modules */
	if (hMcvp->hHscaler)
		BVDC_P_Hscaler_Destroy(hMcvp->hHscaler);
	if (hMcvp->hAnr)
		BVDC_P_Anr_Destroy(hMcvp->hAnr);
	if (hMcvp->hMcdi)
		BVDC_P_Mcdi_Destroy(hMcvp->hMcdi);

	BDBG_OBJECT_DESTROY(hMcvp, BVDC_MVP);
	/* it is gone afterwards !!! */
	BKNI_Free((void*)hMcvp);

	BDBG_LEAVE(BVDC_P_Mcvp_Destroy);
	return BERR_TRACE(eResult);
}

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_Init
*
* called by BVDC_P_Mcvp_AcquireConnect only
*/
static void BVDC_P_Mcvp_Init
	( BVDC_P_Mcvp_Handle               hMcvp )
{
	uint32_t  ulReg;

	BDBG_ENTER(BVDC_P_Mcvp_Init);
	BDBG_OBJECT_ASSERT(hMcvp, BVDC_MVP);

#if (BVDC_P_SUPPORT_MCVP_VER >= 2)
	ulReg = BREG_Read32(hMcvp->hRegister, BCHP_MVP_TOP_0_HW_CONFIG + hMcvp->ulRegOffset);
	hMcvp->bAnr  = BCHP_GET_FIELD_DATA(ulReg, MVP_TOP_0_HW_CONFIG, ANR) ? true : false;
#else
	BSTD_UNUSED(ulReg);
	hMcvp->bAnr  = true;
#endif

	BDBG_LEAVE(BVDC_P_Mcvp_Init);
	return;
}

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_AcquireConnect
*
* It is called by BVDC_Window_Validate after changing from disable mcvp to
* enable mcvp.
*/
BERR_Code BVDC_P_Mcvp_AcquireConnect
	( BVDC_P_Mcvp_Handle                 hMcvp,
	  BVDC_Heap_Handle                   hHeap,
	  BVDC_P_WindowId                    eWinId,
	  BVDC_P_Window_Info                *pCurInfo,
	  const BVDC_Anr_Settings           *pAnrSetting,
	  BVDC_P_Source_InfoPtr              pCurSrcInfo )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Mcvp_AcquireConnect);

	hMcvp->eWinId = eWinId;
	hMcvp->hHeap = hHeap;

	BVDC_P_Mcvp_Init(hMcvp);
	BVDC_P_Hscaler_Init(hMcvp->hHscaler);
	BVDC_P_Mcdi_Init(hMcvp->hMcdi);

	BVDC_P_Mcdi_Init_Chroma_DynamicDefault(hMcvp->hMcdi, &pCurInfo->stMadSettings.stChromaSettings,
			pCurSrcInfo->pFmtInfo, BVDC_P_SRC_IS_MPEG(pCurInfo->hSource->eId));
	/* Sync up the data that will be propage down to Mcdi */
	pCurInfo->stMadSettings.stTestFeature1.bEnable = hMcvp->hMcdi->stTestFeature1.bEnable;
	pCurInfo->stMadSettings.stTestFeature1.ulBitsPerPixel = hMcvp->hMcdi->stTestFeature1.ulBitsPerPixel;

	if(hMcvp->bAnr)
	{
		BVDC_P_Anr_SetUsrSettingPtr(hMcvp->hAnr, pAnrSetting);
		BVDC_P_Anr_SetSrcInfo(hMcvp->hAnr, pCurSrcInfo);
	}

	BDBG_LEAVE(BVDC_P_Mcvp_AcquireConnect);
	return BERR_TRACE(eResult);
}

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_ReleaseConnect_isr
*
* It is called after window decided that mcvp is no-longer used by HW in its
* vnet mode (i.e. it is really shut down and teared off from vnet).
*/
BERR_Code BVDC_P_Mcvp_ReleaseConnect_isr
	( BVDC_P_Mcvp_Handle              *phMcvp )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Mcvp_ReleaseConnect_isr);

	/* handle validation */
	if (NULL != *phMcvp)
	{
		BDBG_OBJECT_ASSERT(*phMcvp, BVDC_MVP);

		/* another win might still using it */
		BVDC_P_Resource_ReleaseHandle_isr(
			BVDC_P_SubRul_GetResourceHandle_isr(&(*phMcvp)->SubRul),
			BVDC_P_ResourceType_eMcvp, (void *)(*phMcvp));

		/* this makes win to stop calling mcvp code */
		*phMcvp = NULL;
	}

	BDBG_LEAVE(BVDC_P_Mcvp_ReleaseConnect_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_FreeBuf_isr
*
* Called by BVDC_P_Mcvp_UnsetVnetFreeBuf_isr/BVDC_P_Mcvp_SetVnetAllocBuf_isr
* to deallocate buffers
*/

static void BVDC_P_Mcvp_FreeBuf_isr
	( BVDC_P_Mcvp_Handle                hMcvp,
	  bool                              bBypassMcvp)
{
	bool bMadr=hMcvp->hMcdi->bMadr;

	BDBG_ENTER(BVDC_P_Mcvp_FreeBuf_isr);
	BDBG_OBJECT_ASSERT(hMcvp, BVDC_MVP);

	/* free internally used buffers if no win is using this MCVP. */
	if ((0 == BVDC_P_SubRul_GetWinsActFlags(&(hMcvp->SubRul)))
		|| bBypassMcvp)
	{
		BVDC_P_HeapNodePtr *ppHeapNode, *ppQmHeapNode;

		/* we store mcvp */
		ppHeapNode = &(hMcvp->hMcdi->apHeapNode[0]);
		if(NULL != *ppHeapNode)
		{
			BVDC_P_BUF_MSG(("Mcvp[%d] free	%d buffers (%s)", hMcvp->eId, hMcvp->ulBufCnt,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME((*ppHeapNode)->pHeapInfo->eBufHeapId)));
			BVDC_P_BufferHeap_FreeBuffers_isr(hMcvp->hHeap,
				ppHeapNode, hMcvp->ulBufCnt, hMcvp->bBufIsContinuous);
			*ppHeapNode = NULL;
		}

		ppQmHeapNode = &(hMcvp->hMcdi->apQmHeapNode[0]);
		if(NULL != *ppQmHeapNode)
		{
			BVDC_P_BUF_MSG(("Mcvp[%d] free	%d QM buffers (%s)",
				hMcvp->eId,
				bMadr? BVDC_P_MAD_QM_BUFFER_COUNT : BVDC_P_MCDI_QM_BUFFER_COUNT,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME((*ppQmHeapNode)->pHeapInfo->eBufHeapId)));

			BVDC_P_BufferHeap_FreeBuffers_isr(hMcvp->hHeap, ppQmHeapNode,
				bMadr? BVDC_P_MAD_QM_BUFFER_COUNT : BVDC_P_MCDI_QM_BUFFER_COUNT,
				false);
			*ppQmHeapNode = NULL;
#if BCHP_MDI_TOP_0_QM_FIELD_MSTART_0
		(hMcvp->hMcdi->apQmHeapNode[1]) = NULL;
#endif
		}
	}

	BDBG_LEAVE(BVDC_P_Mcvp_FreeBuf_isr);
}
/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_AllocBuf_isr
*
*/

static void BVDC_P_Mcvp_AllocBuf_isr
		( BVDC_P_Mcvp_Handle               hMcvp,
		  uint32_t                         ulBufCnt,
		  BVDC_P_BufferHeapId              ePixelBufHeapId,
		  BVDC_P_BufferHeapId              eQmBufHeapId)
{
	BVDC_P_HeapNodePtr *ppHeapNode, *ppQmHeapNode;
	BERR_Code  eResult = BERR_SUCCESS;
	bool  bMadr = hMcvp->hMcdi->bMadr;

	if(hMcvp->hMcdi->apHeapNode[0] ||
		(BVDC_P_BufferHeapId_eUnknown == ePixelBufHeapId)||
		(BVDC_P_BufferHeapId_eUnknown == eQmBufHeapId))
		return;

	/* allocate internally used buffers. Currently this is done during VNET reconfig.
	* notice that mcdi and anr could be turned off seperately inside MCVP, the top
	* module MCVP is enabled as long as one of them are enabled. Also even if anr is
	* enabled, it might bypass mctf and only let AND function. */
	BVDC_P_BUF_MSG(("Mcvp[%d] alloc %d buffers (%s) %s", hMcvp->eId, ulBufCnt,
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(ePixelBufHeapId),hMcvp->bBufIsContinuous ? "continuous" : "non-continuous" ));

	ppHeapNode = &(hMcvp->hMcdi->apHeapNode[0]);
	eResult = BVDC_P_BufferHeap_AllocateBuffers_isr(hMcvp->hHeap, ppHeapNode,
		ulBufCnt, hMcvp->bBufIsContinuous, ePixelBufHeapId, BVDC_P_BufferHeapId_eUnknown);
	/* Not enough memory, dump out configuration */
	if(eResult == BERR_OUT_OF_DEVICE_MEMORY)
	{
		BDBG_ERR(("Mcvp[%d] Not enough memory for MCVP pixel field buffers! Configuration:", hMcvp->eId));
		BDBG_ERR(("Mcvp[%d] BufferCnt: %d", hMcvp->eId, ulBufCnt));
	}
	BDBG_ASSERT(BERR_SUCCESS == eResult);
	hMcvp->ulBufCnt = ulBufCnt;

#if (BVDC_P_SUPPORT_MADR) || (BVDC_P_SUPPORT_MCDI_VER >= BVDC_P_MCDI_VER_2)
	/* Allocate QM field buffers */
	ppQmHeapNode = &(hMcvp->hMcdi->apQmHeapNode[0]);
	BVDC_P_BUF_MSG(("Mcvp[%d] alloc %d QM filed buffers (%s)",
		hMcvp->eId,
		bMadr? BVDC_P_MAD_QM_BUFFER_COUNT : BVDC_P_MCDI_QM_BUFFER_COUNT,
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eQmBufHeapId)));

	eResult = BVDC_P_BufferHeap_AllocateBuffers_isr(hMcvp->hHeap,
		ppQmHeapNode,
		bMadr? BVDC_P_MAD_QM_BUFFER_COUNT : BVDC_P_MCDI_QM_BUFFER_COUNT,
		false,
		eQmBufHeapId, BVDC_P_BufferHeapId_eUnknown);
	/* Not enough memory, dump out configuration */
	if(eResult == BERR_OUT_OF_DEVICE_MEMORY)
	{
		BDBG_ERR(("Mcvp[%d] Not enough memory for MCVP QM field buffers! Configuration:", hMcvp->eId));
		BDBG_ERR(("Mcvp[%d] BufferCnt: %d", hMcvp->eId, bMadr? BVDC_P_MAD_QM_BUFFER_COUNT : BVDC_P_MCDI_QM_BUFFER_COUNT));
	}
#else
		BSTD_UNUSED(ppQmHeapNode);
		BSTD_UNUSED(bMadr);
#endif

	BDBG_ASSERT(BERR_SUCCESS == eResult);
}

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
	  bool                             bRfcgVnet)
{
	BVDC_P_HeapNodePtr *ppHeapNode;
	bool  bShareCap;

	BDBG_ENTER(BVDC_P_Mcvp_SetVnetAllocBuf_isr);
	BDBG_OBJECT_ASSERT(hMcvp, BVDC_MVP);
	BDBG_OBJECT_ASSERT(hMcvp->hMcdi, BVDC_MDI);

	if(bRfcgVnet)
	{
		/* set up for joining vnet, including acquiring loop-back */
		BVDC_P_SubRul_SetVnet_isr(&(hMcvp->SubRul), ulSrcMuxValue, eVnetPatchMode);
	}

	if(BVDC_P_VNET_BYPASS_MCVP(*pMcvpMode)) {
		BVDC_P_Mcvp_FreeBuf_isr(hMcvp, true);
		return;
	}
	if(((BVDC_P_VNET_USED_MAD(*pMcvpMode)==true) ||
		(BVDC_P_VNET_USED_ANR(*pMcvpMode)==true))&&
		(NULL == hMcvp->hMcdi->apHeapNode[0])) {

		BVDC_P_Mcvp_AllocBuf_isr(hMcvp, ulBufCnt, ePixelBufHeapId, eQmBufHeapId);

		ppHeapNode = &(hMcvp->hMcdi->apHeapNode[0]);
		if(hMcvp->bAnr)
		{
			BDBG_OBJECT_ASSERT(hMcvp->hAnr,  BVDC_ANR);
			/* cp buf node ptr to anr context so it can easily set buf addr in the case
			* that MAD is off */
			bShareCap = BVDC_P_VNET_USED_ANR(*pMcvpMode) && BVDC_P_VNET_USED_MAD(*pMcvpMode);
			BVDC_P_Anr_SetBufNodes_isr(hMcvp->hAnr, ppHeapNode, bShareCap);
		}
	}

	BDBG_LEAVE(BVDC_P_Mcvp_SetVnetAllocBuf_isr);
}

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_UnsetVnetFreeBuf_isr
*
* called by BVDC_P_Window_UnsetWriter(Reader)Vnet_isr to to release the
* potentially used loop-back, and free buffers
*/
void BVDC_P_Mcvp_UnsetVnetFreeBuf_isr
	( BVDC_P_Mcvp_Handle                hMcvp )
{
	BDBG_ENTER(BVDC_P_Mcvp_UnsetVnetFreeBuf_isr);
	BDBG_OBJECT_ASSERT(hMcvp, BVDC_MVP);

	/* release free-channel or loop-back */
	BVDC_P_SubRul_UnsetVnet_isr(&(hMcvp->SubRul));

	/* free internally used buffers if no win is using this MCVP. */
	BVDC_P_Mcvp_FreeBuf_isr(hMcvp, false);

	BDBG_LEAVE(BVDC_P_Mcvp_UnsetVnetFreeBuf_isr);
}

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_BuildRul_SetEnable_isr
*
* called by BVDC_P_Mcvp_BuildRul_isr at every vsync to enable or disable
* hscaler, anr, mcdi and top level.
*/
static void BVDC_P_Mcvp_BuildRul_SetEnable_isr
	( BVDC_P_Mcvp_Handle             hMcvp,
	  BVDC_P_ListInfo               *pList,
	  bool                           bEnable,
	  BVDC_P_PictureNode            *pPicture,
	  BAVC_Polarity                  eSrcNxtFld,
	  bool                           bInit )
{
	uint32_t  ulRegOffset;
	uint32_t  ulModeCtrl;
	bool  bAnr, bMcdi;
	BVDC_FilterMode eMode=BVDC_FilterMode_eDisable;
#if (BVDC_P_SUPPORT_MANR_VER >= BVDC_P_MANR_VER_3)
	uint32_t   ulEoPMask;
#endif

	BDBG_OBJECT_ASSERT(hMcvp, BVDC_MVP);
	ulRegOffset = hMcvp->ulRegOffset;
	bAnr  = BVDC_P_VNET_USED_ANR(pPicture->stVnetMode);
	bMcdi = BVDC_P_VNET_USED_MAD(pPicture->stVnetMode);

	BDBG_OBJECT_ASSERT(hMcvp->hMcdi, BVDC_MDI);

	if((hMcvp->bAnr) && (bAnr))
	{
		BDBG_OBJECT_ASSERT(hMcvp->hAnr,  BVDC_ANR);
		eMode = hMcvp->hAnr->pAnrSetting->eMode;
	}
	else
	{
		eMode = BVDC_FilterMode_eDisable;
	}

	if (bEnable)
	{
		/* 1. configure MCVP_MODE_CTRL */
#if (BVDC_P_SUPPORT_MCVP_VER<=1)
		if(bMcdi)
		{
			ulModeCtrl =
				(false == bAnr)?
				BCHP_FIELD_ENUM(MCVP_TOP_0_MCVP_TOP_CTRL, MCVP_MODE_CTRL, MCDI_ONLY) :
			(BVDC_FilterMode_eBypass == eMode) ?
				BCHP_FIELD_ENUM(MCVP_TOP_0_MCVP_TOP_CTRL, MCVP_MODE_CTRL, MCDI_WITH_NOISE_DETECTION) :
			BCHP_FIELD_ENUM(MCVP_TOP_0_MCVP_TOP_CTRL, MCVP_MODE_CTRL, NORMAL_MCVP_MODE);
		}
		else /* must have (true == bAnr) */
		{
			/*BDBG_ERR(("mcvp mode: ND only or ND+MCTF"));*/
			ulModeCtrl =
				(BVDC_FilterMode_eBypass == eMode) ?
				BCHP_FIELD_ENUM(MCVP_TOP_0_MCVP_TOP_CTRL, MCVP_MODE_CTRL, NOISE_DETECTION_ONLY) :
			BCHP_FIELD_ENUM(MCVP_TOP_0_MCVP_TOP_CTRL, MCVP_MODE_CTRL, MCTF_WITH_NOISE_DETECTION);
		}

		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MCVP_TOP_0_MCVP_TOP_CTRL, ulRegOffset,
			BCHP_FIELD_ENUM(MCVP_TOP_0_MCVP_TOP_CTRL, ENABLE_CTRL, STOP_ON_FIELD_COMPLETION) |
			BCHP_FIELD_DATA(MCVP_TOP_0_MCVP_TOP_CTRL, MCVP_MODE_CTRL, ulModeCtrl));
#else
		if(bMcdi)
		{
			ulModeCtrl =
				(BVDC_FilterMode_eEnable == eMode)?
				BCHP_FIELD_ENUM(MVP_TOP_0_CTRL, MODE_CTRL, NORMAL) :
				BCHP_FIELD_ENUM(MVP_TOP_0_CTRL, MODE_CTRL, MDI_ONLY);
		}
		else
		{
			/*BDBG_ERR(("mcvp mode: ND only or ND+MCTF"));*/
			ulModeCtrl =
				(BVDC_FilterMode_eEnable == eMode)?
				BCHP_FIELD_ENUM(MVP_TOP_0_CTRL, MODE_CTRL, ANR_ONLY):
				BCHP_FIELD_ENUM(MVP_TOP_0_CTRL, MODE_CTRL, BYPASS);
		}

		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MVP_TOP_0_CTRL, ulRegOffset,
			BCHP_FIELD_ENUM(MVP_TOP_0_CTRL, ENABLE_CTRL, STOP_ON_FIELD_COMPLETION) |
			BCHP_FIELD_DATA(MVP_TOP_0_CTRL, MODE_CTRL, ulModeCtrl));
#if (BVDC_P_SUPPORT_MANR_VER >= BVDC_P_MANR_VER_3)
		/*MCVP_TOP. DEBUG_CTRL.EOP_MASK should be programmed to 0
		  unless MCVP is running at ANR only and trick mode. */
		ulEoPMask = ((!bMcdi) && (hMcvp->bAnr) &&
			(pPicture->stFlags.bPictureRepeatFlag || pPicture->stFlags.bRepeatField)) ? 4:0;
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MVP_TOP_0_DEBUG_CTRL, ulRegOffset,
			BCHP_FIELD_DATA(MVP_TOP_0_DEBUG_CTRL, EOP_MASK, ulEoPMask) |
			BCHP_FIELD_ENUM(MVP_TOP_0_DEBUG_CTRL, DIS_AUTO_FLUSH, OFF));
#endif
#endif

		/* 2. config and enable mcdi */
		BVDC_P_Mcdi_BuildRul_SetEnable_isr(hMcvp->hMcdi, pList, bMcdi,
			pPicture->stFlags.bPictureRepeatFlag, eSrcNxtFld, pPicture->eFrameRateCode,
			(BVDC_P_VNET_USED_MCVP_AT_WRITER(pPicture->stVnetMode)
			 ? pPicture->eSrcOrientation : pPicture->eDispOrientation), bInit);

		/* 3. configure and enable ANR
		*    3.1. program AND thresh, AND_AND_MODE;
		*    3.2. enable and;
		*    3.3. configure and enable MCTF */
#if (BVDC_P_SUPPORT_MANR)
		if(hMcvp->bAnr)
		{
			BVDC_P_Anr_BuildRul_SetEnable_isr(hMcvp->hAnr, pPicture, pList, (eMode==BVDC_FilterMode_eEnable), eSrcNxtFld,
				(pPicture->stFlags.bPictureRepeatFlag || pPicture->stFlags.bRepeatField),
				bInit);
		}
#endif
		/* 4. configure and enable HSCL, HSCL cfg and enabling have already set in shadow */
		BVDC_P_Hscaler_BuildRul_SetEnable_isr(hMcvp->hHscaler, pList);

		/* 5 enable mcvp top */
#if (BVDC_P_SUPPORT_MCVP_VER<=1)
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MCVP_TOP_0_ENABLE, ulRegOffset,
			BCHP_FIELD_ENUM(MCVP_TOP_0_ENABLE, ENABLE, ON));
	}
	else
	{
		/* 1 disable mcvp top */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MCVP_TOP_0_ENABLE, ulRegOffset,
			BCHP_FIELD_ENUM(MCVP_TOP_0_ENABLE, ENABLE, OFF));
#else
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MVP_TOP_0_ENABLE, ulRegOffset,
			BCHP_FIELD_ENUM(MVP_TOP_0_ENABLE, ENABLE, ON));
	}
	else
	{
		/* 1 disable mcvp top */
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MVP_TOP_0_ENABLE, ulRegOffset,
			BCHP_FIELD_ENUM(MVP_TOP_0_ENABLE, ENABLE, OFF));
#endif

		/* 2. configure and disable HSCL */
		BVDC_P_Hscaler_SetEnable_isr(hMcvp->hHscaler, false);
		BVDC_P_Hscaler_BuildRul_SetEnable_isr(hMcvp->hHscaler, pList);

		/* 3. disable ANR */
		if(hMcvp->bAnr)
		{
			BVDC_P_Anr_BuildRul_SetEnable_isr(hMcvp->hAnr, pPicture, pList, false, eSrcNxtFld,
				false, bInit);
		}

		/* 4. config and enable mcdi */
		BVDC_P_Mcdi_BuildRul_SetEnable_isr(hMcvp->hMcdi, pList, bEnable,
			pPicture->stFlags.bPictureRepeatFlag, eSrcNxtFld, pPicture->eFrameRateCode,
			(BVDC_P_VNET_USED_MCVP_AT_WRITER(pPicture->stVnetMode)
			 ? pPicture->eSrcOrientation : pPicture->eDispOrientation), bInit);
	}

	return;
}

/***************************************************************************
* {private}
*
* BVDC_P_Mcvp_BuildRul_DrainVnet_isr
*
* called by BVDC_P_Mcvp_BuildRul_isr after resetting to drain the module and
* its pre-patch FreeCh or LpBack.
*/
static void BVDC_P_Mcvp_BuildRul_DrainVnet_isr
	( BVDC_P_Mcvp_Handle             hMcvp,
	  BVDC_P_ListInfo               *pList )
{
	BDBG_OBJECT_ASSERT(hMcvp, BVDC_MVP);

	BVDC_P_SubRul_StartDrain_isr(&(hMcvp->SubRul), pList,
		hMcvp->ulCoreResetAddr, hMcvp->ulCoreResetMask);

	BVDC_P_SubRul_FinishDrain_isr(&(hMcvp->SubRul), pList,
		hMcvp->ulCoreResetAddr, hMcvp->ulCoreResetMask,
		hMcvp->ulVnetResetAddr, hMcvp->ulVnetResetMask);

	return;
}

void BVDC_P_MCVP_SetInfo_isr
	( BVDC_P_Mcvp_Handle                 hMcvp,
	  BVDC_Window_Handle                 hWindow,
	  BVDC_P_PictureNode                *pPicture )
{
	BVDC_P_AnrDirtyBits   *pAnrSwDirty=NULL, *pAnrHwDirty=NULL;
	BVDC_P_McdiDirtyBits  *pMcdiSwDirty=NULL, *pMcdiHwDirty=NULL;
	const BVDC_P_Rect     *pHsclOut;
	uint32_t              ulHSize, ulVSize;
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
	bool                   bDeinterlace;
	BVDC_Video_TestFeature1_Settings   *pUserVideoTestFeature1;
#endif
	bool bShareCap, bMvpBypass = false;
	BVDC_P_VnetMode    stMcvpMode, *pMcvpMode, *pCurMcvpMode;

	BDBG_ENTER(BVDC_P_MCVP_SetInfo_isr);

	pHsclOut        = pPicture->pHsclOut;

	pMcdiSwDirty    = &hMcvp->hMcdi->stSwDirty;
	pMcdiHwDirty    = &hMcvp->hMcdi->stHwDirty;
	BVDC_P_CLEAN_ALL_DIRTY(pMcdiHwDirty);
	BVDC_P_CLEAN_ALL_DIRTY(pMcdiSwDirty);

	if(hMcvp->bAnr)
	{
		pAnrSwDirty     = &hMcvp->hAnr->stSwDirty;
		pAnrHwDirty     = &hMcvp->hAnr->stHwDirty;
		BVDC_P_CLEAN_ALL_DIRTY(pAnrHwDirty);
		BVDC_P_CLEAN_ALL_DIRTY(pAnrSwDirty);
	}

	bMvpBypass = BVDC_P_VNET_BYPASS_MCVP(pPicture->stVnetMode);

	if (BVDC_P_VNET_USED_MCVP(pPicture->stVnetMode))
	{
		pMcvpMode = &stMcvpMode;
		pCurMcvpMode = &hMcvp->stMcvpMode;

		pMcvpMode->stBits.bUseHscl = pPicture->stVnetMode.stBits.bUseHscl;
		pMcvpMode->stBits.bUseAnr = pPicture->stVnetMode.stBits.bUseAnr;
		pMcvpMode->stBits.bUseMad = pPicture->stVnetMode.stBits.bUseMad;
		pMcvpMode->stBits.bUseMvpBypass = pPicture->stVnetMode.stBits.bUseMvpBypass;

		if(*(uint32_t *) pMcvpMode != *(uint32_t *) pCurMcvpMode)
		{
			BDBG_MSG(("Mcvp %d bUse Hscl %d  bUseAnr %d      bUseMad %d", hMcvp->eId,
			pMcvpMode->stBits.bUseHscl,
			pMcvpMode->stBits.bUseAnr,
			pMcvpMode->stBits.bUseMad));
			hMcvp->bReconf = BVDC_P_DIRTY;
			*(uint32_t *) pCurMcvpMode = *(uint32_t *) pMcvpMode;
		}
		ulHSize = pHsclOut->ulWidth;
		ulVSize = (BAVC_Polarity_eFrame == pPicture->PicComRulInfo.eSrcOrigPolarity)?
			pHsclOut->ulHeight: pHsclOut->ulHeight / 2;

		/* size changes causes ANR and MCDI hard-start.  note: size in ANR and
		* MCDI are always the same: the HSCL output size */
		if ((ulHSize != hMcvp->ulPrevHSize) || (ulVSize != hMcvp->ulPrevVSize) ||
		    (pPicture->eSrcOrientation != hMcvp->hMcdi->ePrevSrcOrientation) ||
		    (pPicture->eDispOrientation != hMcvp->hMcdi->ePrevDispOrientation))
		{
			hMcvp->ulPrevHSize = hMcvp->hMcdi->ulHSize = ulHSize;
			hMcvp->ulPrevVSize = hMcvp->hMcdi->ulVSize = ulVSize;
			hMcvp->hMcdi->ePrevSrcOrientation = pPicture->eSrcOrientation;
			hMcvp->hMcdi->ePrevDispOrientation = pPicture->eDispOrientation;

			hMcvp->hMcdi->stSwDirty.stBits.bSize = BVDC_P_DIRTY;

			if(hMcvp->bAnr)
			{
				hMcvp->hAnr->ulHSize = ulHSize;
				hMcvp->hAnr->ulVSize = ulVSize;
				hMcvp->hAnr->stSwDirty.stBits.bSize = BVDC_P_DIRTY;
			}
		}

		if( (hMcvp->bAnr) &&
			((BVDC_P_DIRTY == hMcvp->hAnr->stSwDirty.stBits.bSize ) ||
			(BVDC_P_DIRTY == hMcvp->bReconf)))
		{
			bShareCap = BVDC_P_VNET_USED_ANR(pPicture->stVnetMode) && BVDC_P_VNET_USED_MAD(pPicture->stVnetMode);
			BVDC_P_Anr_SetBufNodes_isr(hMcvp->hAnr, &(hMcvp->hMcdi->apHeapNode[0]), bShareCap);
		}
		if(hMcvp->bAnr)
			*pAnrHwDirty  = *pAnrSwDirty;
		*pMcdiHwDirty = *pMcdiSwDirty;

		if (true == bMvpBypass)  return;
		/*Obtain MCVP interlace setting*/
		BVDC_P_Mcdi_GetUserConf_isr(hMcvp->hMcdi,
		(BVDC_P_Deinterlace_Settings*)&hWindow->stCurInfo.stMadSettings);

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		bDeinterlace = BVDC_P_VNET_USED_MAD(pPicture->stVnetMode);
		if(bDeinterlace)
		{
			hMcvp->hMcdi->eChromaType = pPicture->eChromaType;
			pUserVideoTestFeature1 =
				&hWindow->stCurInfo.stMadSettings.stTestFeature1;
			BVDC_P_Window_SetMadAnrTestFeature1_isr(hWindow,
				hWindow->stCurInfo.stMadSettings.ePixelFmt,
				pPicture->pMadIn, pPicture,
				&hWindow->stCurInfo.stMadSettings.stTestFeature1,
				BVDC_P_VNET_USED_MCVP_AT_WRITER(hWindow->stVnetMode),
				&hMcvp->hMcdi->stTestFeature1);
		}
		else
		{
			BVDC_P_Window_SetMadAnrTestFeature1_isr(hWindow,
				hWindow->stCurInfo.stAnrSettings.ePxlFormat,
				pPicture->pAnrIn, pPicture,
				&hWindow->stCurInfo.stAnrSettings.stVideoTestFeature1,
				BVDC_P_VNET_USED_MCVP_AT_WRITER(hWindow->stVnetMode),
				&hMcvp->hAnr->stTestFeature1);
		}
#endif
	}

	BDBG_LEAVE(BVDC_P_MCVP_SetInfo_isr);
}

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
void BVDC_P_Mcvp_BuildRul_isr
	( BVDC_P_Mcvp_Handle                 hMcvp,
	  BVDC_P_ListInfo                   *pList,
	  BVDC_P_State                       eVnetState,
	  BVDC_P_WindowContext              *pWindow,
	  BVDC_P_PictureNode                *pPicture )
{
	BVDC_P_AnrDirtyBits  *pAnrSwDirty=NULL, *pAnrHwDirty=NULL;
	BVDC_P_McdiDirtyBits  *pMcdiSwDirty=NULL, *pMcdiHwDirty=NULL;
	BVDC_P_PicComRulInfo  *pPicComRulInfo;
	const BVDC_P_Rect  *pHsclOut;

	uint32_t  ulRulOpsFlags;
	bool  bInit, bEnable, bAnrSize = false, bMcdiSize = false, bAnrPxlFmt = false, bBypass=false;

	BDBG_ENTER(BVDC_P_Mcvp_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hMcvp, BVDC_MVP);

	pPicComRulInfo = &pPicture->PicComRulInfo;
	pHsclOut = pPicture->pHsclOut;

	BDBG_OBJECT_ASSERT(hMcvp->hMcdi, BVDC_MDI);
	pMcdiSwDirty = &hMcvp->hMcdi->stSwDirty;
	pMcdiHwDirty = &hMcvp->hMcdi->stHwDirty;
	BVDC_P_CLEAN_ALL_DIRTY(pMcdiHwDirty);

	if(hMcvp->bAnr)
	{
		BDBG_OBJECT_ASSERT(hMcvp->hAnr, BVDC_ANR);
		pAnrSwDirty = &hMcvp->hAnr->stSwDirty;
		pAnrHwDirty = &hMcvp->hAnr->stHwDirty;
		BVDC_P_CLEAN_ALL_DIRTY(pAnrHwDirty);
	}

	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hMcvp->SubRul), pWindow->eId, eVnetState, pList->bLastExecuted);
	bEnable = (ulRulOpsFlags & BVDC_P_RulOp_eEnable);
	bInit   = (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit);

	bMcdiSize = pMcdiSwDirty->stBits.bSize;
	if(hMcvp->bAnr)
	{
		bAnrSize = pAnrSwDirty->stBits.bSize;
		bAnrPxlFmt = pAnrSwDirty->stBits.bPxlFmt;
	}
	if(bEnable){
		bBypass = BVDC_P_VNET_BYPASS_MCVP(pPicture->stVnetMode);
		if(bBypass)
		{
			BVDC_P_Mcvp_FreeBuf_isr(hMcvp, true);
			/* set mode */
#if (BVDC_P_MCVP_VER_2 <= BVDC_P_SUPPORT_MCVP_VER)
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MVP_TOP_0_CTRL, hMcvp->ulRegOffset,
				BCHP_FIELD_ENUM(MVP_TOP_0_CTRL, ENABLE_CTRL, STOP_ON_FIELD_COMPLETION) |
				BCHP_FIELD_ENUM(MVP_TOP_0_CTRL, MODE_CTRL,	 BYPASS));

				/* set Enable */
				BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MVP_TOP_0_ENABLE, hMcvp->ulRegOffset,
							BCHP_FIELD_ENUM(MVP_TOP_0_ENABLE, ENABLE, ON));

#elif (BVDC_P_MCVP_VER_1 <= BVDC_P_SUPPORT_MCVP_VER)
		BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MCVP_TOP_0_MCVP_TOP_CTRL, hMcvp->ulRegOffset,
			BCHP_FIELD_ENUM(MCVP_TOP_0_MCVP_TOP_CTRL, ENABLE_CTRL,    STOP_ON_FIELD_COMPLETION) |
			BCHP_FIELD_ENUM(MCVP_TOP_0_MCVP_TOP_CTRL, MCVP_MODE_CTRL, BYPASS_MCVP));

			/* set Enable */
			BVDC_P_SUBRUL_ONE_REG(pList, BCHP_MCVP_TOP_0_ENABLE , hMcvp->ulRegOffset,
						BCHP_FIELD_ENUM(MCVP_TOP_0_ENABLE, ENABLE, ON));
#endif
		}
		else{
	/* if we re-set vnet, or if src video format changes (HdDvi), we need to
	* do SrcInit, such as reset buffer size and stride */
		if(bInit || bAnrSize || bMcdiSize || bAnrPxlFmt ||
		    pPicComRulInfo->PicDirty.stBits.bInputFormat ||
		    hMcvp->bReconf)
		{
#if (BVDC_P_MADR_VER_5 >= BVDC_P_SUPPORT_MADR_VER)
			/* SW7552-178/182 workaround for MADR seamless transation */
			if((hMcvp->hMcdi->bMadr) &&
				(bMcdiSize || bInit))
				BVDC_P_BUILD_RESET(pList->pulCurrent,
					hMcvp->ulCoreResetAddr, hMcvp->ulCoreResetMask);
#endif

#if (BVDC_P_SUPPORT_MANR)
			if(hMcvp->bAnr && BVDC_P_VNET_USED_ANR(pPicture->stVnetMode))
			{
				BVDC_P_Anr_BuildRul_SrcInit_isr(hMcvp->hAnr, pList, pPicture);
			}
#endif
			if(BVDC_P_VNET_USED_MAD(pPicture->stVnetMode))
			{
				BVDC_P_Mcvp_AllocBuf_isr(hMcvp, pWindow->usMadPixelBufferCnt, pWindow->eMadPixelHeapId, pWindow->eMadQmHeapId);
				BVDC_P_Mcdi_BuildRul_SrcInit_isr(hMcvp->hMcdi, pList);
			}
		}

#if (BVDC_P_SUPPORT_MANR)
		/* periodic statistics read and dynamic setting for ANR */
		/* todo: not readstatis when and is disabled */
		if(hMcvp->bAnr)
		{
			if ((ulRulOpsFlags & BVDC_P_RulOp_eStatisInit) &&
				(BVDC_FilterMode_eBypass != hMcvp->hAnr->pAnrSetting->eMode))
				BVDC_P_Anr_BuildRul_StatisRead_isr(hMcvp->hAnr, pList, bInit);

			/* changes has been built to RUL, but not executed yet */
			*pAnrHwDirty = *pAnrSwDirty;
			BVDC_P_CLEAN_ALL_DIRTY(pAnrSwDirty);
		}
#endif

		BVDC_P_Mcvp_BuildRul_SetEnable_isr(hMcvp, pList, true, pPicture,
			pPicComRulInfo->eSrcOrigPolarity, bInit);
		*pMcdiHwDirty = *pMcdiSwDirty;
		BVDC_P_CLEAN_ALL_DIRTY(pMcdiSwDirty);
			hMcvp->bReconf = BVDC_P_CLEAN;
		}

		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			/*BDBG_ERR(("mcvp join vnet, mux 0x%x, value 0x%x", hMcvp->SubRul.ulMuxAddr,
			BVDC_P_LpBack_MuxAddr_To_PostMuxValue(hMcvp->SubRul.ulPatchMuxAddr)));*/
			BVDC_P_SubRul_JoinInVnet_isr(&(hMcvp->SubRul), pList);
		}

	}
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_SubRul_DropOffVnet_isr(&(hMcvp->SubRul), pList);
		BVDC_P_Mcvp_BuildRul_SetEnable_isr(hMcvp, pList, false, pPicture,
			pPicComRulInfo->eSrcOrigPolarity, false);
	}
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
	{
		BVDC_P_Mcvp_BuildRul_DrainVnet_isr(hMcvp, pList);
	}
	BDBG_LEAVE(BVDC_P_Mcvp_BuildRul_isr);

	return;
}

#else  /* from #if (BVDC_P_SUPPORT_MCVP) */

BDBG_MODULE(BVDC_MCVP);

BERR_Code BVDC_P_Mcvp_Create
	( BVDC_P_Mcvp_Handle *             phMcvp,
	  BVDC_P_McvpId                    eMcvpId,
	  BREG_Handle                      hRegister,
	  BVDC_P_Resource_Handle           hResource )
{
	BSTD_UNUSED(phMcvp);
	BSTD_UNUSED(eMcvpId);
	BSTD_UNUSED(hRegister);
	BSTD_UNUSED(hResource);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Mcvp_Destroy
	( BVDC_P_Mcvp_Handle               hMcvp )
{
	BSTD_UNUSED(hMcvp);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Mcvp_AcquireConnect
	( BVDC_P_Mcvp_Handle                 hMcvp,
	  BVDC_Heap_Handle                   hHeap,
	  BVDC_P_WindowId                    eWinId,
	  BVDC_P_Window_Info                *pNewInfo,
	  const BVDC_Anr_Settings           *pAnrSetting,
	  BVDC_P_Source_InfoPtr              pCurSrcInfo )
{
	BSTD_UNUSED(hMcvp);
	BSTD_UNUSED(hHeap);
	BSTD_UNUSED(eWinId);
	BSTD_UNUSED(pNewInfo);
	BSTD_UNUSED(pAnrSetting);
	BSTD_UNUSED(pCurSrcInfo);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Mcvp_ReleaseConnect_isr
	( BVDC_P_Mcvp_Handle              *phMcvp )
{
	BSTD_UNUSED(phMcvp);
	return BERR_SUCCESS;
}

void BVDC_P_Mcvp_SetVnetAllocBuf_isr
	( BVDC_P_Mcvp_Handle               hMcvp,
	  uint32_t                         ulSrcMuxValue,
	  BVDC_P_VnetPatch                 eVnetPatchMode,
	  BVDC_P_BufferHeapId              ePixelBufHeapId,
	  BVDC_P_BufferHeapId              eQmBufHeapId,
	  uint32_t                         ulBufCnt,
	  BVDC_P_VnetMode                 *pMcvpMode,
	  bool                             bRfcgVnet)
{
	BSTD_UNUSED(hMcvp);
	BSTD_UNUSED(ulSrcMuxValue);
	BSTD_UNUSED(eVnetPatchMode);
	BSTD_UNUSED(ePixelBufHeapId);
	BSTD_UNUSED(eQmBufHeapId);
	BSTD_UNUSED(ulBufCnt);
	BSTD_UNUSED(pMcvpMode);
	BSTD_UNUSED(bRfcgVnet);
}

void BVDC_P_Mcvp_UnsetVnetFreeBuf_isr
	( BVDC_P_Mcvp_Handle                hMcvp )
{
	BSTD_UNUSED(hMcvp);
}

void BVDC_P_Mcvp_BuildRul_isr
	( BVDC_P_Mcvp_Handle                 hMcvp,
	  BVDC_P_ListInfo                   *pList,
	  BVDC_P_State                       eVnetState,
	  BVDC_P_WindowContext              *pWindow,
	  BVDC_P_PictureNode                *pPicture )
{
	BSTD_UNUSED(hMcvp);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eVnetState);
	BSTD_UNUSED(pWindow);
	BSTD_UNUSED(pPicture);
}

#endif /* #if (BVDC_P_SUPPORT_MCVP) */
/* End of file. */
