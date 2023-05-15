/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdma_pci.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 3/21/11 6:38p $
 *
 * Module Description:
 *
 *
 * Revision History:
 * $brcm_Log: /magnum/portinginterface/dma/7400/bdma_pci.c $
 * 
 * Hydra_Software_Devel/12   3/21/11 6:38p vanessah
 * SW7420-1431: support oflarge DMA descriptor list subset
 * 
 * Hydra_Software_Devel/11   2/2/11 10:28a vanessah
 * SW7550-670: add Sharf DMA support
 * 
 * Hydra_Software_Devel/10   1/7/11 2:58p vanessah
 * SWBLURAY-23683:
 * DMA PI link error fix
 * 
 * Hydra_Software_Devel/9   11/17/10 4:12p vanessah
 * SW7231-18:  Add DMA support for 7231 7344 7346
 * 
 * Hydra_Software_Devel/8   11/4/10 1:42p vanessah
 * SW7552-14:  Add DMA PI support
 * 
 * Hydra_Software_Devel/7   11/1/10 1:57p yuxiaz
 * SW7358-15: Added DMA PI support on 7358.
 * 
 * Hydra_Software_Devel/6   8/31/10 2:36p tdo
 * SW7422-62: Add DMA PI
 *
 * Hydra_Software_Devel/5   2/2/10 3:07p syang
 * SW7408-68: add 7408 support
 *
 * Hydra_Software_Devel/4   12/4/09 10:54a rpan
 * SW7468-19: Fixed a 7468 compile error.
 *
 * Hydra_Software_Devel/3   9/16/09 6:05p tdo
 * SW7630-13: Fix 7630 compiling error
 *
 * Hydra_Software_Devel/2   7/30/08 11:53a syang
 * PR 45128: fix ptr over-run caused by using BDMA_MemDmaEngine_eUnknown
 * for engine handle array boundary
 *
 * Hydra_Software_Devel/15   7/11/08 11:50a syang
 * PR 43729:  make bdma.h chip independent
 *
 * Hydra_Software_Devel/14   12/7/07 4:09p syang
 * PR 34606: rewrite more than half of code to support hw sharing for
 * kernel and user-mode sides
 *
 * Hydra_Software_Devel/13   10/15/07 3:38p syang
 * PR 35947: add "_isr" to call back func name
 *
 * Hydra_Software_Devel/12   3/16/07 12:31p syang
 * PR 28173: add dynamic link support (not turn on yet)
 *
 * Hydra_Software_Devel/11   10/3/06 4:01p syang
 * PR 24634: replace critical section with private mutex for
 * mem/pci_dma_handle create/destroy.
 *
 * Hydra_Software_Devel/10   5/13/05 4:26p syang
 * PR 15269: added implementation of BDMA_Mem_Tran_GetMaxNumBlocks and
 * BDMA_Pci_Tran_GetMaxNumBlocks
 *
 * Hydra_Software_Devel/9   3/8/04 3:15p syang
 * PR 10012: fix a typo in SetMaxBurstSize
 *
 * Hydra_Software_Devel/8   2/4/04 3:01p syang
 * PR 9608: 1). added init for hMemDma / hPciDma of Channel, hDma of
 * MemDma / PciDma and hChannel of Queue; 2). split CallBackFunc type def
 * to mem and pci versions; 3). fixed typo in TranHandle def; 4).leave
 * critical section before calling BINT func; 5). fixed a bug with
 * nextDesc in Desc info setting; 6). use wake to start dma as in sleep
 * mode; 7). corrected typo in the asserts of Queue_CreateTran.
 *
 * Hydra_Software_Devel/7   10/24/03 2:53p syang
 * changed to use PciBusAddr and MemBusAddr from SrcBusAddr and
 * DstBusAddr. I think the new addr model is more natural to the way how
 * user gets the addr
 *
 * Hydra_Software_Devel/6   10/23/03 6:18p syang
 * to make linux comipler silent
 *
 * Hydra_Software_Devel/5   10/23/03 4:20p syang
 * adjusted sub-module implementaion overview
 *
 * Hydra_Software_Devel/4   10/23/03 3:16p syang
 * added real pci dma implementation
 *
 * Hydra_Software_Devel/3   10/10/03 4:06p syang
 * added peStatus to StartAndWait
 *
 * Hydra_Software_Devel/2   10/3/03 6:14p syang
 * added real code
 *
 * Hydra_Software_Devel/1   9/25/03 7:01p syang
 * init version, from scratch
 *
 ***************************************************************************/
