/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_subrul_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/34 $
 * $brcm_Date: 6/21/12 4:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_subrul_priv.c $
 * 
 * Hydra_Software_Devel/34   6/21/12 4:37p vanessah
 * SW7435-228:SW7435-209: SW workaround for timing violations in the BVB
 * buses between BVN_TOP and BVN_MVP_TOP
 * 
 * Hydra_Software_Devel/33   6/15/12 10:36a vanessah
 * SW7435-237: reset vnet channel connections
 * 
 * Hydra_Software_Devel/32   1/10/12 5:07p yuxiaz
 * SW7405-5577: Added more information in debug message.
 * 
 * Hydra_Software_Devel/31   4/20/11 3:09p tdo
 * SW7425-365: Remove compiler warnings with new kernel build
 *
 * Hydra_Software_Devel/30   6/18/10 4:23p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/29   9/3/09 10:32p pntruong
 * SW3548-2373: Rolled back PR56964.
 *
 * Hydra_Software_Devel/28   8/10/09 3:57p hongtaoz
 * PR57535: cleanup subrul reset condition;
 *
 * Hydra_Software_Devel/27   7/21/09 2:20p syang
 * PR 56964: take off the "connect drainer, en and then dis" step  in
 * vnet-drain code
 *
 * Hydra_Software_Devel/26   7/20/09 4:40p syang
 * PR 56663: include bchp_mmisc.h
 *
 * Hydra_Software_Devel/25   4/27/09 7:29p syang
 * PR 54447: modify coment only
 *
 * Hydra_Software_Devel/24   1/8/09 4:09p syang
 * PR 49579, PR 50867: 1). set MC_NM_ K1 to 0 for bypass; 2). set diff
 * AND_0.EDGE_TH and  AND_0.CONTENT_TH for HD and SD; 3). use diff K
 * table for RF, SD, ED, and HD
 *
 * Hydra_Software_Devel/23   11/25/08 11:13a syang
 * PR 49562: add reset to output channel ping-pong buf of sub-modules and
 * loopbacks and free-channels
 *
 * Hydra_Software_Devel/22   11/7/08 4:53p syang
 * PR 41898:  avoid too much drain ops for reader modules during shut-down
 *
 * Hydra_Software_Devel/21   4/28/08 1:15p pntruong
 * PR42100, PR39923: Avoid starvation of release vnet front free channel
 * and loopback that cause mfd to freeze.
 *
 * Hydra_Software_Devel/20   6/8/07 11:32a syang
 * PR 31849: separate drain from disable for MAD, ANR and CAP
 *
 * Hydra_Software_Devel/19   5/10/07 11:38a syang
 * PR 29846: remove enum eDrainVnet; clean up
 *
 * Hydra_Software_Devel/18   4/4/07 3:46p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 * Hydra_Software_Devel/17   3/5/07 2:44p syang
 * PR 28298: buid enable RUL for eShutDownPending state ONLY if it is
 * already enabled
 *
 * Hydra_Software_Devel/16   2/23/07 6:31p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/15   2/7/07 12:49p syang
 * PR 27054:  handle lbox disabling without vnet reconfig
 *
 * Hydra_Software_Devel/14   2/1/07 12:10p syang
 * PR 27054: update lbox code to use subrul and new standard sub-module
 * interface func set
 *
 * Hydra_Software_Devel/13   1/25/07 1:28p syang
 * PR 27054: change assert of repeated ShutDownRul to msg
 *
 * Hydra_Software_Devel/12   1/24/07 1:44p syang
 * PR 27054: put variable for assert under control of #if DEBUG_BUILD
 *
 * Hydra_Software_Devel/11   1/22/07 3:15p syang
 * PR 27054: 1). only 1 win build rul, 2). reader's modules don't build
 * rul before video sur enabled; 3). writer's modules don't build rul
 * before (GetReconfiguring_isr==true )
 *
 * Hydra_Software_Devel/10   1/22/07 1:18p darnstein
 * PR26619: silence compiler warnings.
 *
 * Hydra_Software_Devel/9   1/17/07 3:00p syang
 * PR 27054: handle win init case as reader builRul before sync writer
 *
 * Hydra_Software_Devel/8   1/16/07 4:07p syang
 * PR 27054: fix 7400 A0 build error with "||".
 *
 * Hydra_Software_Devel/7   1/16/07 3:02p syang
 * PR 27054: fix assert for WinOut (cmp_src)
 *
 * Hydra_Software_Devel/6   1/15/07 4:54p syang
 * PR 27054: fix assert for mfd in ShutDownRul vnet state
 *
 * Hydra_Software_Devel/5   1/12/07 11:50a syang
 * PR 27054: fixed a typo with LpBack_MuxAddr_To_HwId
 *
 * Hydra_Software_Devel/4   1/11/07 7:03p syang
 * PR 22569: 1st time check in after anr building vnet correctly and not
 * hang the sys. But display is still static
 *
 * Hydra_Software_Devel/3   1/10/07 11:02a syang
 * PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
 * dnr, mfd, vfd, cap, ...
 *
 * Hydra_Software_Devel/2   1/5/07 5:46p syang
 * PR 22569:  fix compile error for 7400/7403/7440
 *
 * Hydra_Software_Devel/1   1/5/07 3:51p syang
 * PR 22569:  change file name from bvdc_mdlrul_priv.c
 *
 * Hydra_Software_Devel/2   12/22/06 11:49a syang
 * PR 22569:  1st time have simple_vdc compiled successfully
 *
 * Hydra_Software_Devel/1   12/21/06 3:10p syang
 * PR 22569: init version
 *
 ***************************************************************************/
