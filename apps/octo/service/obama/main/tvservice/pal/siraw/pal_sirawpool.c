/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_sirawpool.c
	@brief

	Description:  									\n
	Module: PAL / SIRAW					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
 * (c) 2011-2013 Humax Co., Ltd.
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>

#include <pal_sirawpool.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	SIxRawPoolHandle_t		 hPool;
	unsigned long			 ulSemId;

	HUINT32					 ulCntSameSec;	// all retreive시 사용
} palRawpool_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC palRawpool_t			 s_astPalSiRawPool_Pools[eActionId_MAX];

#define ___LOCAL_FUNCTIONS___
STATIC palRawpool_t *pal_sirawpool_getPalRawPool (HUINT32 ulActionId)
{
	if (ulActionId < eActionId_MAX)
	{
		return &(s_astPalSiRawPool_Pools[ulActionId]);
	}

	return NULL;
}

STATIC SIxDescriptor_t *pal_sirawpool_ParseDesEx (HUINT32 ulActionId, HUINT8 *pucRawData, SIxSpec_e eParseSpec)
{
	SIxRawPoolProperty_t	 stProperty;
	palRawpool_t			*pstPalRawPool = NULL;
	HERROR					 hErr = ERR_FAIL;
	SIxSpec_e 				 eOrgSpec = eSIxSPEC_DVB;
	SIxDescriptor_t			*pstDes = NULL;

	if (NULL == pucRawData)			{ return NULL; }

	switch (eParseSpec)
	{
		case eSIxSPEC_DVBEXT:
		case eSIxSPEC_SSUUNT:
			break;

		default:
			HxLOG_Error("Not support si spec (%d) \n", eParseSpec);
	}

	pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	if (pstPalRawPool == NULL)
	{
		HxLOG_Error ("pal_sirawpool_getPalRawPool() failed.. invalid actionId(%d) \n", ulActionId);
		return NULL;
	}

	VK_SEM_Get (pstPalRawPool->ulSemId);

	// get original si pool spec
	hErr = SILIB_RAW_GetProperty (pstPalRawPool->hPool, &stProperty);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SILIB_RAW_GetProperty() failed..!! \n");
		goto exit_func;
	}

	// set temporary si pool spec
	eOrgSpec = stProperty.eSpec;
	stProperty.eSpec = eParseSpec;
	hErr = SILIB_RAW_ChangeProperty (pstPalRawPool->hPool, &stProperty);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SILIB_RAW_ChangeProperty() failed..!! \n");
		goto exit_func;
	}

	// parse des.
	pstDes = SILIB_RAW_ParseDes (pstPalRawPool->hPool, pucRawData);

	// set original si pool spec
	stProperty.eSpec = eOrgSpec;
	hErr = SILIB_RAW_ChangeProperty (pstPalRawPool->hPool, &stProperty);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("SILIB_RAW_ChangeProperty() failed..!! \n");
	}

exit_func:

	VK_SEM_Release (pstPalRawPool->ulSemId);

	return pstDes;
}


#define ___MEMBER_FUNCTIONS___