#if (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556) && (BCHP_CHIP != 7630) && (BCHP_CHIP != 7640) && (BCHP_CHIP != 7468) \
    && (BCHP_CHIP != 7408) && (BCHP_CHIP != 7422) && (BCHP_CHIP != 7425) \
    && (BCHP_CHIP != 7358) && (BCHP_CHIP != 7552) \
    && (BCHP_CHIP != 7231) && (BCHP_CHIP != 7344) && (BCHP_CHIP != 7346) \
    && (BCHP_CHIP != 7550)
#include "bdma_pci_priv.h"
#include "bkni.h"

BDBG_MODULE(BDMA_PCI);

/***************************************************************************
 * Implementation Overview:
 *
 * The corresponding API functions of both interrupt handler (or critical
 * session) and non-interrupt handler context version call the same
 * corresponding private function to do the real job. However, a non-
 * interrupt handler context version API function enters critical session
 * before calling its private function, and leave critical session afterwards.
 * But interrupt handler is already not interruptable, a interrupt handler
 * context version of API functions call their private functions without
 * entering critical session
 ****************************************************************************/

/* Default settings. */
static const BDMA_Pci_Settings s_stDefaultPciDmaSettings =
{
	BDMA_P_NUM_QUEUE_ENTRIES,
	BDMA_P_MAX_NUM_BLOCKS,

	/* deafult for the case that only one BDMA instance exists in the system */
	NULL, NULL, NULL, 0, true
};

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Create2(
	BDMA_Handle           hDma,
	BDMA_PciDmaEngine     eEngine,
	BDMA_Pci_Settings *   pSettings,
	BDMA_Pci_Handle *     phPciDma )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BDMA_P_Context  *pDma;

	BDMA_P_MAIN_GET_CONTEXT( hDma, pDma );
	if (NULL == pDma)
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BDMA_Err_Pci_Create;
	}

	/* hDma passed the magic check, we trust every thing pointed by it now */
	eResult = BKNI_AcquireMutex(pDma->hMutex);
	if (BERR_SUCCESS == eResult)
	{
		if (NULL != pSettings)
			eResult = BDMA_P_Pci_Create( hDma, eEngine, pSettings, phPciDma );
		else
			eResult = BDMA_P_Pci_Create( hDma, eEngine, &s_stDefaultPciDmaSettings, phPciDma );
		BKNI_ReleaseMutex(pDma->hMutex);
	}

  BDMA_Err_Pci_Create:
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Destroy(
	BDMA_Pci_Handle       hPciDma )
{
	BKNI_MutexHandle  hMutex;
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Pci_AcquireMutex(hPciDma, &hMutex );
	if (BERR_SUCCESS == eResult)
	{
		eResult = BDMA_P_Pci_Destroy( hPciDma );
		BKNI_ReleaseMutex(hMutex);
	}

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_GetDefaultSettings
	( BDMA_Pci_Settings     *pDefSettings )
{
	if (NULL != pDefSettings)
	{
		*pDefSettings = s_stDefaultPciDmaSettings;
		return BERR_SUCCESS;
	}
	else
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_SetSwapMode(
	BDMA_Pci_Handle       hPciDma,
	BDMA_SwapMode         eSwapMode )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_SetSwapMode_isr( hPciDma, eSwapMode );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_SetMaxBurstSize(
	BDMA_Pci_Handle       hPciDma,
	BDMA_MaxBurstSize     eMaxBurstSize )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_SetMaxBurstSize_isr( hPciDma, eMaxBurstSize );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Create2(
	BDMA_Pci_Handle          hPciDma,
	uint32_t                 ulNumBlocks,
	bool                     bCachedDesc,
	BDMA_Pci_Tran_Handle  *  phTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_Create_isr( hPciDma, ulNumBlocks, bCachedDesc, phTran );

	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_GetMaxNumBlocks(
	BDMA_Pci_Handle          hPciDma,
	uint32_t *               pulMaxNumBlocks )
{
	BERR_Code  eResult = BERR_SUCCESS;

	if ( (NULL == hPciDma) || (NULL == pulMaxNumBlocks) )
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BDMA_Done_Pci_Tran_GetMaxNumBlocks;
	}
	*pulMaxNumBlocks = BDMA_P_MAX_NUM_BLOCKS;

  BDMA_Done_Pci_Tran_GetMaxNumBlocks:

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_SetBlockInfo(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulMemBusAddr,
	uint32_t                 ulPciBusAddr,
	uint32_t                 ulBlockSize,
	BDMA_TranDir             eTranDirection )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_SetBlockInfo_isr( hTran, ulBlockId,
											ulMemBusAddr, ulPciBusAddr,
											ulBlockSize,
											eTranDirection );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartDma(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_Start_isr( hTran, ulNumActBlocks, NULL, NULL );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */

BERR_Code BDMA_Pci_Tran_StartDmaSubset(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulFirstBlock,
	uint32_t                 ulNumActBlocks )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_StartSubset_isr( hTran, ulFirstBlock, ulNumActBlocks, NULL, NULL );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartDmaAndCallBack(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks,
	BDMA_Pci_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_Start_isr( hTran, ulNumActBlocks, pCallBackFunc_isr, pUserCallBackParam );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_GetStatus(
	BDMA_Pci_Tran_Handle     hTran,
	BDMA_TranStatus *        peTranStatus )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_GetStatus_isr( hTran, peTranStatus );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);

}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Destroy(
	BDMA_Pci_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_Destroy_isr( hTran );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Reset(
	BDMA_Pci_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_Reset_isr( hTran );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Create2_isr(
	BDMA_Pci_Handle          hPciDma,
	uint32_t                 ulNumBlocks,
	bool                     bCachedDesc,
	BDMA_Pci_Tran_Handle  *  phTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Pci_Tran_Create_isr( hPciDma, ulNumBlocks, bCachedDesc, phTran );

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_SetBlockInfo_isr(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulMemBusAddr,
	uint32_t                 ulPciBusAddr,
	uint32_t                 ulBlockSize,
	BDMA_TranDir             eTranDirection )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Pci_Tran_SetBlockInfo_isr( hTran, ulBlockId,
											ulMemBusAddr, ulPciBusAddr,
											ulBlockSize,
											eTranDirection );

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartDmaAndCallBack_isr(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks,
	BDMA_Pci_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Pci_Tran_Start_isr( hTran, ulNumActBlocks, pCallBackFunc_isr, pUserCallBackParam );

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_GetStatus_isr(
	BDMA_Pci_Tran_Handle     hTran,
	BDMA_TranStatus *        peTranStatus )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Pci_Tran_GetStatus_isr( hTran, peTranStatus );

	return BERR_TRACE(eResult);

}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Destroy_isr(
	BDMA_Pci_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Pci_Tran_Destroy_isr( hTran );

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Reset_isr(
	BDMA_Pci_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Pci_Tran_Reset_isr( hTran );

	return BERR_TRACE(eResult);
}


/***************************************************************************
 * Obsolete Pci Dma API:
 **************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Create(
	BDMA_Handle           hDma,
	BDMA_Pci_Handle *     phPciDma )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BDMA_P_Context  *pDma;
	BDMA_P_Pci_Context  *pPciDma = NULL;
	BDMA_PciDmaEngine  eEngine;

	BDMA_P_MAIN_GET_CONTEXT( hDma, pDma );
	if (NULL == pDma)
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BDMA_Err_Pci_Create;
	}

	/* hDma passed the magic check, we trust every thing pointed by it now */
	*phPciDma = NULL;
	eResult = BKNI_AcquireMutex(pDma->hMutex);
	if (BERR_SUCCESS == eResult)
	{
		for (eEngine = BDMA_PciDmaEngine_e0; eEngine < BDMA_P_SUPPORT_PCI_DMA_ENGINE; eEngine ++)
		{
			pPciDma = BDMA_P_GetPciDmaHandle( hDma, eEngine );
			if (NULL == pPciDma)
				break;
		}

		if ( NULL == pPciDma )
		{
			/* eEngine is not used yet */
			eResult = BDMA_P_Pci_Create( hDma, eEngine, &s_stDefaultPciDmaSettings, phPciDma );
		}
		else
		{
			/* all pci dma engine are used */
			eResult = BDMA_ERR_ENGINE_OCCUPIED;
		}
		BKNI_ReleaseMutex(pDma->hMutex);
	}

  BDMA_Err_Pci_Create:
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Create(
	BDMA_Pci_Handle          hPciDma,
	uint32_t                 ulNumBlocks,
	BDMA_Pci_Tran_Handle  *  phTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_Create_isr( hPciDma, ulNumBlocks, false, phTran );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Start(
	BDMA_Pci_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_Start_isr(
		hTran, hTran->QueueEntry.ulNumTranBlocks, NULL, NULL );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartAndCallBack(
	BDMA_Pci_Tran_Handle     hTran,
	BDMA_Pci_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Pci_Tran_Start_isr( hTran, hTran->QueueEntry.ulNumTranBlocks,
										 pCallBackFunc_isr, pUserCallBackParam );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Create_isr(
	BDMA_Pci_Handle          hPciDma,
	uint32_t                 ulNumBlocks,
	BDMA_Pci_Tran_Handle  *  phTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Pci_Tran_Create_isr( hPciDma, ulNumBlocks, false, phTran );

	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartAndCallBack_isr(
	BDMA_Pci_Tran_Handle     hTran,
	BDMA_Pci_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Pci_Tran_Start_isr( hTran, hTran->QueueEntry.ulNumTranBlocks,
										 pCallBackFunc_isr, pUserCallBackParam );

	return BERR_TRACE(eResult);
}


#else /*of #if (BCHP_CHIP != 3548) && (BCHP_CHIP != 3556)*/

#include "bdma.h"
#include "bdma_errors.h"
#include "bkni.h"

BDBG_MODULE(BDMA_PCI);

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Create2(
	BDMA_Handle           hDma,
	BDMA_PciDmaEngine     eEngine,
	BDMA_Pci_Settings *   pSettings,
	BDMA_Pci_Handle *     phPciDma )
{
	BSTD_UNUSED(hDma);
	BSTD_UNUSED(eEngine);
	BSTD_UNUSED(pSettings);
	BSTD_UNUSED(phPciDma);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Destroy(
	BDMA_Pci_Handle       hPciDma )
{
	BSTD_UNUSED(hPciDma);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_GetDefaultSettings
	( BDMA_Pci_Settings     *pDefSettings )
{
	BSTD_UNUSED(pDefSettings);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_SetSwapMode(
	BDMA_Pci_Handle       hPciDma,
	BDMA_SwapMode         eSwapMode )
{
	BSTD_UNUSED(hPciDma);
	BSTD_UNUSED(eSwapMode);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_SetMaxBurstSize(
	BDMA_Pci_Handle       hPciDma,
	BDMA_MaxBurstSize     eMaxBurstSize )
{
	BSTD_UNUSED(hPciDma);
	BSTD_UNUSED(eMaxBurstSize);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Create2(
	BDMA_Pci_Handle          hPciDma,
	uint32_t                 ulNumBlocks,
	bool                     bCachedDesc,
	BDMA_Pci_Tran_Handle  *  phTran )
{
	BSTD_UNUSED(hPciDma);
	BSTD_UNUSED(ulNumBlocks);
	BSTD_UNUSED(bCachedDesc);
	BSTD_UNUSED(phTran);

return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_GetMaxNumBlocks(
	BDMA_Pci_Handle          hPciDma,
	uint32_t *               pulMaxNumBlocks )
{
	BSTD_UNUSED(hPciDma);
	BSTD_UNUSED(pulMaxNumBlocks);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_SetBlockInfo(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulMemBusAddr,
	uint32_t                 ulPciBusAddr,
	uint32_t                 ulBlockSize,
	BDMA_TranDir             eTranDirection )
{
	BSTD_UNUSED(hTran);
	BSTD_UNUSED(ulBlockId);
	BSTD_UNUSED(ulMemBusAddr);
	BSTD_UNUSED(ulPciBusAddr);
	BSTD_UNUSED(ulBlockSize);
	BSTD_UNUSED(eTranDirection);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartDma(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks )
{
	BSTD_UNUSED(hTran);
	BSTD_UNUSED(ulNumActBlocks);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartDmaAndCallBack(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks,
	BDMA_Pci_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BSTD_UNUSED(hTran);
	BSTD_UNUSED(ulNumActBlocks);
	BSTD_UNUSED(pCallBackFunc_isr);
	BSTD_UNUSED(pUserCallBackParam);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_GetStatus(
	BDMA_Pci_Tran_Handle     hTran,
	BDMA_TranStatus *        peTranStatus )
{
	BSTD_UNUSED(hTran);
	BSTD_UNUSED(peTranStatus);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Destroy(
	BDMA_Pci_Tran_Handle     hTran )
{
	BSTD_UNUSED(hTran);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Reset(
	BDMA_Pci_Tran_Handle     hTran )
{
	BSTD_UNUSED(hTran);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Create2_isr(
	BDMA_Pci_Handle          hPciDma,
	uint32_t                 ulNumBlocks,
	bool                     bCachedDesc,
	BDMA_Pci_Tran_Handle  *  phTran )
{
	BSTD_UNUSED(hPciDma);
	BSTD_UNUSED(ulNumBlocks);
	BSTD_UNUSED(bCachedDesc);
	BSTD_UNUSED(phTran);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_SetBlockInfo_isr(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulMemBusAddr,
	uint32_t                 ulPciBusAddr,
	uint32_t                 ulBlockSize,
	BDMA_TranDir             eTranDirection )
{
	BSTD_UNUSED(hTran);
	BSTD_UNUSED(ulBlockId);
	BSTD_UNUSED(ulMemBusAddr);
	BSTD_UNUSED(ulPciBusAddr);
	BSTD_UNUSED(ulBlockSize);
	BSTD_UNUSED(eTranDirection);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartDmaAndCallBack_isr(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks,
	BDMA_Pci_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BSTD_UNUSED(hTran);
	BSTD_UNUSED(ulNumActBlocks);
	BSTD_UNUSED(pCallBackFunc_isr);
	BSTD_UNUSED(pUserCallBackParam);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_GetStatus_isr(
	BDMA_Pci_Tran_Handle     hTran,
	BDMA_TranStatus *        peTranStatus )
{
	BSTD_UNUSED(hTran);
	BSTD_UNUSED(peTranStatus);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);

}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Destroy_isr(
	BDMA_Pci_Tran_Handle     hTran )
{
	BSTD_UNUSED(hTran);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Reset_isr(
	BDMA_Pci_Tran_Handle     hTran )
{
	BSTD_UNUSED(hTran);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}


/***************************************************************************
 * Obsolete Pci Dma API:
 **************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Create(
	BDMA_Handle           hDma,
	BDMA_Pci_Handle *     phPciDma )
{
	BSTD_UNUSED(hDma);
	BSTD_UNUSED(phPciDma);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Create(
	BDMA_Pci_Handle          hPciDma,
	uint32_t                 ulNumBlocks,
	BDMA_Pci_Tran_Handle  *  phTran )
{
	BSTD_UNUSED(hPciDma);
	BSTD_UNUSED(ulNumBlocks);
	BSTD_UNUSED(phTran);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Start(
	BDMA_Pci_Tran_Handle     hTran )
{
	BSTD_UNUSED(hTran);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartAndCallBack(
	BDMA_Pci_Tran_Handle     hTran,
	BDMA_Pci_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BSTD_UNUSED(hTran);
	BSTD_UNUSED(pCallBackFunc_isr);
	BSTD_UNUSED(pUserCallBackParam);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_Create_isr(
	BDMA_Pci_Handle          hPciDma,
	uint32_t                 ulNumBlocks,
	BDMA_Pci_Tran_Handle  *  phTran )
{
	BSTD_UNUSED(hPciDma);
	BSTD_UNUSED(ulNumBlocks);
	BSTD_UNUSED(phTran);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Pci_Tran_StartAndCallBack_isr(
	BDMA_Pci_Tran_Handle     hTran,
	BDMA_Pci_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BSTD_UNUSED(hTran);
	BSTD_UNUSED(pCallBackFunc_isr);
	BSTD_UNUSED(pUserCallBackParam);
	return BERR_TRACE(BDMA_ERR_ENGINE_NOT_SUPPORTED);
}

#endif

/* End of File */
