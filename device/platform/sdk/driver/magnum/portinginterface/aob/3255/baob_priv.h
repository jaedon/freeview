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
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 8/25/09 3:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/aob/3255/baob_priv.h $
 * 
 * Hydra_Software_Devel/5   8/25/09 3:33p haisongw
 * SW7400-2478:Get OOB lock status from RPC_notification directly
 *
 * Hydra_Software_Devel/4   10/31/06 3:34p haisongw
 * PR25385: do always callback when 3255 notifies host
 *
 * Hydra_Software_Devel/3   9/1/06 5:46p haisongw
 * PR24071: add spectral_inversion control API for OOB DS
 *
 * Hydra_Software_Devel/2   7/31/06 7:07p haisongw
 * PR23108: Rmagnum sometime can't send notification to 74XX
 *
 * Hydra_Software_Devel/1   7/10/06 4:27p haisongw
 * PR 22368: Add AOB, AUS and TNR3255ob support
 *
 ***************************************************************************/

#ifndef BAOB_PRIV_H__
#define BAOB_PRIV_H__

#include "bchp.h"
#include "brpc.h"
#include "brpc_3255.h"
#include "baob.h"
#include "bkni_multi.h"

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
	BRPC_DevId devId;
	BRPC_Handle hRpc;
	BAOB_CallbackFunc pCallback[BAOB_Callback_eLast];
	void *pCallbackParam[BAOB_Callback_eLast];
	bool enableFEC;						/* enable OOB FEC*/
	BAOB_SpectrumMode spectrum; 		/* current specturm setting*/
	bool isLock;						/* current lock status */
	BKNI_MutexHandle mutex;				/* mutex to protect lock status*/
} BAOB_P_Handle;


#ifdef __cplusplus
}
#endif

#endif

