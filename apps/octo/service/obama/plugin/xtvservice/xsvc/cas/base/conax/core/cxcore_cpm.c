/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/**
	@file     cxcore_cpm.c
	@brief    cxcore_cpm.c (CONAX MAIN / CPM)

	Description: Chipset pairing module \n
	Module: MW/CAS/ CONAX \n
	Remarks :\n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <pal_sys.h>
#include <_svc_cas_mgr_pal_port.h>
#include "../local_include/cxcore_main.h"
#include "../local_include/cxcore_os.h"
#include "../local_include/cxcore_nvm.h"
#include "../local_include/cxcore_cpm.h"
#include "../local_include/cxcore_stream.h"
#include "../local_include/cx_dscr.h"

/*******************************************************************/
/********************      Debug				********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines  	   *************************/
/*******************************************************************/
#define CPM_LP_PROTOCOL_ID 0x2001

/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

typedef struct
{
	HUINT8		ucCWE;
	HUINT32		ulChipId;
	HUINT32		ulLPK;
	HUINT32		ulCWPK;
	HUINT32		ulUnknown;
	CxCpmKey_t	aKeyStore[CX_CPM_MAX_KEY_SLOT];
} CxCpm_t;


/*******************************************************************/
/********************      Static Variables 	********************/
/*******************************************************************/
static	CxCpm_t	s_cpm;

/*******************************************************************/
/********************      Functions	   *************************/
/*******************************************************************/

#if 0
static void CxCpmPrint(HUINT32 ulDbgLv, const CxCpmKey_t* pKey, const HUINT8* szFmt, ...)
{
#if defined(CONFIG_DEBUG)
	HINT32	i;
	HUINT8	szBuf[256];
	HUINT8	szMsg[512];
	va_list	va;
	HUINT8	cwpk	= (HUINT8)eCxCpm_CWPK;
	HUINT8	lpk		= (HUINT8)eCxCpm_LPK;

	if (!(ulDbgLv & g_CxCoreCpm_Level))
	{
		return;
	}

	if (pKey)
	{
		HUINT8* pszBuf;

		snprintf(szBuf, 256,				//
				">>> [CxCpm] Key::"			//
				"type(0x%02X,%s)"			//
				", ID: 0x%04X"				//
				", value: "					//
				, pKey->ucType				//
				, (pKey->ucType == cwpk) ? "CWPK" : (pKey->ucType == lpk) ? "LPK" : "Unknown"	//
				, pKey->usId				//
			);

		pszBuf = szBuf + strlen(szBuf);

		for (i = 0 ; i < CX_CPM_KEY_VALUE_BYTES ; i++)
		{
			snprintf(pszBuf, 2, "%02X", pKey->aucValue[i]);
			pszBuf += 2;
		}
		*pszBuf = '\0';
	}
	else
	{
		snprintf(szBuf, 256, ">>> [CxCpm] ");
	}

	va_start(va, szFmt);
	vsprintf(szMsg, szFmt, va);
	va_end(va);

	HxLog_Error("%s | \"%s\"\n", szBuf, szMsg);
#endif
}
#endif

static HBOOL local_cxcpm_IsEmptyKey(const CxCpmKey_t* pKey)
{
	static const HUINT8	aucEmptyValue[CX_CPM_KEY_VALUE_BYTES]	=
	{
		0,
	};
	HINT32				nDiff;

	nDiff = CX_memcmp(pKey->aucValue, aucEmptyValue, pKey->ucLength);

	return nDiff == 0 ? FALSE : TRUE;
}

static CxCpmKey_t* local_cxcpm_FindKey(CxCpm_t* pCpm, HUINT16 usId)
{
	HINT32	i;

	for (i = 0 ; i < CX_CPM_MAX_KEY_SLOT ; i ++)
	{
		if (pCpm->aKeyStore[i].usId == usId)
		{
			return pCpm->aKeyStore + i;
		}
	}
	return NULL;
}

