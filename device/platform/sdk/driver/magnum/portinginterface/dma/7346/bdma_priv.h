/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdma_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 4/4/11 11:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/dma/7400/bdma_priv.h $
 * 
 * Hydra_Software_Devel/20   4/4/11 11:45p vanessah
 * SWDTV-6186: 35233 DMA support
 * 
 * Hydra_Software_Devel/19   2/2/11 10:29a vanessah
 * SW7550-670: add Sharf DMA support
 * 
 * Hydra_Software_Devel/18   12/22/10 3:01p vanessah
 * SWBLURAY-23683: Add DMA PortingInterface support for Blast (7640) chip
 * 
 * Hydra_Software_Devel/18   12/21/10 6:40p vanessah
 * SWBLURAY-23683: Add DMA PortingInterface support for Blast (7640) chip
 * 
 * Hydra_Software_Devel/17   12/1/10 12:08p cdisc
 * SW35230-2451: adding Sharf support for 35230 C0
 * 
 * Hydra_Software_Devel/16   3/9/10 12:05p syang
 * SW7630-72:  update to 41 key slots for 7601, 7635 and 7630
 * 
 * Hydra_Software_Devel/15   9/15/09 8:15a tdo
 * SW7630-13: Fix compiling error
 *
 * Hydra_Software_Devel/14   9/14/09 5:01p tdo
 * SW7630-13: Bringup of portinginterface "dma" for Grain (7630)
 *
 * Hydra_Software_Devel/13   4/8/09 4:23p vsilyaev
 * PR 54015: Don't include bkni_multi.h into the bdbg.h. All thread-aware
 * modules should include explicitly bkni_multi.h
 *
 * Hydra_Software_Devel/12   4/7/09 4:09p syang
 * PR 52973: add 7635 support
 *
 * Hydra_Software_Devel/11   1/23/09 11:24a syang
 * PR 43729:  fix compile for 7405
 *
 * Hydra_Software_Devel/10   1/23/09 10:33a syang
 * PR 43729: change 7440 sharf num to 2, per abbas' request
 *
 * Hydra_Software_Devel/9   11/14/08 2:55p syang
 * PR 43729:  correct number of sharf engines for 7601
 *
 * Hydra_Software_Devel/8   10/3/08 5:29p syang
 * PR 34606: support 41 key slots for 7405 B0
 *
 * Hydra_Software_Devel/7   8/20/08 4:10p syang
 * PR 45212: clean up #define
 *
 * Hydra_Software_Devel/6   8/20/08 2:11p syang
 * PR 45212:  turn off AUTO_APPEND usage (it seems caused wrong data
 * during encryption with dyn link), and now 3 recording streams with
 * crypto/time-shift still works fine.
 *
 * Hydra_Software_Devel/5   8/20/08 1:27p syang
 * PR 43729:  add sharf for 7601
 *
 * Hydra_Software_Devel/4   7/31/08 10:51a syang
 * PR 34606:  use curr_desc again (reg reading validation added)
 *
 * Hydra_Software_Devel/3   7/23/08 6:27p tdo
 * PR33689: Add support for 7601
 *
 * Hydra_Software_Devel/2   7/11/08 3:46p syang
 * PR 43729:  rm code for old crypto hw arch
 *
 * Hydra_Software_Devel/30   7/11/08 11:48a syang
 * PR 43729:  make bdma.h chip independent
 *
 * Hydra_Software_Devel/27   5/28/08 5:20p syang
 * PR 34606: handle the dyn link failure when AUTO_PEND is suppoted and
 * LAST bit in desc is not cleared
 *
 * Hydra_Software_Devel/26   5/19/08 1:58p syang
 * PR 34606:  clean up
 *
 * Hydra_Software_Devel/25   5/19/08 11:48a syang
 * PR 34606:  add validation for HW reg reading
 *
 * Hydra_Software_Devel/24   2/22/08 9:51a rpan
 * PR39458: Initial work for bcm3556.
 *
 * Hydra_Software_Devel/23   2/21/08 1:38p rpan
 * PR34854: Initial work for bcm3548.
 *
 * Hydra_Software_Devel/22   12/12/07 3:42p syang
 * PR 31923: added auto-append support to PI
 *
 * Hydra_Software_Devel/21   12/7/07 4:10p syang
 * PR 34606: rewrite more than half of code to support hw sharing for
 * kernel and user-mode sides
 *
 * Hydra_Software_Devel/19   10/15/07 11:44a yuxiaz
 * PR35323: Added DMA support for 7325.
 *
 * Hydra_Software_Devel/18   5/8/07 3:40p yuxiaz
 * PR29728: Basic DMA support for 7405-A0
 *
 * Hydra_Software_Devel/17   3/21/07 12:47p syang
 * PR 28173: dyn link works now after fixing next_desc shift and status
 * proc  tran loop break. fixed cur hw desc left shift
 *
 * Hydra_Software_Devel/16   3/19/07 4:17p syang
 * PR 28171: prepare to add sharf support
 *
 * Hydra_Software_Devel/15   3/8/07 2:09p tdo
 * PR 26224:  Move chip secific and prive macro defines out of bdma.h back
 * to bdma_priv.h
 *
 * Hydra_Software_Devel/14   3/7/07 4:27p tdo
 * PR 26224:  Add support for 2nd MEM DMA for 7400B0
 *
 * Hydra_Software_Devel/13   3/7/07 3:08p yuxiaz
 * PR 28167: Added DMA support for MEMC_2 on 7400.
 *
 * Hydra_Software_Devel/12   2/5/07 4:43p syang
 * PR 23354: allow mem addr in mem_1; cleanup cip specific code section
 * macro usage
 *
 * Hydra_Software_Devel/11   1/8/07 12:53p tdo
 * PR 26224:  Add second DMA MEM engine support for 7400 B0
 *
 * Hydra_Software_Devel/10   10/3/06 4:02p syang
 * PR 24634: replace critical section with private mutex for
 * mem/pci_dma_handle create/destroy.
 *
 * Hydra_Software_Devel/9   9/22/04 3:44p syang
 * PR12728: Fixed -pdantic build warnings.
 *
 * Hydra_Software_Devel/8   11/20/03 5:46p syang
 * set ptr to NULL in BDMA_P_GENERIC_DESTROY_CONTEXT
 *
 * Hydra_Software_Devel/7   11/19/03 6:39p syang
 * take out struct ptr cast from get_context
 *
 * Hydra_Software_Devel/6   10/23/03 3:16p syang
 * added real pci dma implementation
 *
 * Hydra_Software_Devel/5   10/23/03 11:07a syang
 * first time checking after resolving comipling errors
 *
 * Hydra_Software_Devel/4   10/21/03 4:46p syang
 * added GetMemDmaHandle and GetPciDmaHandle
 *
 * Hydra_Software_Devel/3   10/20/03 12:19p syang
 * change function name SetMem/PciHandle to SetMem/PciDmaHandle
 *
 * Hydra_Software_Devel/2   10/17/03 4:16p syang
 * generic_get_context are moved in from tool.h, engine id enum is added
 *
 * Hydra_Software_Devel/1   10/7/03 3:53p syang
 * init version
 *
 * Hydra_Software_Devel/1   9/25/03 6:44p syang
 * init version, from scratch
 *
 ***************************************************************************/
