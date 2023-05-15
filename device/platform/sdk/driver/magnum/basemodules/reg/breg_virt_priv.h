/***************************************************************************
 *	   Copyright (c) 2003, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_virt_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 5/21/04 8:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_virt_priv.h $
 * 
 * Hydra_Software_Devel/1   5/21/04 8:54p vsilyaev
 * PR 11175: Merged into the integration branch
 * 
 * Hydra_Software_Devel/BCM7411/2   5/17/04 6:55p vsilyaev
 * Fixed read/write names
 * 
 * Hydra_Software_Devel/BCM7411/1   5/14/04 7:14p vsilyaev
 * just test
 * 
 * 
 ***************************************************************************/
#ifndef BREG_VIRT_PRIV_H
#define BREG_VIRT_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BREG_Virt_Impl
{
	void * context;
	void (*BREG_Write32)(void * context, uint32_t addr, uint32_t reg);
	void (*BREG_Write16)(void * context, uint32_t addr, uint16_t reg);
	void (*BREG_Write8)(void * context, uint32_t addr, uint8_t reg);
	uint32_t (*BREG_Read32)(void * context, uint32_t addr);
	uint16_t (*BREG_Read16)(void * context, uint32_t addr);
	uint8_t (*BREG_Read8)(void * context, uint32_t addr);
}BREG_Virt_Impl;

#ifdef __cplusplus
}
#endif
 
#endif
/* End of File */

