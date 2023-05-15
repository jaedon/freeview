/********************************************************************************************
*     (c)2004-2010 Broadcom Corporation                                                     *
*                                                                                           *
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,   *
*  and may only be used, duplicated, modified or distributed pursuant to the terms and      *
*  conditions of a separate, written license agreement executed between you and Broadcom    *
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants*
*  no license (express or implied), right to use, or waiver of any kind with respect to the *
*  Software, and Broadcom expressly reserves all rights in and to the Software and all      *
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU       *
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY                    *
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.                                 *
*
*  Except as expressly set forth in the Authorized License,                                 *
*
*  1.     This program, including its structure, sequence and organization, constitutes     *
*  the valuable trade secrets of Broadcom, and you shall use all reasonable efforts to      *
*  protect the confidentiality thereof,and to use this information only in connection       *
*  with your use of Broadcom integrated circuit products.                                   *
*                                                                                           *
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"          *
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR                   *
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO            *
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES            *
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,            *
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION             *
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF              *
*  USE OR PERFORMANCE OF THE SOFTWARE.                                                      *
*                                                                                           *
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS         *
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR             *
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR               *
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF             *
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT              *
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE            *
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF              *
*  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: b_dtcp_exch_key.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 12/1/10 4:35p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/lib/dtcp_ip/src/b_dtcp_exch_key.c $
 * 
 * 7   12/1/10 4:35p leisun
 * SW7408-187: 7408 BE mode fix
 * 
 * 6   12/1/10 1:26p leisun
 * SW7408-187: DTCP-IP for BE platform fix
 * 
 * 5   5/19/10 11:24a leisun
 * SW7420-561: Fix for sumsung reported issue
 * 
 * 4   3/23/10 6:57p leisun
 * SW7420-561: Fixes related to DMS with PS3 client
 * 
 * 3   7/13/09 11:19a leisun
 * PR 55693: Re-design interfaces, and other cleanups
 * 
 * 2   6/30/09 10:43p leisun
 * PR 55693: Added exchange key expiration timer, and other fixes
 * 
 * 1   6/5/09 4:07p leisun
 * PR 55693: checking in DTCP-IP lib
*
* Description:
*    DTCP Exchange key related functions.
*
*********************************************************************************************/
/*! \file b_dtcp_content.c
 *  \brief Implement dtcp content management functions.
 */

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "b_os_lib.h"
#include "b_dtcp_applib.h"
#include "b_dtcp_ake.h"
#include "b_dtcp_status_codes.h"
#include "b_ecc_wrapper.h"
#include "b_dtcp_exch_key.h"

BDBG_MODULE(b_dtcp_ip);
#define EXCHANGE_KEY_COUNT		4
/*! \brief Create exchange key data, use RFNf for full and RNGr for restricted auth.
 *  This function only create keys, doesn't maintain exchange key label.
 *  \param[in, out] pExchKeyData exchange key data pointer.
 */
static BERR_Code B_DTCP_CreateExchKeyData(BCRYPT_Handle hBcrypt, B_DTCP_ExchKeyData_T * pExchKeyData)
{
	int i = 0;
	BDBG_ASSERT(pExchKeyData);

	pExchKeyData->Valid = 0;
	for( i = 0; i < EXCHANGE_KEY_COUNT; i++)
	{
		if(B_RNG(hBcrypt, pExchKeyData->ExchangeKeys[i], DTCP_EXCHANGE_KEY_SIZE) != BCRYPT_STATUS_eOK)
			return BERR_CRYPT_FAILED;
		pExchKeyData->Valid |= (1 << i);
	}
	
	return BERR_SUCCESS;
}
/*! \brief Inialize exchange key context, called by source device only
 *  \param[in] pExchKeyData input exchange key data.
 */
BERR_Code B_DTCP_InitExchKeyData(BCRYPT_Handle hBcrypt, B_DTCP_ExchKeyData_T * pExchKeyData)
{
	unsigned char max = 255, label = 0;
	BERR_Code ret = BERR_SUCCESS;
	if (pExchKeyData->Initialized == true)
		return BERR_SUCCESS;
	if((ret = B_RNG_max(hBcrypt, (unsigned char*)&label, &max, 1) != BCRYPT_STATUS_eOK))
		return BERR_CRYPT_FAILED;
	pExchKeyData->Label =  label;
	ret = B_DTCP_CreateExchKeyData(hBcrypt, pExchKeyData);
	
	if(ret == BERR_SUCCESS)
		pExchKeyData->Initialized = true;

	return ret;
}
/*! \brief helper function, return an exchange key from a set of keys
 */