#ifndef BDMA_PRIV_H__
#define BDMA_PRIV_H__

#include "bdma.h"
#include "bdma_errors.h"
#include "bkni_multi.h"

#ifdef __cplusplus
extern "C" {
#endif


#if (BCHP_CHIP == 7400)
#if (BCHP_VER>=BCHP_VER_B0)
#define BDMA_P_MEM_2_ADDR_BASE                (0x70000000)
#define BDMA_P_MEM_2_ADDR_SIZE                (64*1024*1024)
#endif
#define BDMA_P_MEM_1_ADDR_BASE                (0x60000000)
#define BDMA_P_MEM_1_ADDR_SIZE                (64*1024*1024)
#elif (BCHP_CHIP==7405) || (BCHP_CHIP == 7325)
#define BDMA_P_MEM_1_ADDR_BASE                (0x60000000)
#define BDMA_P_MEM_1_ADDR_SIZE                (64*1024*1024)
#elif (BCHP_CHIP==3563)
#define BDMA_P_MEM_1_ADDR_BASE                (0x60000000)
#define BDMA_P_MEM_1_ADDR_SIZE                (32*1024*1024)
#endif

#if (BCHP_CHIP==7400) || (BCHP_CHIP==7401)
#define BDMA_P_MEM_ADDR_SIZE                  (7<<28)
#else
#define BDMA_P_MEM_ADDR_SIZE                  (1<<28)
#endif

/* number of mem ctrl */
#if (BCHP_CHIP==7400) && (BCHP_VER>=BCHP_VER_B0)
#define BDMA_P_SUPPORT_MEM_CTRL                3
#elif (BCHP_CHIP==7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335)
#define BDMA_P_SUPPORT_MEM_CTRL                2
#elif (BCHP_CHIP==3563)
#define BDMA_P_SUPPORT_MEM_CTRL                2
#else
#define BDMA_P_SUPPORT_MEM_CTRL                1
#endif

#if ((BCHP_CHIP==7440) && (BCHP_VER>=BCHP_VER_B0)) || \
    (BCHP_CHIP == 7601) || (BCHP_CHIP == 7635) || (BCHP_CHIP == 7630) || (BCHP_CHIP == 7640) \
    || ((BCHP_CHIP == 35230) && (BCHP_VER==BCHP_VER_C0)) \
    || ((BCHP_CHIP ==  7550) && (BCHP_VER>=BCHP_VER_B0)) \
    || (BCHP_CHIP ==  35233) 
#define BDMA_P_SUPPORT_SHARF_DMA_ENGINE        2
#endif

/* number of mem dma HW engines */
#if (BCHP_CHIP==7400) && (BCHP_VER>=BCHP_VER_B0)
#define BDMA_P_SUPPORT_MEM_DMA_ENGINE          2
#elif(BCHP_CHIP==7550) 
#define BDMA_P_SUPPORT_MEM_DMA_ENGINE          0
#else
#define BDMA_P_SUPPORT_MEM_DMA_ENGINE          1
#endif

/* number of pci dma HW engines */
#if (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
#define BDMA_P_SUPPORT_PCI_DMA_ENGINE          0
#else
#define BDMA_P_SUPPORT_PCI_DMA_ENGINE          1
#endif

/* register name support mutiple engine or not */
#if ((BCHP_CHIP==3548) || (BCHP_CHIP==3556) || \
     (BCHP_CHIP==3563) || (BCHP_CHIP==7118) || (BCHP_CHIP==7401) || (BCHP_CHIP==7403) || \
     (BCHP_CHIP==7440) || ((BCHP_CHIP==7400) && (BCHP_VER<BCHP_VER_B0)) || \
     (BCHP_CHIP==7601) || (BCHP_CHIP==7635)) || (BCHP_CHIP==7630)
