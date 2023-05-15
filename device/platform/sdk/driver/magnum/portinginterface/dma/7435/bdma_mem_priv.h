/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdma_mem_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 5/2/11 1:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/dma/7400/bdma_mem_priv.h $
 * 
 * Hydra_Software_Devel/7   5/2/11 1:30p vanessah
 * SWDTV-6186: 35233 DMA support
 * 
 * Hydra_Software_Devel/6   3/21/11 6:47p vanessah
 * SW7420-1431: Large descriptor list subset support
 * 
 * Hydra_Software_Devel/5   1/18/11 4:54p cdisc
 * SW35125-56: adding 35125 BCHP precompile lines and removing 35125 stub
 * functions
 * 
 * Hydra_Software_Devel/4   6/18/10 2:43p cdisc
 * SW35230-352: M2M and partial key ladder support (adjusting
 * BDMA_P_MEM_KEY_SIZE for 35230 only)
 * 
 * Hydra_Software_Devel/3   10/3/08 5:29p syang
 * PR 34606: support 41 key slots for 7405 B0
 * 
 * Hydra_Software_Devel/2   7/11/08 3:45p syang
 * PR 43729:  rm code for old crypto hw arch
 * 
 * Hydra_Software_Devel/1   7/11/08 1:15p syang
 * PR 43729: break from the old 7038
 * 
 * Hydra_Software_Devel/28   12/7/07 6:55p syang
 * PR 34606:  fix compile warning
 * 
 * Hydra_Software_Devel/27   12/7/07 6:38p syang
 * PR 34606: move SetCrypto to Tran level from Engine level for better
 * sharing by threads of the same progress
 * 
 * Hydra_Software_Devel/26   12/7/07 4:09p syang
 * PR 34606: rewrite more than half of code to support hw sharing for
 * kernel and user-mode sides
 * 
 * Hydra_Software_Devel/25   10/15/07 3:37p syang
 * PR 35947: add "_isr" to call back func name
 * 
 * Hydra_Software_Devel/24   3/19/07 4:18p syang
 * PR 28171: prepare to add sharf support
 * 
 * Hydra_Software_Devel/23   3/16/07 3:22p syang
 * PR 28173: add scatter-gather support
 * 
 * Hydra_Software_Devel/22   3/16/07 12:50p syang
 * PR 28173: add dynamic link support (not turn on yet)
 * 
 * Hydra_Software_Devel/20   3/7/07 4:26p tdo
 * PR 26224:  Add support for 2nd MEM DMA for 7400B0
 * 
 * Hydra_Software_Devel/19   2/5/07 4:42p syang
 * PR 23354: allow mem addr in mem_1; cleanup cip specific code section
 * macro usage
 * 
 * Hydra_Software_Devel/18   1/18/07 4:01p syang
 * PR 16059: update BDMA_P_MEM_ADDR_MASK for 7401 according to request
 * from Michael Phillips
 * 
 * Hydra_Software_Devel/17   1/8/07 12:53p tdo
 * PR 26224:  Add second DMA MEM engine support for 7400 B0
 * 
 * Hydra_Software_Devel/16   10/3/06 4:00p syang
 * PR 24634: replace critical section with private mutex for
 * mem/pci_dma_handle create/destroy.
 * 
 * Hydra_Software_Devel/15   3/23/06 12:42p syang
 * PR 19670: added support for 7438
 * 
 * Hydra_Software_Devel/14   2/3/06 4:51p syang
 * PR 19425: allow mem addr bigger than (1<<28)
 * 
 * Hydra_Software_Devel/13   7/14/05 5:11p syang
 * PR 16059: updated 7401 crypto configure
 * 
 * Hydra_Software_Devel/12   7/8/05 3:48p nissen
 * PR 15575: Added BDMA_Mem_Transfer() entry point function to do memory
 * transfers with less function call overhead.
 * 
 * Hydra_Software_Devel/11   5/18/05 9:51p nissen
 * PR 14516: Added support for setting ENC_DEC_INIT and removed word
 * alignment check of TRANSFER_SIZE.
 * 
 * Hydra_Software_Devel/10   5/18/05 9:07p nissen
 * PR 15312: Added support for big endian memory transfers.
 * 
 * Hydra_Software_Devel/9   5/24/04 5:59p jasonh
 * PR 11189: Merge down from B0 to main-line
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   5/6/04 2:23p syang
 * PR 10685:  In order to support new crypt features of B0, changed DesSel
 * to CryptAlg,  expanded Key sel, added MODE_SEL and ENC_DEC_INIT
 * setting (changed to use mode 1, mode 0 is not used again),
 * 
 * Hydra_Software_Devel/8   2/4/04 3:01p syang
 * PR 9608: 1). added init for hMemDma / hPciDma of Channel, hDma of
 * MemDma / PciDma and hChannel of Queue; 2). split CallBackFunc type def
 * to mem and pci versions; 3). fixed typo in TranHandle def; 4).leave
 * critical section before calling BINT func; 5). fixed a bug with
 * nextDesc in Desc info setting; 6). use wake to start dma as in sleep
 * mode; 7). corrected typo in the asserts of Queue_CreateTran.
 * 
 * Hydra_Software_Devel/7   10/23/03 6:17p syang
 * to make linux comipler silent
 * 
 * Hydra_Software_Devel/6   10/23/03 4:20p syang
 * adjusted sub-module implementaion overview
 * 
 * Hydra_Software_Devel/5   10/23/03 3:15p syang
 * added real pci dma implementation
 * 
 * Hydra_Software_Devel/4   10/23/03 11:07a syang
 * first time checking after resolving comipling errors
 * 
 * Hydra_Software_Devel/3   10/15/03 3:04p syang
 * updated to use TranHandle, rather than TranId
 * 
 * Hydra_Software_Devel/2   10/10/03 4:10p syang
 * updated BDMA_Mem_Context
 * 
 * Hydra_Software_Devel/1   9/25/03 6:44p syang
 * init version, from scratch
 * 
 ***************************************************************************/
