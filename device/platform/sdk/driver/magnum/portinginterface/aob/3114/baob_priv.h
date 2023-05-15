/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: baob_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 9/14/09 2:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/aob/3117/baob_priv.h $
 * 
 * Hydra_Software_Devel/4   9/14/09 2:28p vishk
 * SW7405-3008: Need to be able to configure the BCM3117 OB_AGC_CTL and
 * DS_AGC_CTL pins to totem-pole (CMOS).
 * 
 * Hydra_Software_Devel/3   6/26/09 1:36p vishk
 * PR 56235: Add support to set IfFreq in AOB PI for 3117
 * PR 56288: Add support to set OOB AGC pin mode for 3117
 * 
 * Hydra_Software_Devel/2   1/16/09 7:37p vishk
 * PR46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/1   12/16/08 11:27p vishk
 * PR46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
  ***************************************************************************/

#ifndef BAOB_PRIV_H__
#define BAOB_PRIV_H__

#include "bchp.h"
#include "bhab.h"
#include "baob.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	The handle for Out-of-Band Downstream module.

Description:

See Also:
	BAOB_Open()

****************************************************************************/

typedef struct BAOB_P_Handle
{
	uint32_t magicId;					/* Used to check if structure is corrupt */
	BCHP_Handle hChip;
	BREG_Handle hRegister;
	BINT_Handle hInterrupt;
	BHAB_DevId devId;
	BHAB_Handle hHab;
	BAOB_CallbackFunc pCallback[BAOB_Callback_eLast];
	void *pCallbackParam[BAOB_Callback_eLast];
	bool enableFEC;						/* enable OOB FEC*/
	BAOB_SpectrumMode spectrum; 		/* current specturm setting*/
	BKNI_MutexHandle mutex;				/* mutex to protect lock status*/
	bool isLock;						/* current lock status */
	unsigned long ifFreq;				/* IF frequency used. */
} BAOB_P_Handle;


#ifdef __cplusplus
}
#endif

#endif

