/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdgp_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/21/07 2:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/dgp/7405/bdgp_priv.h $
 * 
 * Hydra_Software_Devel/1   6/21/07 2:00p nissen
 * PR 29766: Initial version of DGP module.
 * 
 ***************************************************************************/
#ifndef BDGP_PRIV_H__
#define BDGP_PRIV_H__

#include "bdgp.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
typedef struct BDGP_P_Context
{
	BCHP_Handle hChip;
	BREG_Handle hRegister;
	BINT_Handle hInterrupt;

	BINT_CallbackHandle hTriggerCallback;
	BDGP_Trigger eCallbackTrigger;
	BDGP_CallbackFunc_isr pfCallback;
	void *pvCallbackParm1;
	int iCallbackParm2;

	bool bDeviceEnabled;
	bool bAlphaPremultiply;
	bool bErrorSelectPixelValue;
	uint32_t uiErrorPixelValue;
	uint32_t uiLineCompareMask;
	uint32_t uiLineComparePosition;

	BSUR_Surface_Handle hSurface;
	BPXL_Format eFormat;
	void *pvMemory;
	uint32_t uiPitch;
	uint32_t uiWidth;
	uint32_t uiHeight;
	uint32_t uiOffset;
	uint32_t uiBpp;
}
BDGP_P_Context;

/***************************************************************************/
void BDGP_P_Trigger_isr(
	void *pvParam1,
	int iParam2 );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BDGP_PRIV_H__ */

/* End of file. */
