
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
* ¨Ï 2011-2012 Humax Co., Ltd.
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

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <trauma_int.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define	TRAUMA_HCR_PATH								"/tmp/hcrplain"
#define	TRAUMA_MAX_HCR_DATA_SIZE 					(1024 * 1024) // 1Mbyte
#define TRAUMA_TVPORTAL_API_UPLOAD_CRASHLOG			"api/UploadCrashLog?"

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
void trauma_hcr_report(HCHAR *pszData)
{
	TRAUMA_MSG_t stMsgData;
	HCHAR 		pUrl[TRAUMA_TVPORTAL_URL_MAX_LEN]={0,};
	UNIXTIME 	uxcurtime;
	HCHAR 		*pszSysId, *pszMac, *pszSysVer;
	HxCountry_e	eCountryCode;
	HCHAR 		szbuffer[128]={0,};
	const HCHAR	*pszCountry = NULL;

	if(TRAUMA_GetSystemInfo(&pszSysId,&pszSysVer) == ERR_FAIL)
	{
		HxLOG_Error("TRAUMA_GetSystemInfo..Failed\n");
		return;
	}
	APK_SCENARIO_GetCountryCode(&eCountryCode);
	uxcurtime 	= HLIB_STD_GetSystemTime();
	pszMac 		= TRAUMA_GetLanMac();
	pszCountry 	= HLIB_COUNTRY_IdTo3166_Alpha3(eCountryCode);

	HxSTD_snprintf(pUrl, TRAUMA_TVPORTAL_URL_MAX_LEN,
		"%s/%scrashDate=%d&deviceID=%s&SystemID=%s&SWVer=%s",
		CONFIG_APPPORTAL_SERVER_URL,TRAUMA_TVPORTAL_API_UPLOAD_CRASHLOG,uxcurtime,pszMac,pszSysId,pszSysVer);
	if(pszCountry)/* [OPTIONAL] */
	{
		HxSTD_snprintf(szbuffer,128,"&country=%s",pszCountry);
		HxSTD_StrNCat(pUrl,szbuffer,HxSTD_StrLen(szbuffer));
	}
	HxLOG_Debug(HxANSI_COLOR_GREEN("URL = [%s]\n"),pUrl);

	HxSTD_MemSet(&stMsgData,0x0,sizeof(TRAUMA_MSG_t));
	stMsgData.msg = eTRAUMA_MSG_TVPORTAL_HTTPPUT_HCR;
	stMsgData.param1 = (HINT32)HLIB_STD_StrDup(pUrl);
	stMsgData.param2 = (HINT32)pszData;

	TRAUMA_TVPORTAL_MSG_Send(&stMsgData);
}
/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
void TRAUMA_HCR_Check(void)
{
	HUINT32 	ulSize = 0;
	HxFILE_t 	pFile;
	HBOOL		bDoReport = FALSE;
	HCHAR		*pszBuffer = NULL;

	if (HLIB_FILE_Exist(TRAUMA_HCR_PATH) == TRUE)
	{
		pFile = HLIB_FILE_Open(TRAUMA_HCR_PATH, (const HUINT8 *)"r");
		if(pFile != NULL)
		{
			(void)HLIB_FILE_Size32(pFile, &ulSize);
			HLIB_FILE_Close(pFile);
			HxLOG_Warning("[Humax Crash Report] check HCR size (%d).\n", ulSize);

			if (ulSize > 0 && ulSize <= TRAUMA_MAX_HCR_DATA_SIZE)
			{
				bDoReport = TRUE;
			}
			else
			{
				HxLOG_Error("HCR size over limitation(%d).\n", ulSize);
				(void)HLIB_FILE_Delete(TRAUMA_HCR_PATH);
			}
		}
	}

	if(bDoReport == TRUE)
	{
		pszBuffer = (HCHAR *)HLIB_MEM_Calloc(ulSize + 1); // + 1 is \0
		if (ERR_OK == HLIB_FILE_ReadFile(TRAUMA_HCR_PATH, (void **)&pszBuffer, &ulSize))
		{
			trauma_hcr_report(pszBuffer);
			(void)HLIB_FILE_Delete(TRAUMA_HCR_PATH);
		}
		else
		{
			HLIB_MEM_Free(pszBuffer);
		}
	}
}


