/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_util_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 12/3/08 7:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7400/bvbi_util_priv.h $
 * 
 * Hydra_Software_Devel/1   12/3/08 7:49p darnstein
 * PR45819: Source files for 7400 are no longer symbolic links.
 * 
 * Hydra_Software_Devel/1   9/7/07 5:07p darnstein
 * PR25708: Miscellaneous support routines, not for public use.
 * 
 ***************************************************************************/

#ifndef BVBI_UTIL_PRIV_H__
#define BVBI_UTIL_PRIV_H__

#include "bstd.h"
#include "bmem.h"
#include "berr.h"

/* 
 * A software object for accumulating sections of data into a line. Provides
 * double-buffering.
 */

typedef struct BVBI_LineBuilder_Handle *BVBI_LineBuilder_Handle;

BERR_Code BVBI_LineBuilder_Open ( 
	BVBI_LineBuilder_Handle* pHandle, 
	BMEM_Handle hMem, size_t lineCount, size_t lineSize);

void BVBI_LineBuilder_Close (BVBI_LineBuilder_Handle handle);

BERR_Code BVBI_LineBuilder_Put (
	BVBI_LineBuilder_Handle handle, uint8_t* sectionData, size_t sectionSize, 
	size_t sectionOffset, int sequenceNumber, int lineNumber);

BERR_Code BVBI_LineBuilder_Get (
	BVBI_LineBuilder_Handle handle, uint8_t** pLineData, int* pSequenceNumber, 
	int* pLineNumber);

#endif /* BVBI_UTIL_PRIV_H__ */
