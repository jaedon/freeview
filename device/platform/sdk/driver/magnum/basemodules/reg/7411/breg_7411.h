/***************************************************************************
 *	   Copyright (c) 2003, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_7411.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 1/11/05 6:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/7411/breg_7411.h $
 * 
 * Hydra_Software_Devel/3   1/11/05 6:31p vsilyaev
 * PR 13803: Changed code to support EBI protocol
 * 
 * Hydra_Software_Devel/2   9/14/04 3:28p darnstein
 * PR 10943: add some #include's.
 * 
 * Hydra_Software_Devel/1   5/21/04 8:54p vsilyaev
 * PR 11175: Merged into the integration branch
 * 
 * Hydra_Software_Devel/BCM7411/2   5/18/04 5:50p vsilyaev
 * Fixed compiler warning
 * 
 * Hydra_Software_Devel/BCM7411/1   5/17/04 6:56p vsilyaev
 * 7411 function based register interface
 * 
 ***************************************************************************/
#ifndef BREG_7411_H
#define  BREG_7411_H

#include "berr.h"
#include "breg_virt.h"
#include "breg_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

BERR_Code BREG_7411_Open( BREG_Virt_Handle *pRegHandle, BREG_Handle base);
void BREG_7411_Close(BREG_Virt_Handle regHandle);

#ifdef __cplusplus
}
#endif

#endif /* BREG_7411_H */