#define ___API_FUNCTIONS___
HERROR PAL_SIRAWPOOL_Init (void)
{
	HUINT32					 ulCount;
	HCHAR					 szSemName[32];
	SIxRawPoolProperty_t	 stPoolProperty;
	HERROR					 hErr;

	HxSTD_MemSet (s_astPalSiRawPool_Pools, 0, sizeof(palRawpool_t) * eActionId_MAX);

	for (ulCount = 0; ulCount < eActionId_MAX; ulCount++)
	{
		palRawpool_t		*pstPalPool = pal_sirawpool_getPalRawPool (ulCount);

		HxSTD_PrintToStrN (szSemName, 31, "plRawPlSem_%02d", ulCount);
		szSemName[31] = '\0';

		hErr = VK_SEM_Create (&(pstPalPool->ulSemId), szSemName, VK_SUSPENDTYPE_FIFO);
		if (VK_OK != hErr)
		{
			HxLOG_Error ("VK_SEM_Create err: ulCount(%d)\n", ulCount);
			return ERR_FAIL;
		}

		HxSTD_MemSet (&stPoolProperty, 0, sizeof(SIxRawPoolProperty_t));
		HxSTD_PrintToStrN (stPoolProperty.szPoolName, SILIB_RAWPOOL_NAME_LEN, "PalRawPool_%02d", ulCount);
		stPoolProperty.szPoolName[SILIB_RAWPOOL_NAME_LEN] = '\0';

		pstPalPool->hPool = SILIB_RAW_CreatePool (&stPoolProperty);
		if (SILIB_RAWPOOL_HANDLE_NULL == pstPalPool->hPool)
		{
			HxLOG_Error ("SILIB_RAW_CreatePool err: ulCount(%d)\n", ulCount);
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

HERROR PAL_SIRAWPOOL_Clear (HUINT32 ulActionId)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HERROR					 hErr;

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	hErr = SILIB_RAW_ClearPool (pstPalRawPool->hPool);
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hErr;
}

HERROR PAL_SIRAWPOOL_GetSiProperty (HUINT32 ulActionId, SIxSpec_e *peSpec, SIxTextEncoding_e *peTextEnc)
{
	SIxRawPoolProperty_t	 stProperty;
	palRawpool_t			*pstPalRawPool = NULL;
	HERROR					 hResult = ERR_FAIL;

	if ((peSpec == NULL) && (peTextEnc == NULL))
	{
		HxLOG_Error ("invalid parameters - actionId(%d) \n", ulActionId);
		return ERR_FAIL;
	}

	pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	if (pstPalRawPool == NULL)
	{
		HxLOG_Error ("invalid actionId(%d) \n", ulActionId);
		return ERR_FAIL;
	}

	VK_SEM_Get (pstPalRawPool->ulSemId);

	hResult = SILIB_RAW_GetProperty (pstPalRawPool->hPool, &stProperty);
	if (hResult == ERR_OK)
	{
		if (peSpec != NULL)
		{
			*peSpec = stProperty.eSpec;
		}

		if (peTextEnc != NULL)
		{
			*peTextEnc = stProperty.eTextEnc;
		}
	}

	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hResult;
}

HERROR PAL_SIRAWPOOL_ChangeSiSpecType (HUINT32 ulActionId, SIxSpec_e eSpec)
{
	SIxRawPoolProperty_t		 stProperty;
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HERROR					 hErr, hResult = ERR_FAIL;

	if (pstPalRawPool == NULL)
	{
		HxLOG_Error ("invalid actionId(%d) \n", ulActionId);
		return ERR_FAIL;
	}

	VK_SEM_Get (pstPalRawPool->ulSemId);

	hErr = SILIB_RAW_GetProperty (pstPalRawPool->hPool, &stProperty);
	if (ERR_OK == hErr)
	{
		if (stProperty.eSpec == eSpec)
		{
			// 동일한 내용은 업데이트 생략한다
			hResult = ERR_OK;
		}
		else
		{
			stProperty.eSpec = eSpec;
			hErr = SILIB_RAW_ChangeProperty (pstPalRawPool->hPool, &stProperty);
			if (ERR_OK == hErr)
			{
				hResult = ERR_OK;
			}
		}
	}

	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hResult;
}

HERROR PAL_SIRAWPOOL_ChangeSiTextEncType (HUINT32 ulActionId, SIxTextEncoding_e eTextEnc)
{
	SIxRawPoolProperty_t		 stProperty;
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HERROR					 hErr, hResult = ERR_FAIL;

	if (pstPalRawPool == NULL)
	{
		HxLOG_Error ("invalid actionId(%d) \n", ulActionId);
		return ERR_FAIL;
	}

	VK_SEM_Get (pstPalRawPool->ulSemId);

	hErr = SILIB_RAW_GetProperty (pstPalRawPool->hPool, &stProperty);
	if (ERR_OK == hErr)
	{
		if (stProperty.eTextEnc == eTextEnc)
		{
			// 동일한 내용은 업데이트 생략한다
			hResult = ERR_OK;
		}
		else
		{
			stProperty.eTextEnc = eTextEnc;
			hErr = SILIB_RAW_ChangeProperty (pstPalRawPool->hPool, &stProperty);
			if (ERR_OK == hErr)
			{
				hResult = ERR_OK;
			}
		}
	}

	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hResult;
}

HERROR PAL_SIRAWPOOL_IncreaseSameSectionCounter(HUINT32 ulActionId, HUINT32 *pulCnt)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	pstPalRawPool->ulCntSameSec++;
	*pulCnt = pstPalRawPool->ulCntSameSec;
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return ERR_OK;
}

HERROR PAL_SIRAWPOOL_GetSameSectionCounter(HUINT32 ulActionId, HUINT32 *pulCnt)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	*pulCnt = pstPalRawPool->ulCntSameSec;
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return ERR_OK;
}