#define BDMA_P_SUPPORT_MEM_DMA_REG_NAME_i      0
#else
#define BDMA_P_SUPPORT_MEM_DMA_REG_NAME_i      1
#endif

/* Scatter-Gather */
#if ((BCHP_CHIP==3563) || (BCHP_CHIP==7118) || (BCHP_CHIP==7401) || (BCHP_CHIP==7403) || \
     ((BCHP_CHIP==7400) && (BCHP_VER<BCHP_VER_B0)) || \
     ((BCHP_CHIP==7440) && (BCHP_VER<BCHP_VER_B0)))
#define BDMA_P_SUPPORT_MEM_DMA_SCATTER_GATHER  0
#else
#define BDMA_P_SUPPORT_MEM_DMA_SCATTER_GATHER  1
#endif

/* auto-pend so that scatter-gather link list can be appended with non-scatter-gather linked list */
#if ((BCHP_CHIP==3563) || (BCHP_CHIP==7118) || (BCHP_CHIP==7401) || (BCHP_CHIP==7403) || \
     ((BCHP_CHIP==7400) && (BCHP_VER<BCHP_VER_D0)) || (BCHP_CHIP==7440) || \
     ((BCHP_CHIP==7405) && (BCHP_VER<BCHP_VER_B0)))
#define BDMA_P_SUPPORT_MEM_DMA_AUTO_APPEND     0
#else
/* XXX ??? AUTO_APPEND currently cause bad data with dyn link */
#define BDMA_P_SUPPORT_MEM_DMA_AUTO_APPEND     0
#endif

