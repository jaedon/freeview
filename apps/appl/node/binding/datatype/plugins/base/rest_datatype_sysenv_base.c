/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_sysenv_base.c
	@brief

	Description:  									\n
	Module: node/binding/datatype_convert		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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
#include <dlib.h>
#include <apk.h>
#include <rest_bind_api.h>
#include <rest_datatype.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
#define _____SYSTEM_ENVIRONMENT_____
HERROR rest_datatype_base_JsonizeSysEnv (HINT32 nJsonWriter, RestSysEnv_t *pstSysEnv, HCHAR *szSelect)
{
	if (NULL == pstSysEnv)
	{
		HxLOG_Error("pstSvc NULL\n");
		return ERR_FAIL;
	}

	// Contents:
	{
		if ('\0' != pstSysEnv->szMainLangCode[0])
		{
			(void)rest_datatype_SetJsonString(nJsonWriter, "mainLanguage", pstSysEnv->szMainLangCode, szSelect);
		}

		if ('\0' != pstSysEnv->szAudioLangCode[0])
		{
			(void)rest_datatype_SetJsonString(nJsonWriter, "audioLanguage", pstSysEnv->szAudioLangCode, szSelect);
		}
		
		if ('\0' != pstSysEnv->szSubtitleLangCode[0])
		{
			(void)rest_datatype_SetJsonString(nJsonWriter, "subtitleLanguage", pstSysEnv->szSubtitleLangCode, szSelect);
		}

		if (-1 != pstSysEnv->nHardOfHearing)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "hardOfHearing", pstSysEnv->nHardOfHearing, szSelect);
		}

		if (-1 != pstSysEnv->nMaturityRating)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "maturityRating", DxNAMEOF(DxRatingAge_e), pstSysEnv->nMaturityRating, szSelect);
		}

		if (eCountryID_NONE != pstSysEnv->eCountry)
		{
			HCHAR	*szAlpha3 = (HCHAR *)HLIB_COUNTRY_IdTo3166_Alpha3(pstSysEnv->eCountry);

			if (NULL != szAlpha3)
			{
				(void)rest_datatype_SetJsonString(nJsonWriter, "country", szAlpha3, szSelect);
			}
		}

		if (-1 != pstSysEnv->nAntConnectType)
		{
			(void)rest_datatype_SetJsonEnum(nJsonWriter, "antConnect", DxNAMEOF(DxAntennaConnection_e), pstSysEnv->nAntConnectType, szSelect);
		}

		if (-1 != pstSysEnv->nStartPaddingTime)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "startPaddingTime", pstSysEnv->nStartPaddingTime, szSelect);
		}
		
		if (-1 != pstSysEnv->nEndPaddingTime)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "endPaddingTime", pstSysEnv->nEndPaddingTime, szSelect);
		}
		
		if (-1 != pstSysEnv->nAutoDelete)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "autoDelete", pstSysEnv->nAutoDelete, szSelect);
		}
		
		if (-255 != pstSysEnv->nGmtOffset)
		{
			(void)rest_datatype_SetJsonInteger(nJsonWriter, "gmtOffset", pstSysEnv->nGmtOffset, szSelect);
		}
	}

	return ERR_OK;
}

HERROR rest_datatype_base_DejsonizeSysEnv (HxJSON_t *pstJson, RestSysEnv_t *pstSysEnv)
{
	HCHAR			 szAlpha3[4];

	if (NULL == pstSysEnv)
	{
		HxLOG_Error("pstSysEnv NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(szAlpha3, 0, 4);

	(void)rest_datatype_GetJsonString(pstJson, "mainLanguage", pstSysEnv->szMainLangCode, 3);
	(void)rest_datatype_GetJsonString(pstJson, "audioLanguage", pstSysEnv->szAudioLangCode, 3);
	(void)rest_datatype_GetJsonString(pstJson, "subtitleLanguage", pstSysEnv->szSubtitleLangCode, 3);
	(void)rest_datatype_GetJsonInteger(pstJson, "hardOfHearing", &(pstSysEnv->nHardOfHearing));
	(void)rest_datatype_GetJsonEnum(pstJson, "maturityRating", DxNAMEOF(DxRatingAge_e), &(pstSysEnv->nMaturityRating));
	(void)rest_datatype_GetJsonString(pstJson, "country", szAlpha3, 3);
	pstSysEnv->eCountry = HLIB_COUNTRY_3166_Alpha3ToId((const HCHAR *)szAlpha3);
	(void)rest_datatype_GetJsonEnum(pstJson, "antConnect", DxNAMEOF(DxAntennaConnection_e), &(pstSysEnv->nAntConnectType));
	(void)rest_datatype_GetJsonInteger(pstJson, "startPaddingTime", &(pstSysEnv->nStartPaddingTime));
	(void)rest_datatype_GetJsonInteger(pstJson, "endPaddingTime", &(pstSysEnv->nEndPaddingTime));
	(void)rest_datatype_GetJsonInteger(pstJson, "autoDelete", &(pstSysEnv->nAutoDelete));
	(void)rest_datatype_GetJsonInteger(pstJson, "gmtOffset", &(pstSysEnv->nGmtOffset));

	return ERR_OK;	
}