#ifndef BDMA_MEM_PRIV_H__
#define BDMA_MEM_PRIV_H__

#include "bdma_priv.h"
#include "bdma_queue.h"
#ifdef BDMA_P_SUPPORT_SHARF_DMA_ENGINE
#include "bdma_mem_sharf_priv.h"
#endif

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
 * Memory DMA sub-module code should never refer to the member of queue
 * module directly, however it needs to know the def of struct
 * BDMA_P_QueueEntry.
 *
 * By abstracting out the queue module, the readabilty, maintainabilty, and
 * extendabilty (such as to IO DMA) are much improved. Code size is also
 * improved.
 ****************************************************************************/

#define BDMA_P_MEM_GET_CONTEXT(handle, context) \
	BDMA_P_GENERIC_GET_CONTEXT((handle), (context), BDMA_P_Mem_Context)

#define BDMA_P_MEM_SET_BLACK_MAGIC(handle) \
	BDMA_P_GENERIC_SET_BLACK_MAGIC((handle), BDMA_P_Mem_Context)

#define BDMA_P_MEM_DESTROY_CONTEXT(struct_ptr) \
	BDMA_P_GENERIC_DESTROY_CONTEXT(struct_ptr, BDMA_P_Mem_Context)	

#if (BDMA_P_SUPPORT_MEM_DMA_41_KEY_SLOTS)
#define BDMA_P_MEM_KEY_SLOT_MAX               41
#else
#define BDMA_P_MEM_KEY_SLOT_MAX               15
#endif	

#if ((BCHP_CHIP == 35230) || (BCHP_CHIP == 35125) || (BCHP_CHIP == 35233))
#define BDMA_P_MEM_KEY_SIZE                   8
#else
#define BDMA_P_MEM_KEY_SIZE                   6
#endif

#define BDMA_P_MEM_MAX_BLOCK_SIZE             (0x1000000 -1)  /* 16 MBytes */	
	
/***************************************************************************
 * Memory Tran Handle
 */
typedef struct BDMA_P_Mem_TranContext
{
	BDMA_P_QueueEntry  QueueEntry;
	
} BDMA_P_Mem_TranContext;


/***************************************************************************
 * Memory Dma Context
 */
typedef struct BDMA_P_Mem_Context
{
	uint32_t   ulBlackMagic;   /* Black magic for handle validation */

	/* created from this handle */
	BDMA_Handle    hDma;

	/* engine id */
	BDMA_MemDmaEngine eEngine;

	/* register offset */
	uint32_t       ulRegOffset;

	/* configures */
	BDMA_Endian     eReadEndian;/* endian read mode */
	BDMA_SwapMode   eSwapMode;  /* swap mode */
	BDMA_CryptMode  eCryptMode;
	uint32_t        ulKeySlot;
	bool            bSgEnable;
	bool            bCryptoSetInEngine;
	bool            bCryptoSetInTran;
	
	/* tran entry queue */
	BDMA_P_QueueHandle  hQueue;
	
	/* interrupt call back handle */
	BINT_CallbackHandle  hCallBack;	

#ifdef BDMA_P_SUPPORT_SHARF_DMA_ENGINE
	BDMA_P_Mem_Sharf_Handle  hSharf;
#endif
	
} BDMA_P_Mem_Context;


/***************************************************************************
 *
 * Utility functions used by BDMA
 *
 ***************************************************************************/
/*--------------------------------------------------------------------------
 * To be called by BDMA API func before taking real action, to get dma mutex
 * WITH block, ideally for user mode
 */
