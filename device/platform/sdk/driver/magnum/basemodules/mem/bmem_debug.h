/***************************************************************************
 *     Copyright (c) 2001-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmem_debug.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 2/24/10 4:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/mem/bmem_debug.h $
 * 
 * Hydra_Software_Devel/7   2/24/10 4:14p erickson
 * SW3548-2721: don't print uninitialized file and line in
 * BMEM_Dbg_DumpHeap. make BMEM_Dbg_DumpBlock private because its params
 * are internal.
 *
 * Hydra_Software_Devel/6   1/21/05 4:11p albertl
 * PR13717:  Removed BMEM_DBG_ENTER defines.
 *
 * Hydra_Software_Devel/5   12/31/03 12:02p jasonh
 * PR 8940: Changed return type of destroyheap to void. Removed use of
 * DEBUG macro and printf.
 *
 * Hydra_Software_Devel/4   10/2/03 11:52a hongtaoz
 * Removed "//" comment lines for ANSI C compliance.
 *
 * Hydra_Software_Devel/3   9/2/03 1:35p vadim
 * Some magnum updates.
 *
 * Hydra_Software_Devel/2   3/20/03 3:51p erickson
 * renamed all MEM_ to BMEM_
 *
 * Hydra_Software_Devel/1   3/20/03 3:24p erickson
 * initial bmem work, taken from SetTop/memorylib
 *
 * SanJose_DVTSW_Devel\4   4/18/02 11:28a poz
 * Use BRCM_DBG_OUT for output.
 *
 * SanJose_DVTSW_Devel\3   4/15/02 6:3p poz
 * Fixed multiple typedef declaration.
 *
 * SanJose_DVTSW_Devel\2   4/15/02 1:35p poz
 * Removed unused macros.
 * Removed extra include.
 * Updated copyright and keywords in header.
 *
 ***************************************************************************/
#ifndef BMEM_DEBUG_H__
#define BMEM_DEBUG_H__

#include "bmem.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * These functions print out various info from the heap for debugging
 * purposes.
 */
void BMEM_Dbg_Map(BMEM_Handle pheap);
void BMEM_Dbg_DumpHeap(BMEM_Handle pheap);

void BMEM_Dbg_ClearErrorCount(BMEM_Handle pheap);
uint32_t BMEM_Dbg_GetErrorCount(BMEM_Handle pheap);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* #ifndef BMEM_DEBUG_H__ */

/* End of File */

