/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdma_pci_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 3/21/11 6:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/dma/7400/bdma_pci_priv.h $
 * 
 * Hydra_Software_Devel/3   3/21/11 6:39p vanessah
 * SW7420-1431: support oflarge DMA descriptor list subset
 * 
 * Hydra_Software_Devel/2   7/11/08 3:46p syang
 * PR 43729:  rm code for old crypto hw arch
 * 
 * Hydra_Software_Devel/1   7/11/08 1:16p syang
 * PR 43729: break from the old 7038
 * 
 * Hydra_Software_Devel/13   12/7/07 4:09p syang
 * PR 34606: rewrite more than half of code to support hw sharing for
 * kernel and user-mode sides
 * 
 * Hydra_Software_Devel/12   10/15/07 3:42p syang
 * PR 35947: add "_isr" to call back func name
 * 
 * Hydra_Software_Devel/11   3/16/07 12:50p syang
 * PR 28173: add dynamic link support (not turn on yet)
 * 
 * Hydra_Software_Devel/9   2/5/07 4:43p syang
 * PR 23354: allow mem addr in mem_1; cleanup cip specific code section
 * macro usage
 * 
 * Hydra_Software_Devel/8   10/3/06 4:01p syang
 * PR 24634: replace critical section with private mutex for
 * mem/pci_dma_handle create/destroy.
 * 
 * Hydra_Software_Devel/7   2/4/04 3:01p syang
 * PR 9608: 1). added init for hMemDma / hPciDma of Channel, hDma of
 * MemDma / PciDma and hChannel of Queue; 2). split CallBackFunc type def
 * to mem and pci versions; 3). fixed typo in TranHandle def; 4).leave
 * critical section before calling BINT func; 5). fixed a bug with
 * nextDesc in Desc info setting; 6). use wake to start dma as in sleep
 * mode; 7). corrected typo in the asserts of Queue_CreateTran.
 * 
 * Hydra_Software_Devel/6   10/24/03 2:53p syang
 * changed to use PciBusAddr and MemBusAddr from SrcBusAddr and
 * DstBusAddr. I think the new addr model is more natural to the way how
 * user gets the addr
 * 
 * Hydra_Software_Devel/5   10/23/03 6:18p syang
 * to make linux comipler silent
 * 
 * Hydra_Software_Devel/4   10/23/03 4:20p syang
 * adjusted sub-module implementaion overview
 * 
 * Hydra_Software_Devel/3   10/23/03 3:16p syang
 * added real pci dma implementation
 * 
 * Hydra_Software_Devel/2   10/10/03 4:05p syang
 * updated BDMA_Pci_Context
 * 
 * Hydra_Software_Devel/1   9/25/03 6:45p syang
 * init version, from scratch
 * 
 ***************************************************************************/
#ifndef BDMA_PCI_PRIV_H__
#define BDMA_PCI_PRIV_H__

#include "bdma_priv.h"
#include "bdma_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * {private}

 * Implementation Overview:
 *
 * The implementation uses a private queue module, to manage the active
 * transfers. The queue module is shared by both memory DMA and PCI DMA.
 * It manages the queue entry assigning, freeing, and the transfer status.
 * Refer to queue module overview for more info.
 * 
 * PCI DMA sub-module code should never refer to the member of queue
 * module directly, however it needs to know the def of struct
 * BDMA_P_QueueEntry.
 *
 * By abstracting out the queue module, the readabilty, maintainabilty, and
 * extendabilty (such as to IO DMA) are much improved. Code size is also
 * improved.
 ****************************************************************************/

#define BDMA_P_PCI_GET_CONTEXT(handle, context) \
	BDMA_P_GENERIC_GET_CONTEXT((handle), (context), BDMA_P_Pci_Context)

#define BDMA_P_PCI_SET_BLACK_MAGIC(handle) \
	BDMA_P_GENERIC_SET_BLACK_MAGIC((handle), BDMA_P_Pci_Context)

#define BDMA_P_PCI_DESTROY_CONTEXT(struct_ptr) \
	BDMA_P_GENERIC_DESTROY_CONTEXT(struct_ptr, BDMA_P_Pci_Context)
	
#define BDMA_P_PCI_NUM_WORDS_PER_DESC         4
	
#define BDMA_P_PCI_MAX_BLOCK_SIZE             (0x1000000 -1)  /* 16 MBytes */
#define BDMA_P_PCI_ADDR_MASK                  (0xffffffff)
#define BDMA_P_PCI_LITTLE_ENDIAN   1

#define BDMA_P_PCI_END_IF_ERR(result, done_lable) \
	if ( BERR_SUCCESS != (result)) \
	{\
		goto done_lable;  \
	}\