HERROR PAL_SIRAWPOOL_ResetSameSectionCounter(HUINT32 ulActionId)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	pstPalRawPool->ulCntSameSec = 0;
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return ERR_OK;
}

HERROR PAL_SIRAWPOOL_FinalizeIncompleteSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulExtId)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HERROR					 hErr = ERR_FAIL;

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	hErr = SILIB_RAW_FinalizeIncompleteSection(pstPalRawPool->hPool, eSiType, ulExtId);
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hErr;
}

HERROR PAL_SIRAWPOOL_AddRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT8 *pucRaw, HUINT32 ulUniqueId)
{
	SIxRawOption_t			 stOption;
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HERROR					 hErr;

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	HxSTD_MemSet (&stOption, 0, sizeof(SIxRawOption_t));
	stOption.ulUniqueId = ulUniqueId;

	VK_SEM_Get (pstPalRawPool->ulSemId);
	hErr = SILIB_RAW_AddRawSection (pstPalRawPool->hPool, eSiType, pucRaw, &stOption, FALSE);
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hErr;
}

HERROR PAL_SIRAWPOOL_AddMultiRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulRawNum, HUINT8 **ppucRawArray, HUINT32 ulUniqueId, HBOOL bIncompleteSec)
{
	HUINT32					 ulCount;
	SIxRawOption_t			 stOption;
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }
	if ((0 == ulRawNum) || (NULL == ppucRawArray))	{ return ERR_FAIL; }

	HxSTD_MemSet (&stOption, 0, sizeof(SIxRawOption_t));
	stOption.ulUniqueId = ulUniqueId;

	VK_SEM_Get (pstPalRawPool->ulSemId);
	for (ulCount = 0; ulCount < ulRawNum; ulCount++)
	{
		SILIB_RAW_AddRawSection (pstPalRawPool->hPool, eSiType, ppucRawArray[ulCount], &stOption, bIncompleteSec);
	}
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return ERR_OK;
}

HERROR PAL_SIRAWPOOL_ReplaceMultiRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId, HUINT32 ulRawNum, HUINT8 **ppucRawArray, HUINT32 ulUniqueId)
{
	HUINT32					 ulCount;
	SIxRawOption_t			 stOption;
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }
	if ((0 == ulRawNum) || (NULL == ppucRawArray))	{ return ERR_FAIL; }

	HxSTD_MemSet (&stOption, 0, sizeof(SIxRawOption_t));
	stOption.ulUniqueId = ulUniqueId;

	VK_SEM_Get (pstPalRawPool->ulSemId);

	(void)SILIB_RAW_FreeRawSectionByExtId (pstPalRawPool->hPool, eSiType, bUseExtId, ulExtId);
	for (ulCount = 0; ulCount < ulRawNum; ulCount++)
	{
		SILIB_RAW_AddRawSection (pstPalRawPool->hPool, eSiType, ppucRawArray[ulCount], &stOption, FALSE);
	}

	VK_SEM_Release (pstPalRawPool->ulSemId);

	return ERR_OK;
}

HERROR PAL_SIRAWPOOL_FindSameRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulExtId, HUINT32 ulRawNum, HUINT8 **ppucRawArray)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HUINT32					 ulCount;
	HERROR					 hErr = ERR_FAIL;

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	HxLOG_Print("ulRawNum = %d, ulExtId = 0x%x\n", ulRawNum, ulExtId);
	for (ulCount = 0; ulCount < ulRawNum; ulCount++)
	{
		hErr = SILIB_RAW_FindSameRawSection (pstPalRawPool->hPool, eSiType, ulExtId, ppucRawArray[ulCount]);
		if (hErr == ERR_OK)
		{
			// find same raw section
			break;
		}
	}
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hErr;
}

