/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdma.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 2/2/11 10:24a $
 *
 * Module Description:
 *
 *
 * Revision History:
 * $brcm_Log: /magnum/portinginterface/dma/7400/bdma.c $
 * 
 * Hydra_Software_Devel/3   2/2/11 10:24a vanessah
 * SW7550-670: add Sharf DMA support
 * 
 * Hydra_Software_Devel/2   7/30/08 11:53a syang
 * PR 45128: fix ptr over-run caused by using BDMA_MemDmaEngine_eUnknown
 * for engine handle array boundary
 * 
 * Hydra_Software_Devel/10   2/21/08 1:37p rpan
 * PR34854: Initial work for bcm3548.
 * 
 * Hydra_Software_Devel/9   1/30/08 10:53a rpan
 * PR34606: Relaxed input parameter check a bit.
 * 
 * Hydra_Software_Devel/8   12/7/07 4:10p syang
 * PR 34606: rewrite more than half of code to support hw sharing for
 * kernel and user-mode sides
 * 
 * Hydra_Software_Devel/7   10/15/07 12:01p syang
 * PR 35045, PR 35970: coverity (RESOURCE_LEAK) fix
 * 
 * Hydra_Software_Devel/6   10/25/06 3:12p syang
 * PR 24634: fix compile warning
 * 
 * Hydra_Software_Devel/5   10/3/06 3:59p syang
 * PR 24634: replace critical section with private mutex for
 * mem/pci_dma_handle create/destroy.
 * 
 * Hydra_Software_Devel/4   8/4/04 4:38p syang
 * PR 12172: fix warning with release build
 * 
 * Hydra_Software_Devel/3   11/19/03 6:40p syang
 * added black magic set to _create
 * 
 * Hydra_Software_Devel/2   10/23/03 11:07a syang
 * first time checking after resolving comipling errors
 * 
 * Hydra_Software_Devel/1   10/7/03 3:51p syang
 * init version
 * 
 ***************************************************************************/
#include "bdma.h"             /*  */
#include "bdma_priv.h"        /*  */
#include "bkni.h"

BDBG_MODULE(BDMA);


/***************************************************************************
 *
 */
BERR_Code BDMA_Open(
	BDMA_Handle *         phDma,
	BCHP_Handle           hChip,
	BREG_Handle           hRegister,
	BMEM_Handle           hMemory,
	BINT_Handle           hInterrupt )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BDMA_P_Context *  pDma = NULL;
	BKNI_MutexHandle hMutex = NULL;

	if ( (NULL == phDma) ||
		 (0    == hRegister) ||
		 (NULL == hMemory) ) 
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BDMA_P_Err_Main_Open;		
	}
	
	pDma = (BDMA_P_Context *)BKNI_Malloc( sizeof(BDMA_P_Context) );
	if ( NULL == pDma )
	{
		eResult = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BDMA_P_Err_Main_Open;
	}
	BKNI_Memset((void*)pDma, 0x0, sizeof(BDMA_P_Context));

	eResult = BKNI_CreateMutex(&hMutex);
	if ((BERR_SUCCESS != eResult) || (NULL == hMutex))
	{
#if BDBG_DEBUG_BUILD == 1	
		BERR_TRACE(eResult);
#endif		
		goto BDMA_P_Err_Main_Open;		
	}
	
	pDma->hChip = hChip;
	pDma->hRegister = hRegister;
	pDma->hMemory = hMemory;
	pDma->hInterrupt = hInterrupt;
	pDma->hMutex = hMutex;
	/* hMemDma = NULL; */
	/* hPciDma = NULL; */
	BDMA_P_MAIN_SET_BLACK_MAGIC(pDma);

	*phDma = pDma;
	return eResult;

  BDMA_P_Err_Main_Open:
	if ( NULL != pDma )
		BDMA_P_MAIN_DESTROY_CONTEXT(pDma);
	
	if ( NULL != phDma )
		*phDma = NULL;
		
	return eResult;
}

/***************************************************************************
 *
 */
BERR_Code BDMA_Close(
	BDMA_Handle           hDma )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BDMA_P_Context *  pDma;
#if BDMA_P_SUPPORT_MEM_DMA_ENGINE
	BDMA_MemDmaEngine  eMem;
#endif

#if BDMA_P_SUPPORT_PCI_DMA_ENGINE
	BDMA_PciDmaEngine  ePci;
#endif
#if BDMA_P_SUPPORT_SHARF_DMA_ENGINE
	BDMA_Sharf         eSharf;
#endif

	BDMA_P_MAIN_GET_CONTEXT( hDma, pDma );
	if ( NULL == pDma )
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		goto BDMA_P_Done_Main_Close;		
	}

	/* sub-modules have to be explicitly destroied first */
#if BDMA_P_SUPPORT_MEM_DMA_ENGINE	
	for (eMem = BDMA_MemDmaEngine_e0; eMem < BDMA_P_SUPPORT_MEM_DMA_ENGINE; eMem ++)
	{
		if (NULL != pDma->hMemDma[eMem])
		{
			eResult = BERR_TRACE(BERR_LEAKED_RESOURCE);
			goto BDMA_P_Done_Main_Close;		
		}
	}
#endif



#if BDMA_P_SUPPORT_PCI_DMA_ENGINE
	for (ePci = BDMA_PciDmaEngine_e0; ePci < BDMA_P_SUPPORT_PCI_DMA_ENGINE; ePci ++)
	{
		if (NULL != pDma->hPciDma[ePci])
		{
			eResult = BERR_TRACE(BERR_LEAKED_RESOURCE);
			goto BDMA_P_Done_Main_Close;		
		}
	}
#endif


#if BDMA_P_SUPPORT_SHARF_DMA_ENGINE
		for (eSharf = BDMA_Sharf_e0; eSharf < BDMA_Sharf_eInvalid; eSharf ++)
		{
			if (NULL != pDma->hSharfDma[eSharf])
		{
			eResult = BERR_TRACE(BERR_LEAKED_RESOURCE);
			goto BDMA_P_Done_Main_Close;		
		}
	}
#endif

	if (pDma->hMutex)
	{
		BKNI_DestroyMutex(pDma->hMutex);
	}
		
	BDMA_P_MAIN_DESTROY_CONTEXT(pDma);

  BDMA_P_Done_Main_Close:
	return eResult;	
}

/* End of File */