static CxCpmKey_t* local_cxcpm_GetEmptySlot(CxCpm_t* pCpm)
{
	return local_cxcpm_FindKey(pCpm, CX_CPM_INVALID_KEYID);
}

static HERROR local_cxcpm_KeyCompare(CxCpmKey_t* pKey, CxCpmKey_t* pNewKey)
{
	if (pKey->ucType != pNewKey->ucType)
	{
		return ERR_FAIL;
	}

	if (pKey->ucLength != pNewKey->ucLength)
	{
		return ERR_FAIL;
	}

	if (CX_memcmp(pKey->aucValue, pNewKey->aucValue, pNewKey->ucLength) != 0)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_cxcpm_UpdateKey(CxCpm_t* pCpm, HUINT32 ulChipId, CxCpmKey_t* pNewKey)
{
	CxCpmKey_t* pKey;

	if (ulChipId != pCpm->ulChipId)
	{
		HxLOG_Print("ChipID unmatch (STB : %d, EMM : %d)\n", pCpm->ulChipId, ulChipId);
		return ERR_OK;
	}

	if (local_cxcpm_IsEmptyKey(pNewKey) == FALSE)
	{
		HxLOG_Error("Empty value!!\n");
		return ERR_FAIL;
	}

	if (pNewKey->ucType == (HUINT8)eCxCpm_CWPK)
	{
		if(pNewKey->usId > 0xff)
		{
			HxLOG_Error("keyId of CWPK is bigger than 0xFF.\n");
			return ERR_FAIL;
		}
	}
	else if (pNewKey->ucType == (HUINT8)eCxCpm_LPK)
	{
		if(pNewKey->usId == CX_CPM_INVALID_KEYID)
		{
			HxLOG_Error("keyId of LPK is invalid.\n");
			return ERR_FAIL;
		}
	}
	else
	{
		; // 뭐해야 하나...?
	}

	pKey = local_cxcpm_FindKey(pCpm, pNewKey->usId);

	if (pKey)
	{
		if (local_cxcpm_KeyCompare(pKey, pNewKey) == ERR_OK)
		{
			HxLOG_Print("Has same Key in KeyStore\n");
			return ERR_OK;
		}

		// 기존에 key의 type이 변경이 되었습니다...
		// 이전 key의 count는 감소시키고 새 key의 count는 증가시킵니다..
		if (pKey->ucType != pNewKey->ucType)
		{
			if (pKey->ucType == (HUINT8)eCxCpm_CWPK)
			{
				pCpm->ulCWPK--;
			}
			else if (pKey->ucType == (HUINT8)eCxCpm_LPK)
			{
				pCpm->ulLPK--;
			}
			else
			{
				pCpm->ulUnknown--;
			}

			if (pNewKey->ucType == (HUINT8)eCxCpm_CWPK)
			{
				pCpm->ulCWPK++;
			}
			else if (pNewKey->ucType == (HUINT8)eCxCpm_LPK)
			{
				pCpm->ulLPK++;
			}
			else
			{
				pCpm->ulUnknown++;
			}
		}
	}
	else // if (pKey == NULL)
	{
		pKey = local_cxcpm_GetEmptySlot(pCpm);
		if (pKey == NULL)	// FULL??
		{
			HxLOG_Error("KeyStore overflow\n");
			return ERR_FAIL;
		}

		if (pNewKey->ucType == (HUINT8)eCxCpm_CWPK)
		{
			pCpm->ulCWPK++;
		}
		else if (pNewKey->ucType == (HUINT8)eCxCpm_LPK)
		{
			pCpm->ulLPK++;
		}
		else
		{
			pCpm->ulUnknown++;
		}
	}

	//CxCpmPrint(DBG_PRINT, pNewKey, "New Key Updated!!");

	CX_memcpy(pKey, pNewKey, sizeof(CxCpmKey_t));

	return ERR_OK;
}

static HERROR local_cxcpm_RemoveKey(CxCpm_t* pCpm, HUINT32 ulChipId, HUINT16 usKeyId)
{
	CxCpmKey_t* pKey;

	if (ulChipId != pCpm->ulChipId)
	{
		HxLOG_Print("ChipID unmatch (STB : %d, EMM : %d)\n", pCpm->ulChipId, ulChipId);
		return ERR_OK;
	}

	pKey = local_cxcpm_FindKey(pCpm, usKeyId);
	if (pKey == NULL)
	{
		HxLOG_Print("Has no matched key in KeyStore(ID:0x%04X)\n", usKeyId);
		return ERR_OK;
	}

	if (pKey->ucType == (HUINT8)eCxCpm_CWPK)
	{
		pCpm->ulCWPK--;
	}
	else if (pKey->ucType == (HUINT8)eCxCpm_LPK)
	{
		pCpm->ulLPK--;
	}
	else
	{
		pCpm->ulUnknown--;
	}

	//CxCpmPrint(DBG_PRINT, pKey, "Key Removed!!");

	// remove 되는 key가 descrambler에 load되어 있다면 clear 시키자...
	if (pKey->ucType == (HUINT8)eCxCpm_CWPK)
	{
		CXSTREAM_RemoveCWPK(usKeyId);
	}

	CX_memset(pKey, 0, sizeof(CxCpmKey_t));

	return ERR_OK;
}

#define INIT_________________________________________________

HERROR CXCPM_InitCore(void)
{
	HERROR	err;
	HINT32	i;
	HUINT8	ucSCS, ucJTAG, ucOTP;
	HUINT8	ucRemux			= 0, ucMarketID	= 0;

	ENTER_FUNCTION;

	CX_memset(&s_cpm, 0, sizeof(CxCpm_t));

	err = svc_cas_PalPortSysGetFusingInfoForConax(&ucSCS, &ucJTAG, &ucOTP, &s_cpm.ucCWE, &ucRemux, &ucMarketID);
	if (err != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevPortSysGetFusingInfo() error\n");
	}

	HxLOG_Error("Fusing state : SCS(%d), JTAG(%d), OTP(%d), CWE(%d), RemuxFusing(%d), MarketID(%d)\n", ucSCS, ucJTAG, ucOTP, s_cpm.ucCWE, ucRemux, ucMarketID);


	err = svc_cas_DevPortSysGetChipId(&s_cpm.ulChipId);
	if (err != ERR_OK)
	{
		LEAVE_FUNCTION;
		return err;
	}

	HxLOG_Error("Chipset ID : %d\n", s_cpm.ulChipId);

	err = CXNVM_ReadKeyStore(s_cpm.aKeyStore, 0, sizeof(CxCpmKey_t) * CX_CPM_MAX_KEY_SLOT);
	if (err != ERR_OK)
	{
		LEAVE_FUNCTION;
		return err;
	}

	for (i = 0 ; i < CX_CPM_MAX_KEY_SLOT ; i++)
	{
		if (s_cpm.aKeyStore[i].usId == CX_CPM_INVALID_KEYID)
		{
			continue;
		}

		if (s_cpm.aKeyStore[i].ucType == (HUINT8)eCxCpm_CWPK)
		{
			s_cpm.ulCWPK++;
		}
		else if (s_cpm.aKeyStore[i].ucType == (HUINT8)eCxCpm_LPK)
		{
			s_cpm.ulLPK++;
		}
		else
		{
			s_cpm.ulUnknown++;
		}
	}



	// TODO: Semaphore???

	LEAVE_FUNCTION;

	return ERR_OK;
}


HBOOL CXCPM_IsEncryptedCwMode(void)
{
#if defined(CONFIG_MW_CAS_CONAX_PRETEST)
	return TRUE;
#else
	return s_cpm.ucCWE;
#endif
}


HUINT32 CXCPM_CountKey(CxCpmKeyType_t type)
{
	HUINT32	count			= 0;

	ENTER_FUNCTION;

	if (type == eCxCpm_CWPK)
	{
		count = s_cpm.ulCWPK;
	}
	else if (type == eCxCpm_LPK)
	{
		count = s_cpm.ulLPK;
	}
	else
	{
		count = s_cpm.ulUnknown;
	}

	LEAVE_FUNCTION;

	return count;
}



HERROR CXCPM_GetProtocolId(HUINT16* pusId)
{
	ENTER_FUNCTION;

	if (pusId == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

#if defined(CONFIG_PROD_CPU_BCM)
	*pusId = 0x2000;
#elif defined(CONFIG_PROD_CPU_ST)
	*pusId = 0x1000;
#else
#error	What the ...??
#endif

#if defined(CONFIG_MW_CAS_CONAX_CPM_WITH_LPK)
	// Chipset Pairing with Link Protection - please contact Conax(stbsupport@conax.com) for legal value...
	*pusId = CPM_LP_PROTOCOL_ID;
#endif

	LEAVE_FUNCTION;
	return ERR_OK;
}


HERROR CXCPM_GetKey(HUINT16 usKeyId, CxCpmKey_t* pKeyRet)
{
	CxCpmKey_t* pKey;

	ENTER_FUNCTION;

	if (pKeyRet == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	pKey = local_cxcpm_FindKey(&s_cpm, usKeyId);
	if (pKey == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_memcpy(pKeyRet, pKey, sizeof(CxCpmKey_t));

	LEAVE_FUNCTION;
	return ERR_OK;
}


HBOOL CXCPM_HasValue(CxCpmKey_t* pKey)
{
	HBOOL	bRet;

	ENTER_FUNCTION;

	if (pKey == NULL)
	{
		LEAVE_FUNCTION;
		return FALSE;
	}

	bRet = local_cxcpm_IsEmptyKey(pKey);

	LEAVE_FUNCTION;

	return bRet;
}


HERROR CXCPM_Process_UPDATE_KEY(void* context, HUINT32 ulChipId, CxCpmKeyType_t type, HUINT16 usKeyId, const HUINT8* pucValue, HUINT8 ucPL)
{
	HERROR		err;
	CxCpmKey_t	newKey;

	ENTER_FUNCTION;

	(void)context;	// reserved for future use

	if (pucValue == NULL)
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	CX_memset(&newKey, 0, sizeof(CxCpmKey_t));

	newKey.ucType = (HUINT8)type;
	newKey.usId = usKeyId;
	newKey.ucLength = ucPL - 7;		// (PL-7) by specification.

	if (newKey.ucLength > CX_CPM_KEY_VALUE_BYTES)
	{
		newKey.ucLength = CX_CPM_KEY_VALUE_BYTES;
	}

	CX_memcpy(newKey.aucValue, pucValue, newKey.ucLength);

	err = local_cxcpm_UpdateKey(&s_cpm, ulChipId, &newKey);
	if (err != ERR_OK)
	{
		LEAVE_FUNCTION;
		return err;
	}

	// TODO: 일단 몽땅 쓰자
	err = CXNVM_WriteKeyStore(s_cpm.aKeyStore, 0, sizeof(CxCpmKey_t) * CX_CPM_MAX_KEY_SLOT);

	LEAVE_FUNCTION;

	return err;
}

HERROR CXCPM_Process_REMOVE_KEY(void* context, HUINT32 ulChipId, HUINT16 usKeyId)
{
	HERROR	err;

	ENTER_FUNCTION;

	(void)context;	// reserved for future use

	err = local_cxcpm_RemoveKey(&s_cpm, ulChipId, usKeyId);
	if (err != ERR_OK)
	{
		LEAVE_FUNCTION;
		return err;
	}
	// TODO: 일단 몽땅 쓰자
	err = CXNVM_WriteKeyStore(s_cpm.aKeyStore, 0, sizeof(CxCpmKey_t) * CX_CPM_MAX_KEY_SLOT);

	LEAVE_FUNCTION;
	return err;
}

/*********************** End of File ******************************/


