/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdma_mem.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 3/21/11 6:37p $
 *
 * Module Description:
 *
 *
 * Revision History:
 * $brcm_Log: /magnum/portinginterface/dma/7400/bdma_mem.c $
 * 
 * Hydra_Software_Devel/9   3/21/11 6:37p vanessah
 * SW7420-1431: support oflarge DMA descriptor list subset
 * 
 * Hydra_Software_Devel/8   2/22/11 7:48a vanessah
 * SW7550-670: add Sharf DMA support
 * 
 * Hydra_Software_Devel/7   2/2/11 10:26a vanessah
 * SW7550-670: add Sharf DMA support
 * 
 * Hydra_Software_Devel/6   1/18/11 4:53p cdisc
 * SW35125-56: adding 35125 BCHP precompile lines and removing 35125 stub
 * functions
 * 
 * Hydra_Software_Devel/5   11/9/10 5:11p vanessah
 * SW35125-11:  DMA PI support for 35125
 * 
 * Hydra_Software_Devel/4   8/20/08 4:07p syang
 * PR 45892 : took off unnecessary criticalsection protection
 * 
 * Hydra_Software_Devel/3   7/30/08 11:53a syang
 * PR 45128: fix ptr over-run caused by using BDMA_MemDmaEngine_eUnknown
 * for engine handle array boundary
 * 
 * Hydra_Software_Devel/2   7/11/08 3:45p syang
 * PR 43729:  rm code for old crypto hw arch
 * 
 * Hydra_Software_Devel/24   12/7/07 6:38p syang
 * PR 34606: move SetCrypto to Tran level from Engine level for better
 * sharing by threads of the same progress
 * 
 * Hydra_Software_Devel/23   12/7/07 4:10p syang
 * PR 34606: rewrite more than half of code to support hw sharing for
 * kernel and user-mode sides
 * 
 * Hydra_Software_Devel/22   10/15/07 3:37p syang
 * PR 35947: add "_isr" to call back func name
 * 
 * Hydra_Software_Devel/21   3/16/07 3:23p syang
 * PR 28173: add scatter-gather support
 * 
 * Hydra_Software_Devel/20   3/16/07 12:51p syang
 * PR 28173: add dynamic link support (not turn on yet)
 * 
 * Hydra_Software_Devel/18   10/3/06 4:00p syang
 * PR 24634: replace critical section with private mutex for
 * mem/pci_dma_handle create/destroy.
 * 
 * Hydra_Software_Devel/17   3/23/06 12:41p syang
 * PR 19670: added support for 7438
 * 
 * Hydra_Software_Devel/16   7/14/05 5:12p syang
 * PR 16059: updated 7401 crypto configure
 * 
 * Hydra_Software_Devel/15   7/8/05 4:00p nissen
 * PR 15575: Added BDMA_Mem_Transfer() entry point function to do memory
 * transfers with less function call overhead.
 * 
 * Hydra_Software_Devel/14   5/19/05 4:46p syang
 * PR 14516: added obsolete comments to older API Func, and added _isr
 * version of SetDmaBlockInfo
 * 
 * Hydra_Software_Devel/13   5/18/05 9:44p nissen
 * PR 14516: Added support for setting ENC_DEC_INIT and removed word
 * alignment check of TRANSFER_SIZE.
 * 
 * Hydra_Software_Devel/12   5/18/05 9:00p nissen
 * PR 15312: Added support for big endian memory transfers.
 * 
 * Hydra_Software_Devel/11   5/13/05 4:23p syang
 * PR 15269: added implementation of BDMA_Mem_Tran_GetMaxNumBlocks and
 * BDMA_Pci_Tran_GetMaxNumBlocks
 * 
 * Hydra_Software_Devel/10   5/24/04 5:58p jasonh
 * PR 11189: Merge down from B0 to main-line
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   5/6/04 2:23p syang
 * PR 10685:  In order to support new crypt features of B0, changed DesSel
 * to CryptAlg,  expanded Key sel, added MODE_SEL and ENC_DEC_INIT
 * setting (changed to use mode 1, mode 0 is not used again),
 * 
 * Hydra_Software_Devel/9   2/4/04 3:00p syang
 * PR 9608: 1). added init for hMemDma / hPciDma of Channel, hDma of
 * MemDma / PciDma and hChannel of Queue; 2). split CallBackFunc type def
 * to mem and pci versions; 3). fixed typo in TranHandle def; 4).leave
 * critical section before calling BINT func; 5). fixed a bug with
 * nextDesc in Desc info setting; 6). use wake to start dma as in sleep
 * mode; 7). corrected typo in the asserts of Queue_CreateTran.
 * 
 * Hydra_Software_Devel/8   10/23/03 6:17p syang
 * to make linux comipler silent
 * 
 * Hydra_Software_Devel/7   10/23/03 4:20p syang
 * adjusted sub-module implementaion overview
 * 
 * Hydra_Software_Devel/6   10/23/03 11:07a syang
 * first time checking after resolving comipling errors
 * 
 * Hydra_Software_Devel/5   10/21/03 4:34p syang
 * added implementation of StartAndCallBack
 * 
 * Hydra_Software_Devel/4   10/20/03 12:15p syang
 * updated to use TranHandle (rather than TranId)
 * 
 * Hydra_Software_Devel/3   10/10/03 4:15p syang
 * added peStatus to CallAndWait
 * 
 * Hydra_Software_Devel/2   10/3/03 6:14p syang
 * added real code
 * 
 * Hydra_Software_Devel/1   9/25/03 7:02p syang
 * init version, from scratch
 * 
 ***************************************************************************/