HERROR PAL_SIRAWPOOL_FreeRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HERROR					 hErr;

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	hErr = SILIB_RAW_FreeRawSectionByExtId (pstPalRawPool->hPool, eSiType, bUseExtId, ulExtId);
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hErr;
}

HERROR PAL_SIRAWPOOL_FreeRawSectionByUniqueId (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulUniqueId)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HERROR					 hErr;

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	hErr = SILIB_RAW_FreeRawSectionByUniqueId (pstPalRawPool->hPool, eSiType, ulUniqueId);
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hErr;
}

HERROR PAL_SIRAWPOOL_FreeAllRawSection (HUINT32 ulActionId, SIxTable_Type_e eSiType)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HERROR					 hErr;

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	hErr = SILIB_RAW_FreeAllRawSection (pstPalRawPool->hPool, eSiType);
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hErr;
}

SIxTable_PsiTable_t *PAL_SIRAWPOOL_ParseTable (HUINT32 ulActionId, SIxTable_Type_e eSiType, HUINT32 ulExtId)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	SIxTable_PsiTable_t		*pstTable = NULL;

	if (NULL == pstPalRawPool)						{ return NULL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	pstTable = SILIB_RAW_Parse (pstPalRawPool->hPool, eSiType, ulExtId);
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return pstTable;
}

void PAL_SIRAWPOOL_FreeTable (SIxTable_PsiTable_t *pstTable)
{
	SILIB_TABLE_Free (pstTable);
}

HUINT32 PAL_SIRAWPOOL_CountRaws (HUINT32 ulActionId, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId)
{
	HUINT32					 ulCount;
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);

	if (NULL == pstPalRawPool)						{ return 0; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	ulCount = SILIB_RAW_CountRaw (pstPalRawPool->hPool, eSiType, bUseExtId, ulExtId);
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return ulCount;
}

HERROR PAL_SIRAWPOOL_GetRawArrays (HUINT32 ulActionId, SIxTable_Type_e eSiType, HBOOL bUseExtId, HUINT32 ulExtId, HUINT32 ulRawArrayMax, HUINT8 **ppucRawArray, HUINT32 *pulNumRaw)
{
	palRawpool_t			*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	HERROR					 hErr;

	if (NULL == pstPalRawPool)						{ return ERR_FAIL; }

	VK_SEM_Get (pstPalRawPool->ulSemId);
	hErr = SILIB_RAW_GetRawArrays (pstPalRawPool->hPool, eSiType, bUseExtId, ulExtId, ulRawArrayMax, ppucRawArray, pulNumRaw);
	VK_SEM_Release (pstPalRawPool->ulSemId);

	return hErr;
}

SIxDescriptor_t *PAL_SIRAWPOOL_ParseDes (HUINT32 ulActionId, HUINT8 *pucRawData)
{
	palRawpool_t				*pstPalRawPool = pal_sirawpool_getPalRawPool (ulActionId);
	SIxDescriptor_t				*pstDes;

	if ((NULL == pstPalRawPool) || (NULL == pucRawData))			{ return NULL; }

	// Raw Data를 쓰는 것이 아니므로 Semaphore까지는 필요 없을 것으로 본다.
	//VK_SEM_Get (pstPalRawPool->ulSemId);
	pstDes = SILIB_RAW_ParseDes (pstPalRawPool->hPool, pucRawData);
	//VK_SEM_Release (pstPalRawPool->ulSemId);

	return pstDes;
}

SIxDescriptor_t *PAL_SIRAWPOOL_ParseDvbExtDes (HUINT32 ulActionId, HUINT8 *pucRawData)
{
	return pal_sirawpool_ParseDesEx(ulActionId, pucRawData, eSIxSPEC_DVBEXT);
}

SIxDescriptor_t *PAL_SIRAWPOOL_ParseSsuUntDes (HUINT32 ulActionId, HUINT8 *pucRawData)
{
	return pal_sirawpool_ParseDesEx(ulActionId, pucRawData, eSIxSPEC_SSUUNT);
}

void PAL_SIRAWPOOL_FreeDes (SIxDescriptor_t *pstDes)
{
	(void)SILIB_DESC_Delete (pstDes);
}

/* end of file */