#include "bvdc_subrul_priv.h"
#include "bchp_vnet_f.h"
#include "bchp_vnet_b.h"
#include "bchp_mmisc.h"

BDBG_MODULE(VNET);

/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_Init
 *
 * called by BVDC_P_*_Create to init BVDC_P_SubRul_Context
 */
void  BVDC_P_SubRul_Init(
	BVDC_P_SubRulContext          *pSubRul,
	uint32_t                       ulMuxAddr,
	uint32_t                       ulPostMuxValue,
	BVDC_P_DrainMode               eDrainMode,
	int32_t                        lStatisReadMark,
	BVDC_P_Resource_Handle         hResource )
{
	/* init the context */
	pSubRul->hResource = hResource;
	pSubRul->lAccumCntr = BVDC_P_SUBRUL_VNET_INIT_MARK;
	pSubRul->eBldWin = BVDC_P_WindowId_eUnknown;
	pSubRul->ulMuxAddr = ulMuxAddr;
	pSubRul->ulPostMuxValue = ulPostMuxValue;
	pSubRul->eDrainMode = eDrainMode;
	pSubRul->lStatisReadMark =
		(lStatisReadMark)? lStatisReadMark : 0x7ffffff0;

	/* all src mux will be set to be disabled by resetting bit VNET_B/F
	 * in BCHP_MMISC_SOFT_RESET in BVDC_P_ResetBvn(pVdc) */
}


/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_ResetAccumCntr_isr
 *
 * Called by a module to reset AccumCntr to 0 for statistics cntr reset.
 */
void BVDC_P_SubRul_ResetAccumCntr_isr(
	BVDC_P_SubRulContext          *pSubRul )
{
	pSubRul->lAccumCntr = BVDC_P_MIN(0, pSubRul->lAccumCntr);
}


/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_SetRulBuildWinId_isr
 *
 * Called by BVDC_P_Window_Writer_isr to set MFD's RUL BuildWinId to be the
 * last win connected to the MFD. Otherwise Mfd might start to feed right
 * after the 1st win gets enabled, then the following win might join in the
 * vnet after feeding started.
 */