#include "bdma_mem_priv.h"    /*  */
#include "bkni.h"

BDBG_MODULE(BDMA_MEM);

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
static const BDMA_Mem_Settings s_stDefaultMemDmaSettings =
{
	BDMA_P_NUM_QUEUE_ENTRIES,
	BDMA_P_MAX_NUM_BLOCKS,

	/* deafult for the case that only one BDMA instance exists in the system */
	NULL, NULL, NULL, 0, true
};

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_GetDefaultSettings
	( BDMA_Mem_Settings     *pDefSettings )
{
	if (NULL != pDefSettings)
	{
		*pDefSettings = s_stDefaultMemDmaSettings;
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
BERR_Code BDMA_Mem_SetByteSwapMode(
	BDMA_Mem_Handle          hMemDma,
	BDMA_Endian              eReadEndian,
	BDMA_SwapMode            eSwapMode )
{
	BERR_Code  eResult = BERR_SUCCESS;

	/*BKNI_EnterCriticalSection();*/
	eResult = BDMA_P_Mem_SetByteSwapMode_isr( hMemDma, eReadEndian, eSwapMode );
	/*BKNI_LeaveCriticalSection();*/
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_Create2(
	BDMA_Mem_Handle          hMemDma,
	uint32_t                 ulNumBlocks,
	bool                     bCachedDesc,
	BDMA_Mem_Tran_Handle  *  phTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();	
	eResult = BDMA_P_Mem_Tran_Create_isr( hMemDma, ulNumBlocks, bCachedDesc, phTran );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_Create2_isr(
	BDMA_Mem_Handle          hMemDma,
	uint32_t                 ulNumBlocks,
	bool                     bCachedDesc,
	BDMA_Mem_Tran_Handle  *  phTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_Create_isr( hMemDma, ulNumBlocks, bCachedDesc, phTran );
		
	return BERR_TRACE(eResult);
}


/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_Create(
	BDMA_Mem_Handle          hMemDma,
	uint32_t                 ulNumBlocks,
	BDMA_Mem_Tran_Handle  *  phTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();	
	eResult = BDMA_P_Mem_Tran_Create_isr( hMemDma, ulNumBlocks, false, phTran );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_Create_isr(
	BDMA_Mem_Handle          hMemDma,
	uint32_t                 ulNumBlocks,
	BDMA_Mem_Tran_Handle  *  phTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_Create_isr( hMemDma, ulNumBlocks, false, phTran );
		
	return BERR_TRACE(eResult);
}


/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_GetMaxNumBlocks(
	BDMA_Mem_Handle          hMemDma,
	uint32_t *               pulMaxNumBlocks )
{
	BERR_Code  eResult = BERR_SUCCESS;

	if ( (NULL == hMemDma) || (NULL == pulMaxNumBlocks) )
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BDMA_Done_Mem_Tran_GetMaxNumBlocks;
	}
	*pulMaxNumBlocks = BDMA_P_MAX_NUM_BLOCKS;

  BDMA_Done_Mem_Tran_GetMaxNumBlocks:
	
	return BERR_TRACE(eResult);
}



/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_GetStatus(
	BDMA_Mem_Tran_Handle     hTran,
	BDMA_TranStatus *        peTranStatus )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_Tran_GetStatus_isr( hTran, peTranStatus );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
 
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_Destroy(
	BDMA_Mem_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_Tran_Destroy_isr( hTran );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_Reset(
	BDMA_Mem_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	/*BKNI_EnterCriticalSection();*/
	eResult = BDMA_P_Mem_Tran_Reset_isr( hTran );
	/*BKNI_LeaveCriticalSection();*/
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_GetStatus_isr(
	BDMA_Mem_Tran_Handle     hTran,
	BDMA_TranStatus *        peTranStatus )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_GetStatus_isr( hTran, peTranStatus );
		
	return BERR_TRACE(eResult);
 
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_Destroy_isr(
	BDMA_Mem_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_Destroy_isr( hTran );
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_Reset_isr(
	BDMA_Mem_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_Reset_isr( hTran );
		
	return BERR_TRACE(eResult);
}


#if BDMA_P_SUPPORT_MEM_DMA_ENGINE

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Create2(
	BDMA_Handle           hDma,
	BDMA_MemDmaEngine     eEngine,
	BDMA_Mem_Settings *   pSettings,	
	BDMA_Mem_Handle *     phMemDma )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BDMA_P_Context  *pDma;

	BDMA_P_MAIN_GET_CONTEXT( hDma, pDma );
	if (NULL == pDma)
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BDMA_Err_Mem_Create;
	}
	
	/* hDma passed the magic check, we trust every thing pointed by it now */
	eResult = BKNI_AcquireMutex(pDma->hMutex);
	if (BERR_SUCCESS == eResult)
	{
		if (NULL != pSettings)
			eResult = BDMA_P_Mem_Create( hDma, eEngine, pSettings, phMemDma );
		else
			eResult = BDMA_P_Mem_Create( hDma, eEngine, &s_stDefaultMemDmaSettings, phMemDma );
		BKNI_ReleaseMutex(pDma->hMutex);
	}

  BDMA_Err_Mem_Create:
	return BERR_TRACE(eResult);
}


/***************************************************************************
 * Obsolete Memory Dma API: 
 **************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Create(
	BDMA_Handle           hDma,
	BDMA_Mem_Handle *     phMemDma )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BDMA_P_Context  *pDma;
	BDMA_P_Mem_Context  *pMemDma = NULL;
	BDMA_MemDmaEngine  eEngine;
	
	BDMA_P_MAIN_GET_CONTEXT( hDma, pDma );
	if (NULL == pDma)
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BDMA_Err_Mem_Create;
	}
	
	/* hDma passed the magic check, we trust every thing pointed by it now */
	*phMemDma = NULL;
	eResult = BKNI_AcquireMutex(pDma->hMutex);
	if (BERR_SUCCESS == eResult)
	{
		for (eEngine = BDMA_MemDmaEngine_e0; eEngine < BDMA_P_SUPPORT_MEM_DMA_ENGINE; eEngine ++)
		{
			pMemDma = BDMA_P_GetMemDmaHandle( hDma, eEngine );
			if (NULL == pMemDma)
				break;
		}
		
		if ( NULL == pMemDma )
		{
			/* eEngine is not used yet */
			eResult = BDMA_P_Mem_Create( hDma, eEngine, &s_stDefaultMemDmaSettings, phMemDma );
		}
		else
		{
			/* all mem dma engine are used */
			eResult = BDMA_ERR_ENGINE_OCCUPIED;
		}
		BKNI_ReleaseMutex(pDma->hMutex);
	}

  BDMA_Err_Mem_Create:
	return BERR_TRACE(eResult);
}



/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Destroy(
	BDMA_Mem_Handle       hMemDma )
{
	BKNI_MutexHandle  hMutex;
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_AcquireMutex( hMemDma, &hMutex );
	if (BERR_SUCCESS == eResult)
	{
		eResult = BDMA_P_Mem_Destroy( hMemDma );
		BKNI_ReleaseMutex(hMutex);
	}
		
	return BERR_TRACE(eResult);
}



/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_SetCrypto(
	BDMA_Mem_Tran_Handle     hTran,
	BDMA_CryptMode           eCryptMode,
	uint32_t                 ulKeySlot,
	bool                     bSgEnable )
{
	BERR_Code  eResult = BERR_SUCCESS;

	/*BKNI_EnterCriticalSection();*/
	eResult = BDMA_P_Mem_Tran_SetCrypto_isr( hTran, eCryptMode, ulKeySlot, bSgEnable );
	/*BKNI_LeaveCriticalSection();*/
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_SetCrypto_isr(
	BDMA_Mem_Tran_Handle     hTran,
	BDMA_CryptMode           eCryptMode,
	uint32_t                 ulKeySlot,
	bool                     bSgEnable )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_SetCrypto_isr( hTran, eCryptMode, ulKeySlot, bSgEnable );
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_SetCrypt(
	BDMA_Mem_Handle          hMemDma,
	BDMA_CryptMode           eCryptMode,
	uint32_t                 ulKeySlot )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_SetCrypto_isr( hMemDma, eCryptMode, ulKeySlot, false );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_SetCrypto(
	BDMA_Mem_Handle          hMemDma,
	BDMA_CryptMode           eCryptMode,
	uint32_t                 ulKeySlot,
	bool                     bSgEnable )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_SetCrypto_isr( hMemDma, eCryptMode, ulKeySlot, bSgEnable );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}


/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_SetDmaBlockInfo(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulDstBusAddr,
	uint32_t                 ulSrcBusAddr,
	uint32_t                 ulBlockSize,
	bool                     bCryptInit )
{
	BERR_Code  eResult = BERR_SUCCESS;

	/*BKNI_EnterCriticalSection();*/
	eResult = BDMA_P_Mem_Tran_SetDmaBlockInfo_isr( hTran, ulBlockId,
											ulDstBusAddr, ulSrcBusAddr,
											ulBlockSize, bCryptInit );
	/*BKNI_LeaveCriticalSection();*/
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_SetDmaBlockInfo_isr(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulDstBusAddr,
	uint32_t                 ulSrcBusAddr,
	uint32_t                 ulBlockSize,
	bool                     bCryptInit )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_SetDmaBlockInfo_isr( hTran, ulBlockId,
											ulDstBusAddr, ulSrcBusAddr,
											ulBlockSize, bCryptInit );
		
	return BERR_TRACE(eResult);
}


/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_SetSgStartEnd(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	bool                     bSgStart,
	bool                     bSgEnd )
{
	BERR_Code  eResult = BERR_SUCCESS;

	/*BKNI_EnterCriticalSection();*/
	eResult = BDMA_P_Mem_Tran_SetSgStartEnd_isr( hTran, ulBlockId,
												 bSgStart, bSgEnd );
	/*BKNI_LeaveCriticalSection();*/
		
	return BERR_TRACE(eResult);
}


/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_SetSgStartEnd_isr(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	bool                     bSgStart,
	bool                     bSgEnd )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_SetSgStartEnd_isr( hTran, ulBlockId,
												 bSgStart, bSgEnd );
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Transfer(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulDstBusAddr,
	uint32_t                 ulSrcBusAddr,
	uint32_t                 ulBlockSize,
	bool                     bCryptInit,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_Transfer_isr( hTran, 
		ulBlockId, ulDstBusAddr, ulSrcBusAddr, ulBlockSize, bCryptInit, 
		pCallBackFunc_isr, pUserCallBackParam );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);

}



/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_StartAndCallBack_isr(
	BDMA_Mem_Tran_Handle     hTran,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_Start_isr( hTran, hTran->QueueEntry.ulNumTranBlocks,
										 pCallBackFunc_isr, pUserCallBackParam );
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_Start(
	BDMA_Mem_Tran_Handle     hTran )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_Tran_Start_isr(
		hTran, hTran->QueueEntry.ulNumTranBlocks, NULL, NULL );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_StartAndCallBack(
	BDMA_Mem_Tran_Handle     hTran,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_Tran_Start_isr( hTran, hTran->QueueEntry.ulNumTranBlocks,
										 pCallBackFunc_isr, pUserCallBackParam );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_StartDma(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_Tran_Start_isr( hTran, ulNumActBlocks, NULL, NULL );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */

BERR_Code BDMA_Mem_Tran_StartDmaSubset(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulFirstBlock,
	uint32_t                 ulNumActBlocks )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_Tran_StartSubset_isr( hTran, ulFirstBlock, ulNumActBlocks, NULL, NULL );
	BKNI_LeaveCriticalSection();

	return BERR_TRACE(eResult);
}
/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_StartDmaAndCallBack(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_Tran_Start_isr( hTran, ulNumActBlocks, pCallBackFunc_isr, pUserCallBackParam );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_StartDmaSubsetAndCallBack(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulFirstBlock,
	uint32_t                 ulNumActBlocks,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BKNI_EnterCriticalSection();
	eResult = BDMA_P_Mem_Tran_StartSubset_isr( hTran, ulFirstBlock, ulNumActBlocks, pCallBackFunc_isr, pUserCallBackParam );
	BKNI_LeaveCriticalSection();
		
	return BERR_TRACE(eResult);
}


/***************************************************************************
 *
 */
BERR_Code BDMA_Mem_Tran_StartDmaAndCallBack_isr(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam )
{
	BERR_Code  eResult = BERR_SUCCESS;

	eResult = BDMA_P_Mem_Tran_Start_isr( hTran, ulNumActBlocks, pCallBackFunc_isr, pUserCallBackParam );
		
	return BERR_TRACE(eResult);
}

#endif