/***************************************************************************
 * Pci Tran Handle
 */
typedef struct BDMA_P_Pci_TranContext
{
	BDMA_P_QueueEntry  QueueEntry;
	
} BDMA_P_Pci_TranContext;


/***************************************************************************
 * Pci Dma Context
 */
typedef struct BDMA_P_Pci_Context
{
	uint32_t   ulBlackMagic;   /* Black magic for handle validation */

	/* created from this handle */
	BDMA_Handle    hDma;

	/* engine id */
	BDMA_PciDmaEngine  eEngine;

	/* configures */
	BDMA_SwapMode      eSwapMode;       /* swap mode */
	BDMA_MaxBurstSize  eMaxBurstSize;	/* max burst size mode */
	
	/* tran entry queue */
	BDMA_P_QueueHandle  hQueue;
	 
	/* interrupt call back handle */
	BINT_CallbackHandle  hCallBack;	

} BDMA_P_Pci_Context;


/***************************************************************************
 *
 * Utility functions used by BDMA
 *
 ***************************************************************************/
/*--------------------------------------------------------------------------
 * To be called by BDMA API func before taking real action, to get dma mutex
 * WITH block, ideally for user mode
 */
BERR_Code BDMA_P_Pci_AcquireMutex(
	BDMA_Pci_Handle          hPciDma,
	BKNI_MutexHandle        *phMutex );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Pci_Init(
	BDMA_P_Pci_Context  * pPciDma,
	const BDMA_Pci_Settings *pSettings,
	uint32_t              ulL2IntrBintId );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Pci_UnInit(
	BDMA_P_Pci_Context  * pPciDma );

/***************************************************************************
 *
 * API support functions
 *
 ***************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Pci_Create(
	BDMA_Handle           hDma,
	BDMA_PciDmaEngine     eEngine,
	const BDMA_Pci_Settings *pSettings,	
	BDMA_Pci_Handle *     phPciDma );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Pci_Destroy(
	BDMA_Pci_Handle          hPciDma );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Pci_SetSwapMode_isr(
	BDMA_Pci_Handle          hPciDma,
	BDMA_SwapMode            eSwapMode );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Pci_SetMaxBurstSize_isr(
	BDMA_Pci_Handle       hPciDma,
	BDMA_MaxBurstSize     eMaxBurstSize );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Pci_Tran_Create_isr(
	BDMA_Pci_Handle          hPciDma,
	uint32_t                 ulNumBlocks,
	bool                     bCachedDesc,
	BDMA_Pci_Tran_Handle  *  phTran );

/***************************************************************************
 * To be called to set the block info for a Tran block. Input para are
 * validated.
 */
BERR_Code BDMA_P_Pci_Tran_SetBlockInfo_isr(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulMemBusAddr,
	uint32_t                 ulPciBusAddr,
	uint32_t                 ulBlockSize,
	BDMA_TranDir             eTranDirection );

/***************************************************************************
 * To be called by both BDMA_P_Pci_Tran_Start and
 * BDMA_P_Pci_Tran_StartAndCallBack, BDMA_P_Pci_Tran_Start should pass NULL
 * for pCallBackFunc_isr and pUserCallBackParam
 */
BERR_Code BDMA_P_Pci_Tran_Start_isr(
	BDMA_Pci_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks,
	BDMA_Pci_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam );

/***************************************************************************
 * To be called by both BDMA_Pci_Tran_StartDmaSubset and
 * BDMA_Pci_Tran_StartDmaSubsetAndCallBack. 
 */

BERR_Code BDMA_P_Pci_Tran_StartSubset_isr(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulFirstBlock,
	uint32_t                 ulNumActBlocks,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam );


/***************************************************************************
 * To be called by BDMA_Pci_Tran_GetStatus. 
 */
BERR_Code BDMA_P_Pci_Tran_GetStatus_isr(
	BDMA_Pci_Tran_Handle     hTran,
	BDMA_TranStatus *        peTranStatus );

/***************************************************************************
 * To be called by BDMA_Pci_Tran_Destroy. It free the queue entries occupied
 * this Tran
 */
BERR_Code BDMA_P_Pci_Tran_Destroy_isr(
	BDMA_Pci_Tran_Handle     hTran );


/***************************************************************************
 * To be called by BDMA_Pci_Tran_Reset. It resets the Tran into its 
 * initially created state
 */
BERR_Code BDMA_P_Pci_Tran_Reset_isr(
	BDMA_Pci_Tran_Handle     hTran );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BDMA_PCI_PRIV_H__ */

/* end of file */