/* 41 key slots */
#if ((BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==3560) || \
	 (BCHP_CHIP==3563) || (BCHP_CHIP==7038) || \
	 (BCHP_CHIP==7118) || (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || \
	 (BCHP_CHIP==7400) || (BCHP_CHIP==7401) || (BCHP_CHIP==7403) || \
	 (BCHP_CHIP==7438) || (BCHP_CHIP==7440) || \
	 ((BCHP_CHIP==7405) && (BCHP_VER<BCHP_VER_B0)))
#define BDMA_P_SUPPORT_MEM_DMA_41_KEY_SLOTS    0
#else
#define BDMA_P_SUPPORT_MEM_DMA_41_KEY_SLOTS    1
#endif

#define BDMA_P_WATER_MARK_SG_OPEN              (0x2A)
#define BDMA_P_WATER_MARK_SG_CLOSE             (0x33)
#define BDMA_P_WATER_MARK_MASK                 (0x3F)


/* code configure */
#define BDMA_P_USE_CURR_DESC_ADDR	           1

#define BDMA_P_CHECK_BASIC                     0
#define BDMA_P_CHECK_NEXT_DESC                 0
#define BDMA_P_CHECK_SCATTER_GATHER            0
#define BDMA_P_CHECK_CORRUPT                   0

#define BDMA_P_SHOW_DESC                       0
#define BDMA_P_SHOW_SG_OPEN                    0
#define BDMA_P_SHOW_DYN_LINK_FAIL              0


/***************************************************************************
 *
 *  Generic utility macro
 *
 ****************************************************************************/

/* This macro take the check for a validity of a handle, and
 * cast to context pointer.
 */
#define BDMA_P_GENERIC_GET_CONTEXT(handle, context, structname) \
{ \
	if(!(handle) || \
	   (((structname*)(handle))->ulBlackMagic != \
	    (sizeof(structname) | 0xbac98700))) \
	{ \
		BDBG_ERR(("NULL context handle\n")); \
		(context) = NULL; \
	} \
	else \
	{ \
		(context) = (handle); \
	} \
}

/* This macro set the black magic for later handle validation
 */
#define BDMA_P_GENERIC_SET_BLACK_MAGIC(handle, structname) \
{ \
	((structname*)(handle))->ulBlackMagic = sizeof(structname) | 0xbac98700; \
}

/* This macro ends the function if error "result" is seen
 */
#define BDMA_P_END_IF_ERR(result, done_lable) \
	if ( BERR_SUCCESS != (result)) \
	{\
		goto done_lable;  \
	}

/* This macro shred the garbage
 */