BERR_Code BDMA_P_Mem_AcquireMutex(
	BDMA_Mem_Handle          hMemDma,
	BKNI_MutexHandle        *phMutex );


/***************************************************************************
 *
 * API support functions
 *
 ***************************************************************************/

/*--------------------------------------------------------------------------
 */
BERR_Code BDMA_P_Mem_GetMemDmaPtrFromTran_isr(
	BDMA_P_QueueEntry *    pTran,
	BDMA_P_Mem_Context **  ppMemDma );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Mem_Init(
	BDMA_P_Mem_Context  * pMemDma,
	const BDMA_Mem_Settings *pSettings,
	uint32_t              ulL2IntrBintId );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Mem_UnInit(
	BDMA_P_Mem_Context  * pMemDma );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Mem_Create(
	BDMA_Handle           hDma,
	BDMA_MemDmaEngine     eEngine,
	const BDMA_Mem_Settings *pSettings,
	BDMA_Mem_Handle *     phMemDma );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Mem_Destroy(
	BDMA_Mem_Handle          hMemDma );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Mem_SetSwapMode_isr(
	BDMA_Mem_Handle          hMemDma,
	BDMA_SwapMode            eSwapMode );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Mem_SetByteSwapMode_isr(
	BDMA_Mem_Handle          hMemDma,
	BDMA_Endian              eReadEndian,
	BDMA_SwapMode            eSwapMode );

BERR_Code BDMA_P_Mem_Tran_SetCrypto_isr(
	BDMA_Mem_Tran_Handle     hTran,
	BDMA_CryptMode           eCryptMode,
	uint32_t                 ulKeySlot,
	bool                     bSgEnable );

/***************************************************************************
 * obsolete
 */
BERR_Code BDMA_P_Mem_SetCrypto_isr(
	BDMA_Mem_Handle          hMemDma,
	BDMA_CryptMode           eCryptMode,	
	uint32_t                 ulKeySlot,
	bool                     bSgEnable );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Mem_Tran_Create_isr(
	BDMA_Mem_Handle          hMemDma,
	uint32_t                 ulNumBlocks,
	bool                     bCachedDesc,
	BDMA_Mem_Tran_Handle  *  phTran );

/***************************************************************************
 *
 */
BERR_Code BDMA_P_Mem_Tran_SetDmaBlockInfo_isr(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulDstBusAddr,
	uint32_t                 ulSrcBusAddr,
	uint32_t                 ulBlockSize,
	bool                     bCryptInit );

/***************************************************************************
 * To be called to mark / unmark the block as scatter-gather start /end point
 * validated against the current engine state.
 */
BERR_Code BDMA_P_Mem_Tran_SetSgStartEnd_isr(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	bool                     bSgStart,
	bool                     bSgEnd );

/***************************************************************************
 * To be called by both BDMA_P_Mem_Tran_Start and
 * BDMA_P_Mem_Tran_StartAndCallBack, BDMA_P_Mem_Tran_Start should pass NULL
 * for pCallBackFunc_isr and pUserCallBackParam
 */
BERR_Code BDMA_P_Mem_Tran_Start_isr(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulNumActBlocks,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam );

/***************************************************************************
 * To be called by both BDMA_Mem_Tran_StartDmaSubset and
 * BDMA_Mem_Tran_StartDmaSubsetAndCallBack. 
 */

BERR_Code BDMA_P_Mem_Tran_StartSubset_isr(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulFirstBlock,
	uint32_t                 ulNumActBlocks,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam );

/***************************************************************************
 */
BERR_Code BDMA_P_Mem_Transfer_isr(
	BDMA_Mem_Tran_Handle     hTran,
	uint32_t                 ulBlockId,
	uint32_t                 ulDstBusAddr,
	uint32_t                 ulSrcBusAddr,
	uint32_t                 ulBlockSize,
	bool                     bCryptInit,
	BDMA_Mem_CallbackFunc    pCallBackFunc_isr,
	void *                   pUserCallBackParam );

/***************************************************************************
 * To be called by BDMA_Mem_Tran_GetStatus. 
 */
BERR_Code BDMA_P_Mem_Tran_GetStatus_isr(
	BDMA_Mem_Tran_Handle     hTran,
	BDMA_TranStatus *        peTranStatus );

/***************************************************************************
 * To be called by BDMA_Mem_Tran_Destroy. It free the queue entries occupied
 * this Tran
 */
BERR_Code BDMA_P_Mem_Tran_Destroy_isr(
	BDMA_Mem_Tran_Handle     hTran );


/***************************************************************************
 * To be called by BDMA_Mem_Tran_Reset. It resets the Tran into its 
 * initially created state
 */
BERR_Code BDMA_P_Mem_Tran_Reset_isr(
	BDMA_Mem_Tran_Handle     hTran );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BDMA_MEM_PRIV_H__ */

/* end of file */
