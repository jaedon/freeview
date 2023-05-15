/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdma_errors.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 7/11/08 11:49a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/dma/7400/bdma_errors.h $
 * 
 * Hydra_Software_Devel/4   7/11/08 11:49a syang
 * PR 43729:  make bdma.h chip independent
 * 
 * Hydra_Software_Devel/3   12/7/07 6:39p syang
 * PR 34606: move SetCrypto to Tran level from Engine level for better
 * sharing by threads of the same progress
 * 
 * Hydra_Software_Devel/2   3/19/07 4:18p syang
 * PR 28171: prepare to add sharf support
 * 
 * Hydra_Software_Devel/1   10/23/03 11:02a syang
 * init version
 * 
 ***************************************************************************/
#ifndef BDMA_ERRORS_H__
#define BDMA_ERRORS_H__

#include "berr_ids.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
	DMA specific error code.

Description:
	

See Also:
	BDMA_Mem_Tran_Create, BDMA_Mem_Tran_SetBlockInfo
	BDMA_Mem_Tran_Start
	BDMA_Mem_Tran_Reset, BDMA_Mem_Tran_Destroy
	BDMA_Pci_Tran_Create, BDMA_Pci_Tran_SetBlockInfo
	BDMA_Pci_Tran_Start
	BDMA_Pci_Tran_Reset, BDMA_Pci_Tran_Destroy
***************************************************************************/
#define BDMA_ERR_TRAN_IN_PROGRESS        BERR_MAKE_CODE(BERR_DMA_ID, 0x0101)
#define BDMA_ERR_ENGINE_BUSY             BERR_MAKE_CODE(BERR_DMA_ID, 0x0102)
#define BDMA_ERR_BLOCK_INFO_UNSET        BERR_MAKE_CODE(BERR_DMA_ID, 0x0103)
#define BDMA_ERR_OUT_OF_QUEUE            BERR_MAKE_CODE(BERR_DMA_ID, 0x0104)
#define BDMA_ERR_ENGINE_OCCUPIED         BERR_MAKE_CODE(BERR_DMA_ID, 0x0105)	
#define BDMA_ERR_NOT_WORD_ALIGNED        BERR_MAKE_CODE(BERR_DMA_ID, 0x0106)
#define BDMA_ERR_NOT_CRYPT_ALIGNED       BERR_MAKE_CODE(BERR_DMA_ID, 0x0107)
#define BDMA_ERR_SIZE_OUT_RANGE          BERR_MAKE_CODE(BERR_DMA_ID, 0x0108)
#define BDMA_ERR_OVERLAP                 BERR_MAKE_CODE(BERR_DMA_ID, 0x0109)
#define BDMA_ERR_SG_MISMATCH             BERR_MAKE_CODE(BERR_DMA_ID, 0x010a)
#define BDMA_ERR_OBSOLETE_API_MIX        BERR_MAKE_CODE(BERR_DMA_ID, 0x010b)		
#define BDMA_ERR_ENGINE_NOT_SUPPORTED    BERR_MAKE_CODE(BERR_DMA_ID, 0x010c)
#define BDMA_ERR_FEATURE_NOT_SUPPORTED   BERR_MAKE_CODE(BERR_DMA_ID, 0x010d)
	
#ifdef __cplusplus
}
#endif

#endif /* #ifndef BDMA_ERRORS_H__ */
/* End of file. */