static BERR_Code B_DTCP_GetExchkey(B_ExchangeKeyCipher_T Cipher, 
		B_DTCP_ExchKeyData_T * pExchKeys,
		B_MutexHandle hMutex,
		unsigned char Key[DTCP_EXCHANGE_KEY_SIZE],
		unsigned char *Label)
{
	int i;
	BERR_Code retValue = BERR_SUCCESS;
	BDBG_ASSERT(pExchKeys);
	BDBG_ASSERT(hMutex);

#ifdef DTCP_DEMO_MODE
	BDBG_MSG(("Exch Key Dump:\n"));
	BDBG_MSG(("Initialized: %d Label: %d Valid: %d\n", pExchKeys->Initialized, pExchKeys->Label, pExchKeys->Valid));
	for(i = 0; i < 4; i++)
	{
		BDBG_MSG(("Key%d\n",i));
		BDBG_BUFF(pExchKeys->ExchangeKeys[i], DTCP_EXCHANGE_KEY_SIZE);
	}
#else
	(void)i;
#endif
	if(Cipher != B_ExchKeyCipher_eM6 && Cipher != B_ExchKeyCipher_eAes)
		return BERR_INVALID_PARAMETER;
	/*B_Mutex_Lock(hMutex); */
	
	if( pExchKeys->Initialized == false)
	{
		retValue =  BERR_NOT_INITIALIZED;
		goto ERR_OUT;
	}
	if( (pExchKeys->Valid & (1 << Cipher)) == 0)
	{
		retValue = BERR_INVALID_EXCHANGE_KEY;
		goto ERR_OUT;
	}
	if(Key != NULL)
		BKNI_Memcpy(Key, pExchKeys->ExchangeKeys[Cipher], DTCP_EXCHANGE_KEY_SIZE);
	
	if(Label != NULL)
		*Label = pExchKeys->Label;
	
ERR_OUT:
	/*B_Mutex_Unlock(gAkeCoreData.hMutex); */
	return retValue;
}

/*! \brief exported function, get current exchange key from AKE core data.
 */
BERR_Code B_DTCP_GetExchKeyFromCore(struct B_DTCP_AkeCoreData * CoreData, B_ExchangeKeyCipher_T Cipher, 
		unsigned char key[DTCP_EXCHANGE_KEY_SIZE],
		unsigned char * Label)
{
	return B_DTCP_GetExchkey(Cipher, &(CoreData->ExchKeyData), CoreData->hMutex,
			key, Label);
}
/*! \brief exported function, get current exchange key from session.
 */
BERR_Code B_DTCP_GetExchKeyFromSession(void * hAkeHandle, 
		B_ExchangeKeyCipher_T Cipher,
		unsigned char key[DTCP_EXCHANGE_KEY_SIZE],
		unsigned char * Label)
{
	B_AkeCoreSessionData_T * pSession = (B_AkeCoreSessionData_T *) hAkeHandle;
		return B_DTCP_GetExchkey(Cipher, &pSession->ExchKeyData, pSession->hMutex,
			key, Label);
}
/*! \brief check if given session's exchange key is valid. 
 */
bool B_DTCP_IsExchKeyValid(void *hAkeHandle, B_ExchangeKeyCipher_T Cipher)
{
	B_AkeCoreSessionData_T * pSession = (B_AkeCoreSessionData_T *) hAkeHandle;
	if(B_DTCP_GetExchkey(Cipher, &pSession->ExchKeyData, pSession->hMutex, NULL, NULL)
			== BERR_SUCCESS)
	{
		return true;
	}else {
		return false;
	}
}
/*! \brief Source device expire/update current exchange keys.
 *  \param[in] pExchKeys pointer to source device's exchange key data.
 *  \retval BERR_SUCCESS or error code
 */
BERR_Code B_DTCP_UpdateSourceExchKeys(BCRYPT_Handle hBcrypt, B_DTCP_ExchKeyData_T * pExchKeys)
{
	BDBG_ASSERT(pExchKeys);

	BDBG_MSG(("Old Label = %d\n", pExchKeys->Label));
	if(pExchKeys->Initialized == false)
	{
		BDBG_ERR(("Exchange Key Data is not initialized\n"));
	}
	
	if(pExchKeys->Label == 255)
		pExchKeys->Label = 0;
	else
		pExchKeys->Label += 1;
	
	BDBG_MSG(("New Label = %d\n", pExchKeys->Label));
	
	return B_DTCP_CreateExchKeyData( hBcrypt, pExchKeys);
}
