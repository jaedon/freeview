/**************************************************************
*	@file		hxm3u8.c
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "../int_streaming.h"


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/



typedef struct _plStreamingKey_Item_t
{
	/* Stream info id */
	HCHAR	szPrefix[PL_STERAMINGKEY_PREFIX_LENGTH];
	HCHAR	szPrefixSpace;
	HCHAR	szUID[PL_STERAMINGKEY_UID_LENGTH];
	HCHAR	szUIDSpace;
	HCHAR	szProfile[PL_STERAMINGKEY_UID_LENGTH];
	HCHAR	szProfileSpace;

	/* device id*/
	HCHAR	szDeviceID[PL_STERAMINGKEY_DEVICEID_LENGTH];
} plStreamingKey_Item_t;

typedef union _plStreamingKey_Key_t{
	HCHAR					szKeyString[PL_STREAMINGKEY_MAX_LENGTH];
	plStreamingKey_Item_t	stKeyStruct;
} plStreamingKey_Key_u;


struct _PL_StreamingKey_t
{
	plStreamingKey_Key_u stKey;
};



STATIC PL_StreamingKey_t	s_stBaseStreamingKey;
/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC PL_StreamingKey_t *_plStreamingkey_Dup(const PL_StreamingKey_t *pstStreamingkey)
{
	return (PL_StreamingKey_t *)HLIB_STD_MemDup(pstStreamingkey, sizeof(PL_StreamingKey_t));
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_StreamingKey_Init(void)
{
	HOM_Result_e 	eHomResult = eHOM_Result_Ok;

	ENTER_FUNCTION;

	/* s_stBaseStreamingKey 값 설정 */
	(void)s_stBaseStreamingKey;

	LEAVE_FUNCTION;

	return eHomResult;
}



/**
 *
 *
 *
 * @param
 * @return
 */
void PL_StreamingKey_DeInit(void)
{
	ENTER_FUNCTION;

	LEAVE_FUNCTION;
}


/**
 *
 *
 *
 * @param
 * @return
 */
PL_StreamingKey_t * PL_StreamingKey_New(const HCHAR *pszPrefix, HUID uUID, Handle_t hMediaProfile, const HCHAR *pszDeviceID)
{
	PL_StreamingKey_t 	*pstStreamingkey = NULL;
	HUINT32				ulSvcUID = (HUINT32)uUID;

	ENTER_FUNCTION;

	if(NULL == pszPrefix || NULL == pszDeviceID)
	{
		HxLOG_Error("pstStreamingkey Check Argument!! pszPrefix(%s) ulSvcUID(%x) pszDeviceID(%x)\r\n", pszPrefix, ulSvcUID, pszDeviceID);
		return NULL;
	}

	pstStreamingkey = _plStreamingkey_Dup(&s_stBaseStreamingKey);
	if(NULL == pstStreamingkey)
	{
		HxLOG_Error("Out of memory!!\r\n");
		return NULL;
	}

	HxSTD_snprintf(pstStreamingkey->stKey.szKeyString, PL_STREAMINGKEY_MAX_LENGTH,
		PL_STREAMINGKEY_FORMAT, pszPrefix, (HUINT32)ulSvcUID, (HUINT32)hMediaProfile, pszDeviceID);

	HxLOG_Debug("StremaingKey(%s)\r\n", pstStreamingkey->stKey.szKeyString);

	LEAVE_FUNCTION;

	return pstStreamingkey;
}



/**
 *
 *
 *
 * @param
 * @return
 */
PL_StreamingKey_t * PL_StreamingKey_NewByString(const HCHAR *pszString)
{
	HINT32				nStrLen = 0;
	PL_StreamingKey_t 	*pstStreamingkey = NULL;
	HCHAR				*pszM3U8Underline = NULL;

	ENTER_FUNCTION;

	if(NULL == pszString)
	{
		HxLOG_Error("pstStreamingkey Check Argument!! pszString=>NULL\r\n");
		return NULL;
	}

	pstStreamingkey = _plStreamingkey_Dup(&s_stBaseStreamingKey);
	if(NULL == pstStreamingkey)
	{
		HxLOG_Error("Out of memory!!\r\n");
		return NULL;
	}

	pszM3U8Underline = HxSTD_StrRChar(pszString, '_');
	if(NULL != pszM3U8Underline)
	{
		nStrLen = pszM3U8Underline-pszString;
		nStrLen = (nStrLen < PL_STREAMINGKEY_MAX_LENGTH) ? nStrLen : PL_STREAMINGKEY_MAX_LENGTH - 1;

		HxSTD_StrNCpy(pstStreamingkey->stKey.szKeyString, pszString, nStrLen);
		pstStreamingkey->stKey.szKeyString[nStrLen] = '\0';
	}
	else
	{
		HxSTD_StrNCpy(pstStreamingkey->stKey.szKeyString, pszString, PL_STREAMINGKEY_MAX_LENGTH - 1);
		pstStreamingkey->stKey.szKeyString[PL_STREAMINGKEY_MAX_LENGTH - 1] = '\0';
	}

	LEAVE_FUNCTION;

	return pstStreamingkey;
}



/**
 *
 *
 *
 * @param
 * @return
 */
PL_StreamingKey_t * PL_StreamingKey_Dup(PL_StreamingKey_t *pstStreamingkey)
{
	PL_StreamingKey_t *pDup = NULL;

	ENTER_FUNCTION;

	pDup = _plStreamingkey_Dup(pstStreamingkey);

	LEAVE_FUNCTION;

	return pDup;
}


/**
 *
 *
 *
 * @param
 * @return
 */
PL_StreamingKey_t *PL_StreamingKey_ConditionalDup(const PL_StreamingKey_t *pstSrc, const HCHAR *pszPrefix, const HUID uUID, const Handle_t pMediaProfile, const HCHAR *pszDeviceID)
{
	PL_StreamingKey_t 	*pDup = NULL;
	HCHAR				szTempBuffer[PL_STREAMINGKEY_MAX_LENGTH] = {0, };

	ENTER_FUNCTION;

	if(NULL == pstSrc)
	{
		HxLOG_Error("pstStreamingkey Check Argument!!\r\n");
		return NULL;
	}

	pDup = _plStreamingkey_Dup(pstSrc);
	if(NULL == pDup)
	{
		HxLOG_Error("Out of Memory!!\r\n");
		return NULL;
	}

	if(pszPrefix)
	{
		HxSTD_memset(szTempBuffer, 0, sizeof(HCHAR) * (PL_STREAMINGKEY_MAX_LENGTH));
		HxSTD_snprintf(szTempBuffer, (PL_STERAMINGKEY_PREFIX_LENGTH+1), PL_STERAMINGKEY_PREFIX_FORMAT, pszPrefix);
		HxSTD_MemCopy(pDup->stKey.stKeyStruct.szPrefix, szTempBuffer, PL_STERAMINGKEY_PREFIX_LENGTH);
	}

	if(uUID)
	{
		HxSTD_memset(szTempBuffer, 0, sizeof(HCHAR) * (PL_STREAMINGKEY_MAX_LENGTH));
		HxSTD_snprintf(szTempBuffer, (PL_STERAMINGKEY_UID_LENGTH+1), PL_STERAMINGKEY_UID_FORMAT, (HINT32)uUID);
		HxSTD_MemCopy(pDup->stKey.stKeyStruct.szUID, szTempBuffer, PL_STERAMINGKEY_UID_LENGTH);
	}

	if(HANDLE_NULL != pMediaProfile && 0 != pMediaProfile)
	{
		HxSTD_memset(szTempBuffer, 0, sizeof(HCHAR) * (PL_STREAMINGKEY_MAX_LENGTH));
		HxSTD_snprintf(szTempBuffer, (PL_STERAMINGKEY_PROFILE_LENGTH+1), PL_STERAMINGKEY_PROFILE_FORMAT, (HINT32)pMediaProfile);
		HxSTD_MemCopy(pDup->stKey.stKeyStruct.szProfile, szTempBuffer, PL_STERAMINGKEY_PROFILE_LENGTH);
	}

	if(pszDeviceID)
	{
		HxSTD_memset(szTempBuffer, 0, sizeof(HCHAR) * (PL_STREAMINGKEY_MAX_LENGTH));
		HxSTD_snprintf(szTempBuffer, (PL_STERAMINGKEY_DEVICEID_LENGTH+1), PL_STERAMINGKEY_DEVICEID_FORMAT, pszDeviceID);
		HxSTD_MemCopy(pDup->stKey.stKeyStruct.szDeviceID, szTempBuffer, PL_STERAMINGKEY_DEVICEID_LENGTH);
	}


	LEAVE_FUNCTION;

	return pDup;
}


/**
 *
 *
 *
 * @param
 * @return
 */
void PL_StreamingKey_Delete(PL_StreamingKey_t *pstStreamingkey)
{
	ENTER_FUNCTION;

	if(NULL == pstStreamingkey)
	{
		HxLOG_Error("pstStreamingkey Check Argument!!\r\n");
		return;
	}

	HLIB_STD_MemFree(pstStreamingkey);

	LEAVE_FUNCTION;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e  PL_StreamingKey_GetItem(const PL_StreamingKey_t *pstStreamingKey, HCHAR *ret_pszPrefix, HUID *ret_puUID, Handle_t *ret_phMediaProfile, HCHAR *ret_pszDeviceID)
{
	HOM_Result_e 	eHomResult = eHOM_Result_Ok;
	HCHAR			szTempBuffer[PL_STREAMINGKEY_MAX_LENGTH] = {0, };

	ENTER_FUNCTION;

	if(NULL == pstStreamingKey)
	{
		HxLOG_Error("pstStreamingkey Check Argument!!\r\n");
		return eHOM_Result_Error;
	}

	if(ret_pszPrefix)
	{
		HxSTD_StrNCpy(ret_pszPrefix, pstStreamingKey->stKey.stKeyStruct.szPrefix, PL_STERAMINGKEY_PREFIX_LENGTH);
		ret_pszPrefix[PL_STERAMINGKEY_PREFIX_LENGTH] = 0;
	}

	if(ret_puUID)
	{
		HxSTD_memset(szTempBuffer, 0, sizeof(HCHAR) * PL_STREAMINGKEY_MAX_LENGTH);
		HxSTD_StrNCpy(szTempBuffer, pstStreamingKey->stKey.stKeyStruct.szUID, PL_STERAMINGKEY_UID_LENGTH);
		szTempBuffer[PL_STERAMINGKEY_UID_LENGTH] = 0;
		*ret_puUID = HLIB_STD_HexToInt(szTempBuffer);
	}

	if(ret_phMediaProfile)
	{
		HxSTD_memset(szTempBuffer, 0, sizeof(HCHAR) * PL_STREAMINGKEY_MAX_LENGTH);
		HxSTD_StrNCpy(szTempBuffer, pstStreamingKey->stKey.stKeyStruct.szProfile, PL_STERAMINGKEY_PROFILE_LENGTH);
		szTempBuffer[PL_STERAMINGKEY_PROFILE_LENGTH] = 0;
		*ret_phMediaProfile = HLIB_STD_HexToInt(szTempBuffer);
	}

	if(ret_pszDeviceID)
	{
		HxSTD_StrNCpy(ret_pszDeviceID, pstStreamingKey->stKey.stKeyStruct.szDeviceID, PL_STERAMINGKEY_DEVICEID_LENGTH);
		ret_pszDeviceID[PL_STERAMINGKEY_DEVICEID_LENGTH] = 0;
	}

	LEAVE_FUNCTION;


	return eHomResult;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HCHAR *PL_StreamingKey_GetBuffer(const PL_StreamingKey_t *pstStreamingKey)
{
	HCHAR *pszStreamingKeyStr = NULL;

	ENTER_FUNCTION;

	if(NULL == pstStreamingKey)
	{
		HxLOG_Error("pstStreamingkey Check Argument!!\r\n");
		return NULL;
	}

	pszStreamingKeyStr = (HCHAR *)pstStreamingKey->stKey.szKeyString;

	LEAVE_FUNCTION;

	return pszStreamingKeyStr;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HINT32 PL_StreamingKey_Copy(PL_StreamingKey_t *pstDst, const PL_StreamingKey_t *pstSrc)
{
	HINT32			nLen = 0;

	ENTER_FUNCTION;

	if(NULL == pstDst || NULL == pstSrc)
	{
		HxLOG_Error("pstStreamingkey Check Argument!! pstDst(0x%x) pstSrc(0x%x)\r\n", (HINT32)pstDst, (HINT32)pstSrc);
		return -1;
	}

	HxSTD_StrNCpy(pstDst->stKey.szKeyString, pstSrc->stKey.szKeyString, PL_STREAMINGKEY_MAX_LENGTH);
	nLen = HxSTD_StrLen(pstDst->stKey.szKeyString);

	LEAVE_FUNCTION;

	return nLen;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HINT32 PL_StreamingKey_Compare(const PL_StreamingKey_t *pstSrc1, const PL_StreamingKey_t *pstSrc2)
{
	HINT32 nCmp = 0;

	ENTER_FUNCTION;

	if(NULL == pstSrc1) return -1;
	if(NULL == pstSrc2) return 1;

	nCmp = HxSTD_StrNCmp(pstSrc1->stKey.szKeyString, pstSrc2->stKey.szKeyString, PL_STREAMINGKEY_STREMINFOID_LENGTH);

	LEAVE_FUNCTION;

	return nCmp;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HINT32 PL_StreamingKey_CompareWithoutProfile(const PL_StreamingKey_t *pstSrc1, const PL_StreamingKey_t *pstSrc2)
{
	HINT32 nCmp = 0;

	ENTER_FUNCTION;

	if(NULL == pstSrc1) return -1;
	if(NULL == pstSrc2) return 1;

	if(0 == nCmp)
	{
		nCmp = HxSTD_StrNCmp(pstSrc1->stKey.stKeyStruct.szPrefix, pstSrc2->stKey.stKeyStruct.szPrefix, PL_STERAMINGKEY_PREFIX_LENGTH);
	}

	if(0 == nCmp)
	{
		nCmp = HxSTD_StrNCmp(pstSrc1->stKey.stKeyStruct.szUID, pstSrc2->stKey.stKeyStruct.szUID, PL_STERAMINGKEY_UID_LENGTH);
	}

	if(0 == nCmp)
	{
		nCmp = HxSTD_StrNCmp(pstSrc1->stKey.stKeyStruct.szDeviceID, pstSrc2->stKey.stKeyStruct.szDeviceID, PL_STERAMINGKEY_DEVICEID_LENGTH);
	}

	LEAVE_FUNCTION;

	return nCmp;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HINT32 PL_StreamingKey_Length(const PL_StreamingKey_t *pstStreamingKey)
{
	HINT32 nLen = 0;

	ENTER_FUNCTION;

	if(NULL == pstStreamingKey)
	{
		HxLOG_Error("pstStreamingkey Check Argument!!\r\n");
		return 0;
	}

	nLen = HxSTD_StrLen(pstStreamingKey->stKey.szKeyString);

	LEAVE_FUNCTION;

	return nLen;
}

/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/

/**
 *
 *
 *
 * @param
 * @return
 */
void	PL_StreamingKey_Dump (PL_StreamingKey_t *pstStreamingkey)
{
#ifdef CONFIG_DEBUG
	if(NULL != pstStreamingkey)
	{
		HLIB_CMD_Printf("pstStreamingkey (%s)\r\n", pstStreamingkey->stKey.szKeyString);
	}
#endif
}