#if BDBG_DEBUG_BUILD
#define BDMA_P_GENERIC_DESTROY_CONTEXT(struct_ptr, structname) \
{ \
	BKNI_Memset((void*)struct_ptr, 0xA3, sizeof(structname)); \
	BKNI_Free((void*)struct_ptr); \
	struct_ptr = NULL; \
}
#else
#define BDMA_P_GENERIC_DESTROY_CONTEXT(struct_ptr, structname) \
{ \
	BKNI_Free((void*)struct_ptr); \
}
#endif

/***************************************************************************
 *
 *  DMA main context utility macro
 *
 ****************************************************************************/

#define BDMA_P_MAIN_GET_CONTEXT(handle, context) \
	BDMA_P_GENERIC_GET_CONTEXT(handle, context, BDMA_P_Context)

#define BDMA_P_MAIN_SET_BLACK_MAGIC(handle) \
	BDMA_P_GENERIC_SET_BLACK_MAGIC(handle, BDMA_P_Context)

#define BDMA_P_MAIN_DESTROY_CONTEXT(struct_ptr) \
	BDMA_P_GENERIC_DESTROY_CONTEXT(struct_ptr, BDMA_P_Context)


/***************************************************************************
 *
 *  Main context struct and utility functions
 *
 ****************************************************************************/

/***************************************************************************
 * BDMA module Context
 */
typedef struct BDMA_P_Context
{
	uint32_t   ulBlackMagic;   /* Black magic for handle validation */

	/* handed down from up level sw */
	BCHP_Handle    hChip;
	BREG_Handle    hRegister;
	BMEM_Handle    hMemory;
	BINT_Handle    hInterrupt;

	/* protect dma API calls from diff threads */
	BKNI_MutexHandle hMutex;

#if BDMA_P_SUPPORT_MEM_DMA_ENGINE
	BDMA_Mem_Handle hMemDma[BDMA_P_SUPPORT_MEM_DMA_ENGINE];
#endif

#if BDMA_P_SUPPORT_PCI_DMA_ENGINE
	BDMA_Pci_Handle hPciDma[BDMA_P_SUPPORT_PCI_DMA_ENGINE];
#endif

#ifdef BDMA_P_SUPPORT_SHARF_DMA_ENGINE
	BDMA_Mem_Handle hSharfDma[BDMA_P_SUPPORT_SHARF_DMA_ENGINE];
#endif

} BDMA_P_Context;

/***************************************************************************
 * BDMA private function to get diff handles passed in as module open
 * no paramter validation
 */
BCHP_Handle BDMA_P_GetChipHandle( BDMA_Handle  hDma );
BREG_Handle BDMA_P_GetRegisterHandle( BDMA_Handle  hDma );
BMEM_Handle BDMA_P_GetMemoryHandle( BDMA_Handle  hDma );
BINT_Handle BDMA_P_GetInterruptHandle( BDMA_Handle  hDma );

/***************************************************************************
 * BDMA private function to set sub-module handle
 * no paramter validation
 * Note that BDMA_P_GetMemDmaHandle and BDMA_P_SetMemDmaHandle are for
 * backward compatible.  For 7400 B0 chip, these 2 functions will set or
 * return the handle of the last MEM engine created.
 */
#if BDMA_P_SUPPORT_MEM_DMA_ENGINE
BDMA_Mem_Handle BDMA_P_GetMemDmaHandle(	BDMA_Handle hDma, BDMA_MemDmaEngine eEngine );
void BDMA_P_SetMemDmaHandle( BDMA_Handle hDma, BDMA_MemDmaEngine eEngine, BDMA_Mem_Handle hMemDma );
#endif

#if BDMA_P_SUPPORT_PCI_DMA_ENGINE
BDMA_Pci_Handle BDMA_P_GetPciDmaHandle(	BDMA_Handle hDma, BDMA_PciDmaEngine eEngine );
void BDMA_P_SetPciDmaHandle( BDMA_Handle hDma, BDMA_PciDmaEngine eEngine, BDMA_Pci_Handle hPciDma );
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BDMA_PRIV_H__ */

/* end of file */