void BVDC_P_SubRul_SetRulBuildWinId_isr(
	BVDC_P_SubRulContext          *pSubRul,
	BVDC_P_WindowId                eWin)
{
	pSubRul->eBldWin = eWin;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_GetOps_isr
 *
 * Called by BVDC_P_*_BuildRul_isr to decide what operations to take with
 * this BVDC_P_*_BuildRul_isr call.
 *
 * IMPORTANT: we assume source calls windows to build RUL in the order from
 * smaller WinId to bigger WinId.
 *
 * Input:
 *   eVnetState - the window's reader or writer state
 */
uint32_t BVDC_P_SubRul_GetOps_isr(
	BVDC_P_SubRulContext          *pSubRul,
	BVDC_P_WindowId                eWin,
	BVDC_P_State                   eVnetState,
	bool                           bLastRulExecuted)
{
	uint32_t ulRulOpsFlags = 0;

	BDBG_ASSERT((BVDC_P_State_eCreate   != eVnetState) &&
				(BVDC_P_State_eDestroy  != eVnetState));

	if (BVDC_P_State_eInactive == eVnetState)
	{
		/* for window init stage, this could be WinOut or SCL */
		BDBG_MSG(("RulOp 0: pSubRul->ulMuxAddr %x, ->ulMuxValue %d, ->ulPatchMuxAddr: %x", 
			pSubRul->ulMuxAddr, pSubRul->ulMuxValue, pSubRul->ulPatchMuxAddr));
		return ulRulOpsFlags;
	}

	if ((BVDC_P_State_eActive           == eVnetState) ||
		((BVDC_P_State_eShutDownPending == eVnetState) &&
		 (pSubRul->ulWinsActFlags & (1<<eWin))))  /* such as writer wait for reader down */
	{
		/* bit eWin in ulWinsActFlags indicates whether eWin is actively using this module */
		pSubRul->ulWinsActFlags |= (1<<eWin);

		/* it is the 1st active window that should build the active RUL */
		if ((BVDC_P_WindowId_eUnknown == pSubRul->eBldWin) || /* 1st, or RUL build win is off */
			(eWin == pSubRul->eBldWin))                       /* it built RUL last time */
		{
			pSubRul->eBldWin = eWin;

			if (BVDC_P_SUBRUL_STATE_VNET_INIT_CHECK(pSubRul->lAccumCntr)||
				BVDC_P_SUBRUL_STATE_STATIS_INIT_CHECK(pSubRul->lAccumCntr))
			{
				/* last time we build into RUL a complete hw init or a hw reset, if
				 * the RUL did get executed, we set lAccumCntr to 0 to indicate a
				 * fresh new subsub statistics, otherwise we need to bulid the complete
				 * thing (same as last time) into RUL again */
				if ( bLastRulExecuted )
					pSubRul->lAccumCntr = 0;
				else
					pSubRul->lAccumCntr --; /* return to init or reset state */
			}

			if ( BVDC_P_SUBRUL_STATE_STATIS_INIT(pSubRul->lAccumCntr) )
			{
				if ( BVDC_P_SUBRUL_STATE_VNET_INIT(pSubRul->lAccumCntr) )
				{
					/* inform next GetOps_isr call to check whether init is sent to HW */
					pSubRul->lAccumCntr = BVDC_P_SUBRUL_VNET_INIT_CHECK;

					/* instruct to init for joinning in vnet, src size, ... */
					ulRulOpsFlags |= BVDC_P_RulOp_eVnetInit;
					BDBG_MSG(("RulOp Init: pSubRul->ulMuxAddr %x, ->ulMuxValue %d, ->ulPatchMuxAddr: %x",
							  pSubRul->ulMuxAddr, pSubRul->ulMuxValue, pSubRul->ulPatchMuxAddr));
				}
				else
				{
					/* inform next SubRul_isr call to check whether reset is sent to HW,
					 * however, if statis should be read every vsync, then there is no
					 * need to check that, just inform next vsync to read again */
					pSubRul->lAccumCntr = (1 < pSubRul->lStatisReadMark)?
						BVDC_P_SUBRUL_STATIS_INIT_CHECK : BVDC_P_SUBRUL_STATIS_INIT_MARK;
				}

				/* instruct to reset statistics */
				ulRulOpsFlags |= BVDC_P_RulOp_eStatisInit;
			}
			else
			{
				/* normal case */
				pSubRul->lAccumCntr ++;

				/* check if neext vsync should read and reset statistics */
				pSubRul->lAccumCntr =
					(pSubRul->lAccumCntr >= pSubRul->lStatisReadMark)?
					BVDC_P_SUBRUL_STATIS_INIT_MARK : pSubRul->lAccumCntr;
			}

			/* instruct to enable the hw to accept pixel data */
			ulRulOpsFlags |= BVDC_P_RulOp_eEnable;
		}
	}
	else if (BVDC_P_State_eShutDownRul == eVnetState)
	{
		/* Note: It is the last shuting-down win that should build the shut-down RUL,
		 * even if this is the 2nd time to build shut down RUL in the case that last
		 * one did not execute. However, regardless of whether this module is still
		 * used by other windows, this win will release this module when its related
		 * reader or writer vnet are shut-down and drained.
		 */

#if BDBG_DEBUG_BUILD
		if ((0 == (pSubRul->ulWinsActFlags & (1<<eWin))) && bLastRulExecuted)
		{
			/* eReaderState could be reset to eShutDownPending if ReConfig is requested
			 * again when window just did a shut down for old vnet mode and have not
			 * brought up the new vnet mode yet. This Reconfig request could be due to
			 * a quick following ApplyChange or a src dynamic format change. If we do
			 * darin as join-in vnet, we might be able to get rid of this situation */
			BDBG_MSG(("Repeated ShutDownRul state: pSubRul->ulMuxAddr %x, ->ulMuxValue %d, ->ulPatchMuxAddr: %x",
					  pSubRul->ulMuxAddr, pSubRul->ulMuxValue, pSubRul->ulPatchMuxAddr));
		}
#endif
		/* this hanldle is for modules such as BoxDetect that could be disabled without
		 * vnet reconfigure */
		if ((0 == (pSubRul->ulWinsActFlags & (1 << eWin))) && (bLastRulExecuted))
		{
			/* disable rul executed for this win, instruct to release handle inside
			 * BVDC_P_*_BuildRul_isr */
			ulRulOpsFlags |= BVDC_P_RulOp_eReleaseHandle;
			BDBG_MSG(("RulOp ReleaseHandle: pSubRul->ulMuxAddr %x, ->ulMuxValue %d, ->ulPatchMuxAddr: %x",
					  pSubRul->ulMuxAddr, pSubRul->ulMuxValue, pSubRul->ulPatchMuxAddr));
		}
		else
		{
			/* vnet for this win is in shuting down process */
			pSubRul->ulWinsActFlags &= ~(1<<eWin);

			if ((0 == pSubRul->ulWinsActFlags) &&
				((BVDC_P_WindowId_eUnknown == pSubRul->eBldWin) || (eWin == pSubRul->eBldWin)))
			{
				/* this win is the last win that shuts down, build shut down and
				 * drain RUL.
				 * Note: if a module could be shared by multiple wins, SetRulBuildWinId
				 * in the setInfo round is needed. If a window with bigger winId is
				 * enabling it at this vsync, we don't want to disable the module
				 * here. */

				/* instruct to shut down this module and drain vnet */
				ulRulOpsFlags |= BVDC_P_RulOp_eDisable;

				/* if it is enabled again, need to re-init src */
				pSubRul->lAccumCntr = BVDC_P_SUBRUL_VNET_INIT_MARK;
				pSubRul->eBldWin = BVDC_P_WindowId_eUnknown;
				BDBG_MSG(("RulOp Disable: pSubRul->ulMuxAddr %x, ->ulMuxValue %d, ->ulPatchMuxAddr: %x",
						  pSubRul->ulMuxAddr, pSubRul->ulMuxValue, pSubRul->ulPatchMuxAddr));
			}
			else if ((0 != pSubRul->ulWinsActFlags) &&
					 ((BVDC_P_WindowId_eUnknown == pSubRul->eBldWin) || (eWin == pSubRul->eBldWin)))
			{
				if (0 == (pSubRul->ulWinsActFlags & ~((1 << eWin) - 1)))
				{
					/* there is some window with smaller winId, but no more win with bigger winId,
					 * that are still actively using this module, this win should build an active
					 * RUL for the last time.
					 *
					 * IMPORTANT: we assume source calls windows to build RUL in an oder from
					 * small WinId to bigger WinId
					 */

					/* instruct to enable the hw to accept pixel data */
					ulRulOpsFlags |= BVDC_P_RulOp_eEnable;
					pSubRul->lAccumCntr ++;
				}

				/* this win no-longer uses the model, but there is other win that still uses it,
				 * therefore this win should no longer drive the RUL build */
				pSubRul->eBldWin = BVDC_P_WindowId_eUnknown;
			}
		}

		/* else: ((0 == pSubRul->ulWinsActFlags) &&
		 *        ((BVDC_P_WindowId_eUnknown != pSubRul->eBldWin) && (eWin != pSubRul->eBldWin))) ||
		 *       ((0 != pSubRul->ulWinsActFlags) &&
		 *        ((BVDC_P_WindowId_eUnknown != pSubRul->eBldWin) && (eWin != pSubRul->eBldWin)))
		 * don't need to do anything here
		 */
	}

	else if ((0 == pSubRul->ulWinsActFlags) &&
			 (BVDC_P_State_eDrainVnet == eVnetState))
	{
		ulRulOpsFlags |= BVDC_P_RulOp_eDrainVnet;
		BDBG_MSG(("RulOp DrainVnet: pSubRul->ulMuxAddr %x, ->ulMuxValue %d, ->ulPatchMuxAddr: %x",
				  pSubRul->ulMuxAddr, pSubRul->ulMuxValue, pSubRul->ulPatchMuxAddr));
	}

	/* else
		((BVDC_P_State_eShutDownPending == eVnetState) && !(pSubRul->ulWinsActFlags & (1<<eWin))) ||
		(((BVDC_P_State_eDrainVnet == eVnetState) || (BVDC_P_State_eShutDown == eVnetState)) &&
		 (0 != pSubRul->ulWinsActFlags))
	 * don't need to do anything here
	 */

	return ulRulOpsFlags;
}


/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_SetVnet_isr
 *
 * Called by BVDC_P_*_BuildRul_isr to setup for joinning into vnet
 *
 * input:
 *   ulSrcMuxValue - this module's new src mux value
 *   eVnetPatchMode - specify need pre-freeCh or pre-Lpback
 */
BERR_Code  BVDC_P_SubRul_SetVnet_isr(
	BVDC_P_SubRulContext          *pSubRul,
	uint32_t                       ulSrcMuxValue,
	BVDC_P_VnetPatch               eVnetPatchMode)
{
	uint32_t  ulFreeChHwId, ulLpBackHwId;
	BERR_Code  eResult;

	/* if the module is already active, we can not change its up-stream
	 * without a shuting down process */
    BDBG_ASSERT((BVDC_P_SUBRUL_STATE_VNET_INIT(pSubRul->lAccumCntr)) ||
                ((eVnetPatchMode == pSubRul->eVnetPatchMode) &&
                 (ulSrcMuxValue == pSubRul->ulMuxValue)));

	/* don't need to do anything if this module already sets-up to join-in vnet
	 * by another wiondow */
	if(BVDC_P_SUBRUL_STATE_VNET_INIT(pSubRul->lAccumCntr) &&
	   (0 == pSubRul->ulPatchMuxAddr))
	{
		BDBG_ASSERT(BVDC_P_VnetPatch_eNone == pSubRul->eVnetPatchMode);
		if (BVDC_P_VnetPatch_eFreeCh == eVnetPatchMode)
		{
			/* acquire a free channel */
			eResult = BVDC_P_Resource_AcquireHwId_isr(pSubRul->hResource,
				BVDC_P_ResourceType_eFreeCh, 0, ulSrcMuxValue, &ulFreeChHwId, false);
			BDBG_MSG(("AcquireFreeCh: eType=%d, ulHwId=%d, Cntr=%d",
				BVDC_P_ResourceType_eFreeCh, ulFreeChHwId,
				BVDC_P_Resource_GetHwIdAcquireCntr_isr(pSubRul->hResource, BVDC_P_ResourceType_eFreeCh, ulFreeChHwId)));
			BDBG_ASSERT(BERR_SUCCESS == eResult);
			if (BERR_SUCCESS != eResult)
				return BERR_TRACE(eResult);

			pSubRul->ulPatchMuxAddr = BVDC_P_FreeCh_eId_To_MuxAddr(ulFreeChHwId);
		}
#ifdef BCHP_VNET_B_LOOPBACK_0_SRC
		else if (BVDC_P_VnetPatch_eLpBack == eVnetPatchMode)
		{
			/* acquire a loop back */
			eResult = BVDC_P_Resource_AcquireHwId_isr(pSubRul->hResource,
				BVDC_P_ResourceType_eLpBck, 0, ulSrcMuxValue, &ulLpBackHwId, false);
			BDBG_MSG(("AcquireLpBck: eType=%d, ulHwId=%d, Cntr=%d",
				BVDC_P_ResourceType_eLpBck, ulLpBackHwId,
				BVDC_P_Resource_GetHwIdAcquireCntr_isr(pSubRul->hResource, BVDC_P_ResourceType_eLpBck, ulLpBackHwId)));
			BDBG_ASSERT(BERR_SUCCESS == eResult);
			if (BERR_SUCCESS != eResult)
				return BERR_TRACE(eResult);

			pSubRul->ulPatchMuxAddr = BVDC_P_LpBack_eId_To_MuxAddr(ulLpBackHwId);
		}
#else
		BSTD_UNUSED(ulLpBackHwId);
#endif
		pSubRul->eVnetPatchMode = eVnetPatchMode;
		pSubRul->ulMuxValue = ulSrcMuxValue;
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_UnsetVnet_isr
 *
 * Called by BVDC_P_*_UnsetVnet_isr to release the free-channel or
 * loop-back used to to patch the vnet.  It should be called only after
 * shut-down RUL is executed.
 */
void BVDC_P_SubRul_UnsetVnet_isr(
	BVDC_P_SubRulContext          *pSubRul)
{
	/* free the free-channel or loop-back that is used to patch the vnet */
	if ((0 == pSubRul->ulWinsActFlags) &&   /* no win is using this module */
		(0 != pSubRul->ulPatchMuxAddr))     /* not freed yet */
	{
		BDBG_ASSERT(BVDC_P_SUBRUL_STATE_VNET_INIT(pSubRul->lAccumCntr));
		BDBG_ASSERT(BVDC_P_WindowId_eUnknown == pSubRul->eBldWin);
		BDBG_ASSERT(BVDC_P_VnetPatch_eNone != pSubRul->eVnetPatchMode);

		if (BVDC_P_VnetPatch_eFreeCh == pSubRul->eVnetPatchMode)
		{
			BVDC_P_Resource_ReleaseHwId_isr(pSubRul->hResource, BVDC_P_ResourceType_eFreeCh,
				BVDC_P_FreeCh_MuxAddr_To_HwId(pSubRul->ulPatchMuxAddr));
			BDBG_MSG(("ReleaseHwId: eType=%d, ulHwId=%d, Cntr=%d",
				BVDC_P_ResourceType_eFreeCh,
				BVDC_P_FreeCh_MuxAddr_To_HwId(pSubRul->ulPatchMuxAddr),
				BVDC_P_Resource_GetHwIdAcquireCntr_isr(pSubRul->hResource,
				BVDC_P_ResourceType_eFreeCh,
				BVDC_P_FreeCh_MuxAddr_To_HwId(pSubRul->ulPatchMuxAddr))));
		}
#ifdef BCHP_VNET_B_LOOPBACK_0_SRC
		else if (BVDC_P_VnetPatch_eLpBack == pSubRul->eVnetPatchMode)
		{
			BVDC_P_Resource_ReleaseHwId_isr(pSubRul->hResource, BVDC_P_ResourceType_eLpBck,
				BVDC_P_LpBack_MuxAddr_To_HwId(pSubRul->ulPatchMuxAddr));
			BDBG_MSG(("ReleaseHwId: eType=%d, ulHwId=%d, Cntr=%d",
				BVDC_P_ResourceType_eLpBck,
				BVDC_P_LpBack_MuxAddr_To_HwId(pSubRul->ulPatchMuxAddr),
				BVDC_P_Resource_GetHwIdAcquireCntr_isr(pSubRul->hResource,
				BVDC_P_ResourceType_eLpBck,
				BVDC_P_LpBack_MuxAddr_To_HwId(pSubRul->ulPatchMuxAddr))));
		}
#endif
		pSubRul->ulPatchMuxAddr = 0;
		pSubRul->eVnetPatchMode = BVDC_P_VnetPatch_eNone;
		pSubRul->ulMuxValue = BVDC_P_MuxValue_SrcOutputDisabled;
	}

	return;
}


/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_JoinInVnet
 *
 * Called by BVDC_P_*_BuildRul_isr to build RUL for joinning into vnet
 */
void BVDC_P_SubRul_JoinInVnet_isr(
	BVDC_P_SubRulContext          *pSubRul,
	BVDC_P_ListInfo               *pList )
{
	/* join in vnet from back to front. Join in vnet after enable module */
	BDBG_ASSERT(pSubRul->ulMuxAddr);
	if (BVDC_P_VnetPatch_eFreeCh == pSubRul->eVnetPatchMode)
	{
		BDBG_ASSERT(pSubRul->ulPatchMuxAddr);
		BVDC_P_SUBRUL_ONE_REG(pList, pSubRul->ulMuxAddr, 0,
			BVDC_P_FreeCh_MuxAddr_To_PostMuxValue(pSubRul->ulPatchMuxAddr));
		BVDC_P_SUBRUL_ONE_REG(pList, pSubRul->ulPatchMuxAddr, 0, pSubRul->ulMuxValue);
	}
#ifdef BCHP_VNET_B_LOOPBACK_0_SRC
	else if (BVDC_P_VnetPatch_eLpBack == pSubRul->eVnetPatchMode)
	{
		BDBG_ASSERT(pSubRul->ulPatchMuxAddr);
		BVDC_P_SUBRUL_ONE_REG(pList, pSubRul->ulMuxAddr, 0,
			BVDC_P_LpBack_MuxAddr_To_PostMuxValue(pSubRul->ulPatchMuxAddr));
		BVDC_P_SUBRUL_ONE_REG(pList, pSubRul->ulPatchMuxAddr, 0, pSubRul->ulMuxValue);
	}
#endif
	else
	{
		BVDC_P_SUBRUL_ONE_REG(pList, pSubRul->ulMuxAddr, 0, pSubRul->ulMuxValue);
	}

	return;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_DropOffVnet
 *
 * Called by BVDC_P_*_BuildRul_isr to build RUL for droping off from vnet
 */
void BVDC_P_SubRul_DropOffVnet_isr(
	BVDC_P_SubRulContext          *pSubRul,
	BVDC_P_ListInfo               *pList )
{
	uint32_t                       ulHwId;
	BVDC_P_ResourceType            eType;

	/* drop off vnet fron front to back. Drop off before disable module */
	BDBG_ASSERT(pSubRul->ulMuxAddr);
	if ((BVDC_P_VnetPatch_eFreeCh == pSubRul->eVnetPatchMode) ||
		(BVDC_P_VnetPatch_eLpBack == pSubRul->eVnetPatchMode))
	{
		BDBG_ASSERT(pSubRul->ulPatchMuxAddr);
#ifdef BCHP_VNET_B_LOOPBACK_0_SRC
		ulHwId = (BVDC_P_VnetPatch_eLpBack == pSubRul->eVnetPatchMode) ?
			BVDC_P_LpBack_MuxAddr_To_HwId(pSubRul->ulPatchMuxAddr) :
			BVDC_P_FreeCh_MuxAddr_To_HwId(pSubRul->ulPatchMuxAddr);
		eType = (BVDC_P_VnetPatch_eLpBack == pSubRul->eVnetPatchMode) ?
			BVDC_P_ResourceType_eLpBck : BVDC_P_ResourceType_eFreeCh;
#else
		ulHwId = BVDC_P_FreeCh_MuxAddr_To_HwId(pSubRul->ulPatchMuxAddr);
		eType = BVDC_P_ResourceType_eFreeCh;
#endif
		BDBG_MSG(("LpBack/FreeCh DropOffVnet: eType=%d, ulHwId=%d, Cntr=%d", eType, ulHwId,
			BVDC_P_Resource_GetHwIdAcquireCntr_isr(pSubRul->hResource, eType, ulHwId)));
		/* Only disable LPB or FCH if it is not shared by any other module */
		/* Since acquired counter is decremented to 0 when HW is released  */
		/* which is happenning later than this, at this point, if acquired */
		/* counter is 1, that indicates that it can be disabled */
		if(BVDC_P_Resource_GetHwIdAcquireCntr_isr(pSubRul->hResource, eType, ulHwId) <= 1)
		{
			BVDC_P_SUBRUL_ONE_REG(pList, pSubRul->ulPatchMuxAddr, 0, BVDC_P_MuxValue_SrcOutputDisabled);
		}
		else
		{
			BVDC_P_SubRul_UnsetVnet_isr(pSubRul);
		}
	}

	BVDC_P_SUBRUL_ONE_REG(pList, pSubRul->ulMuxAddr, 0, BVDC_P_MuxValue_SrcOutputDisabled);

	return;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_StartDrain
 *
 * Called by BVDC_P_*_BuildRul_DrainVnet_isr to reset sub and connect the
 * module to a drain.
 */
void BVDC_P_SubRul_StartDrain_isr(
	BVDC_P_SubRulContext          *pSubRul,
	BVDC_P_ListInfo               *pList,
	uint32_t                       ulResetReg,
	uint32_t                       ulResetMask )
{
	uint32_t  ulDrainMuxAddr;

	/* Saved Drains' current mux value */
	*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VNET_F_DRAIN_0_SRC);
	*pList->pulCurrent++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_1);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VNET_B_DRAIN_0_SRC);

	if (0 != ulResetMask)
	{
		/* reset before drain. capture only need reset */
		BDBG_ASSERT(0 != ulResetReg);
		BVDC_P_SUBRUL_ONE_REG(pList, ulResetReg, 0, ulResetMask);
#if(BVDC_P_SUPPORT_MCDI_VER == 5)
		if(ulResetMask!=BCHP_MMISC_SW_INIT_MVP_0_MASK)
#endif
		BVDC_P_SUBRUL_ONE_REG(pList, ulResetReg, 0, 0);
	}

	if (BVDC_P_DrainMode_eNone != pSubRul->eDrainMode)
	{
		/* connect the module to the drain. will not call this func if drain
		 * is not needed */
		ulDrainMuxAddr = (BVDC_P_DrainMode_eFront == pSubRul->eDrainMode)?
			BCHP_VNET_F_DRAIN_0_SRC : BCHP_VNET_B_DRAIN_0_SRC;
		BVDC_P_SUBRUL_ONE_REG(pList, ulDrainMuxAddr, 0, pSubRul->ulPostMuxValue);

		/* a back drain module (middle module) should then enable and disable
		 * itself, but a front drain module does not need to do this. Finally,
		 * BVDC_P_SubRul_FinishDrain_isr should be called. */
	}

	return;
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_SubRul_FinishDrain
 *
 * Called by BVDC_P_*_BuildRul_DrainVnet_isr to disconnect the module from
 * the drain; reset it again; reset its channel ping-pong buf; and finally
 * drain the free-channel or loop-back used to to patch the vnet, and reset
 * its channel ping-pong buf too.
 */
void BVDC_P_SubRul_FinishDrain_isr(
	BVDC_P_SubRulContext          *pSubRul,
	BVDC_P_ListInfo               *pList,
	uint32_t                       ulResetReg,
	uint32_t                       ulResetMask,
	uint32_t                       ulChnResetReg,
	uint32_t                       ulChnResetMask )
{
	uint32_t  ulDrainMuxAddr;
	uint32_t  ulVnetBChannelReset=0, ulVnetFChannelReset=0;
	uint32_t  ulVnetBChanFch0ResetMask =0, ulVnetFChanLp0ResetMask=0;

	if (0 != ulResetReg)
	{
		/* the module should have disabled itself, now disconnect the module
		 * from the drain */
		ulDrainMuxAddr = (BVDC_P_DrainMode_eFront == pSubRul->eDrainMode)?
			BCHP_VNET_F_DRAIN_0_SRC : BCHP_VNET_B_DRAIN_0_SRC;
		BVDC_P_SUBRUL_ONE_REG(pList, ulDrainMuxAddr, 0, BVDC_P_MuxValue_SrcOutputDisabled);

		/* reset the module after drain */
		BVDC_P_SUBRUL_ONE_REG(pList, ulResetReg, 0, ulResetMask);
		BVDC_P_SUBRUL_ONE_REG(pList, ulResetReg, 0, 0);
	}

	/* reset the channel ping-pong after drain the module */
	if (0 != ulChnResetReg)
	{
		BVDC_P_SUBRUL_ONE_REG(pList, ulChnResetReg, 0, ulChnResetMask);
		BVDC_P_SUBRUL_ONE_REG(pList, ulChnResetReg, 0, 0);
	}

#if BCHP_MMISC_VNET_B_CHANNEL_RESET
	ulVnetBChannelReset = BCHP_MMISC_VNET_B_CHANNEL_RESET;
	ulVnetFChannelReset = BCHP_MMISC_VNET_F_CHANNEL_RESET;
#if BCHP_MMISC_VNET_B_CHANNEL_RESET
	ulVnetBChanFch0ResetMask = BCHP_MMISC_VNET_B_CHANNEL_RESET_FCH_0_RESET_MASK;
	ulVnetFChanLp0ResetMask  = BCHP_MMISC_VNET_F_CHANNEL_RESET_LOOP_0_RESET_MASK;
#endif
#elif BCHP_MMISC_VNET_B_CHANNEL_SW_INIT
	ulVnetBChannelReset = BCHP_MMISC_VNET_B_CHANNEL_SW_INIT;
	ulVnetFChannelReset = BCHP_MMISC_VNET_F_CHANNEL_SW_INIT;
	ulVnetBChanFch0ResetMask = BCHP_MMISC_VNET_B_CHANNEL_SW_INIT_FCH_0_MASK;
	ulVnetFChanLp0ResetMask  = BCHP_MMISC_VNET_F_CHANNEL_SW_INIT_LOOP_0_MASK;
#endif
	/* drain the free-channel or loop-back that is used to patch the vnet */
	if (BVDC_P_VnetPatch_eFreeCh == pSubRul->eVnetPatchMode)
	{
		ulDrainMuxAddr = BCHP_VNET_B_DRAIN_0_SRC;
		BVDC_P_SUBRUL_ONE_REG(pList, ulDrainMuxAddr, 0,
			BVDC_P_FreeCh_MuxAddr_To_PostMuxValue(pSubRul->ulPatchMuxAddr));
		BVDC_P_SUBRUL_ONE_REG(pList, ulDrainMuxAddr, 0, BVDC_P_MuxValue_SrcOutputDisabled);
		/* reset channel ping-pong after drain the free channel */
		if(ulVnetBChannelReset)
		{
			BVDC_P_SUBRUL_ONE_REG(pList, ulVnetBChannelReset, 0,
				BVDC_P_FreeCh_MuxAddr_To_ChnResetMask(pSubRul->ulPatchMuxAddr, ulVnetBChanFch0ResetMask));
			BVDC_P_SUBRUL_ONE_REG(pList, ulVnetBChannelReset, 0, 0);
		}
	}
#ifdef BCHP_VNET_B_LOOPBACK_0_SRC
	else if (BVDC_P_VnetPatch_eLpBack == pSubRul->eVnetPatchMode)
	{
		ulDrainMuxAddr = BCHP_VNET_F_DRAIN_0_SRC;
		BVDC_P_SUBRUL_ONE_REG(pList, ulDrainMuxAddr, 0,
			BVDC_P_LpBack_MuxAddr_To_PostMuxValue(pSubRul->ulPatchMuxAddr));
		BVDC_P_SUBRUL_ONE_REG(pList, ulDrainMuxAddr, 0, BVDC_P_MuxValue_SrcOutputDisabled);

		/* reset channel ping-pong after drain the loop back */
		if(ulVnetFChannelReset)
		{
			BVDC_P_SUBRUL_ONE_REG(pList, ulVnetFChannelReset, 0,
				BVDC_P_LpBack_MuxAddr_To_ChnResetMask(pSubRul->ulPatchMuxAddr, ulVnetFChanLp0ResetMask));
			BVDC_P_SUBRUL_ONE_REG(pList, ulVnetFChannelReset, 0, 0);
		}
	}
#endif

	/* Restore drains' current mux value. */
	*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_0);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VNET_F_DRAIN_0_SRC);
	*pList->pulCurrent++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_1);
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VNET_B_DRAIN_0_SRC);

	return;
}

/* End of file. */
