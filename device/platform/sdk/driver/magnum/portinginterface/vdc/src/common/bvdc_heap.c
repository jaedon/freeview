/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_heap.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 1/24/07 9:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7440/bvdc_heap.c $
 * 
 * Hydra_Software_Devel/2   1/24/07 9:07p albertl
 * PR22237:  Updated BMEM calls to use new BMEM_Heap functions.
 * 
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 3:52p jessem
 * PR 20768: Initial version.
 * 
 *
 ***************************************************************************/
#include "bstd.h"                /* standard types */
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */

#include "bmem.h"
#include "bvdc.h"                /* Video display */
#include "bvdc_bufferheap_priv.h"

BDBG_MODULE(BVDC_HEAP);

/***************************************************************************
 *
 */
BERR_Code BVDC_Heap_Create
	( BVDC_Handle                       hVdc,
	  BVDC_Heap_Handle                 *phHeap,
	  BMEM_Heap_Handle                  hMem,
	  const BVDC_Heap_Settings         *pSettings )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Heap_Create);
	BDBG_ASSERT(hVdc);
	BDBG_ASSERT(hMem);
	BDBG_ASSERT(pSettings);

	err = BVDC_P_BufferHeap_Create(hVdc, phHeap, hMem, pSettings);
	if (err != BERR_SUCCESS)
		return err;

	BDBG_LEAVE(BVDC_Heap_Create);
	return BERR_SUCCESS;
	
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Heap_Destroy
	( BVDC_Heap_Handle                 hHeap)
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Heap_Destroy);
	BDBG_ASSERT(hHeap);

	err = BVDC_P_BufferHeap_Destroy(hHeap);
	if (err != BERR_SUCCESS)
		return err;

	BDBG_LEAVE(BVDC_Heap_Destroy);
	return BERR_SUCCESS;	
}

