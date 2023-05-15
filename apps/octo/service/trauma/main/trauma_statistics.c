
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
#include <trauma_tool.h>
#include <sapi.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define TRAUMA_STATISTICS_REPORT_INTERVAL 			(2* 60 * 1000) // default 2 mins
#define TRAUMA_STATISTICS_CODE_SETINFO_INTERVAL		(24 * 60 * 60 * 1000) // 24 hours
#define TRAUMA_STATISTICS_CODE_SALIVE_INTERVAL 		(10  * 60 *1000) // 10 mins
#define TRAUMA_STATISTICS_CODE_IPEPG_INTERVAL 		(10  * 60 *1000) // 10 mins
/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */
HxList_t	*s_pStatisticsLogs = NULL;
HUINT32		s_ulSemId =0;
HULONG		s_ulInternval_timer =0;
HULONG		s_ulttl_timer =0;

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */

static HBOOL trauma_statistics_IsSameJsonValue(HxJSON_t jsSrc,HxJSON_t jsFilterValue,const HCHAR *pszKey)
{
	void			*pSrcvalue;
	HBOOL			bRet = FALSE;

	pSrcvalue = HLIB_JSON_Get(jsSrc,(void*)pszKey);
	if(pSrcvalue)
	{
		if(HxJSON_IsString(jsFilterValue) == TRUE)
		{
			const HCHAR		*pszSrcvalue,*pszFilterValue;

			pszSrcvalue = HLIB_JSON_Object_GetString(jsSrc,pszKey);
			pszFilterValue = HxJSON_String_Get(jsFilterValue);

			if(pszSrcvalue && HxSTD_StrCmp(pszSrcvalue ,pszFilterValue) == 0)
				bRet = TRUE;
		}
		else if(HxJSON_IsInteger(jsFilterValue) == TRUE)
		{
			HINT32	nSrcValue,nFilterValue;

			nSrcValue = HLIB_JSON_Object_GetInteger(jsSrc,pszKey);
			nFilterValue = HxJSON_Integer_Get(jsFilterValue);
			if(nSrcValue == nFilterValue)
				bRet = TRUE;
		}
		else if(HxJSON_IsBoolean(jsFilterValue) == TRUE)
		{
			HBOOL 	bSrcValue,bFilterValue;

			bSrcValue = HLIB_JSON_Object_GetBoolean(jsSrc,pszKey);
			bFilterValue = HxJSON_Boolean_Get(jsFilterValue);
			if(bSrcValue == bFilterValue)
				bRet = TRUE;
		}
	}

	return bRet;
}


static HBOOL trauma_statistics_checkJsonObject(HCHAR *pszSrc,HCHAR *pszFilter)
{
	HxJSON_t 		jsSrc,jsFilter,jsFilterValue;
	void			*iter;
	const HCHAR		*pszKey;
	HBOOL			bRet = FALSE;

	jsSrc = HLIB_JSON_Decode(pszSrc);
	if(jsSrc==NULL)
	{
		HxLOG_Assert("HLIB_JSON_Decode Failed...");
		return FALSE;
	}

	jsFilter = HLIB_JSON_Decode(pszFilter);
	if(jsFilter==NULL)
	{
		HxLOG_Assert("HLIB_JSON_Decode Failed...");
		return FALSE;
	}

	for ( iter = HLIB_JSON_Object_Iterator(jsFilter); iter	; iter = HLIB_JSON_Object_NextIterator(jsFilter, iter) )
	{
		pszKey  = HLIB_JSON_Object_IteratorKey(iter);
		jsFilterValue  = HLIB_JSON_Object_IteratorValue(iter);
		bRet = trauma_statistics_IsSameJsonValue(jsSrc,jsFilterValue,pszKey);
		if(bRet == FALSE)
		{
			break;
		}

	}

	HLIB_JSON_Delete(jsSrc);
	HLIB_JSON_Delete(jsFilter);

	return bRet;
}

static HBOOL trauma_statistics_logfilter(HCHAR *pLog)
{
	HBOOL 		bRetBlock = FALSE;
	HBOOL 		bActive,bAcceptPrivacy;
	HxList_t	*pList = NULL,*pRetList = NULL;

	bActive = TRAUMA_TVPORTAL_GetLogFilterActiveStatus();
	bAcceptPrivacy =TRAUMA_TVPORTAL_GetAcceptPrivacy();

	if(bAcceptPrivacy == FALSE) // Not Accept Privacy..
		return TRUE;

	if(bActive)
	{
		pRetList = TRAUMA_TVPORTAL_GetLogFilterDenyList(); //Get Deny List..
	}
	else
	{
		pRetList = TRAUMA_TVPORTAL_GetLogFilterAllowList(); // Get Allow List..
		if(pRetList == NULL)
			bRetBlock = TRUE;
	}
	pList = pRetList;
	while(pList)
	{
		HCHAR *pData;

		pData = HLIB_LIST_Data(pList);
		if(bActive == TRUE)
		{
			bRetBlock = FALSE;
			if(trauma_statistics_checkJsonObject(pLog,pData) == TRUE)	//Is Deny List??
			{
				bRetBlock = TRUE;
				break;
			}
		}
		else
		{
			bRetBlock = TRUE;
			if(trauma_statistics_checkJsonObject(pLog,pData) == TRUE)	//Is Allow List??
			{
				bRetBlock = FALSE;
				break;
			}
		}
		pList = HLIB_LIST_Next(pList);
	}
	HLIB_LIST_RemoveAllFunc(pRetList,HLIB_MEM_Free_CB);

	return bRetBlock;
}

static HCHAR* trauma_statistics_makebody(HxList_t *pItemList)
{
	HINT32 		nStream =0,i=0;
	HxList_t	*pList = pItemList;
	HCHAR		*pRetBody = NULL;

	if(HLIB_LIST_Length(pItemList) == 0)
		return NULL;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"[");

	while(pList)
	{
		HCHAR *pszData = (HCHAR*)HLIB_LIST_Data(pList);

		if(HxSTD_StrEmpty(pszData) == FALSE)
		{
			if(trauma_statistics_logfilter(pszData) == FALSE)
			{
				HxLOG_Debug(HxANSI_COLOR_YELLOW("ALLOW)%s\n"),pszData);
				if(i++ > 0)
					HLIB_RWSTREAM_Print(nStream,",");
				HLIB_RWSTREAM_Print(nStream,"%s",pszData);
			}
			else
			{
				HxLOG_Debug(HxANSI_COLOR_YELLOW("DENY)%s\n"),pszData);
			}
		}
		pList = HLIB_LIST_Next(pList);
	}
	HLIB_RWSTREAM_Print(nStream,"]");

	pRetBody= HLIB_RWSTREAM_GetBuf(nStream);
	HLIB_RWSTREAM_CloseWithoutBuf(nStream);

	if(i==0) // empty list..
	{
		HLIB_MEM_Free(pRetBody);
		return NULL;
	}
	return pRetBody;
}

static void trauma_statistics_add(HCHAR *pszLog)
{
	HxSEMT_Get(s_ulSemId);

#ifdef CONFIG_TRAUMA_STATISTICS
	s_pStatisticsLogs = HLIB_LIST_Append(s_pStatisticsLogs,pszLog);
#else
	HLIB_MEM_Free(pszLog);
#endif
	HxSEMT_Release(s_ulSemId);
}

static void trauma_statistics_report(void)
{
	HCHAR *pszReport;
	HxSEMT_Get(s_ulSemId);

	if(HLIB_LIST_Length(s_pStatisticsLogs) > 0)
	{
		pszReport = trauma_statistics_makebody(s_pStatisticsLogs);
		HxLOG_Debug("\n%s\n",pszReport);

		TRAUMA_TVPORTAL_StatisticsUpload(pszReport);
		s_pStatisticsLogs = HLIB_LIST_RemoveAllFunc(s_pStatisticsLogs,HLIB_MEM_Free_CB);
		s_pStatisticsLogs = NULL;
	}
	HxSEMT_Release(s_ulSemId);
}

static void trauma_statistics_set_AcceptPrivacy(TRAUMA_MSG_t *pstMsgData)
{
	HBOOL 	bAcceptPrivacy;

	bAcceptPrivacy 	= pstMsgData->param1;
	TRAUMA_TVPORTAL_SetAcceptPrivacy(bAcceptPrivacy);
}


static void trauma_statistics_code_setinfo(void)
{
	HINT32 					nStream =0,nDescStream = 0;
	HINT32					nInfiDispTimeout = 0, nFont =0;
	HINT32 					nDMSActive = 0,nSambaActive = 0,nFTPActive = 0 ,nIPEPGOnOff = 0, nRemoteProgram = 0,nAutoStandby = 0;
	HINT32					nStartPadding = 0, nEndPadding = 0;
	HINT32					nSeriesPlay = 0, nInstantreplay = 0,nSkipforward = 0,nPowerOnTimerVolume = 0;

	HUINT32					ulLipSyncDelay;
	HBOOL					bPowerOntimer = FALSE,bPowerOffTimer = FALSE;
	HxDATETIME_t			stStartTime,stEndTime;

	DxRatingAge_e			eRating;
	DxAspectRatio_e 		eTvAspectRatio;
	DxDfcSelect_e			eDfc;
	DxResolutionSelect_e	eResolutionSelect;
	DxDigitalAudioFormat_e	eSpdifOut,eHdmiOut;
	DxStereoSelect_e		eStereoSelect;
	DxRsvRepeat_e			eRepeat;
	DxAntInfo_t				*pstAntInfo =NULL;
	DxSchedule_t			stSchedule;

	APK_POWER_BoxStandbyType_e	ePowerMode;

	HCHAR					szLang639[8] ={0,};
	HCHAR					pszStartTime[24] = {0,},pszEndTime[24] = {0,};
	HCHAR					pszRpPeriodStartTime[24] = {0,},pszRpPeriodEndTime[24] = {0,};
	HCHAR					szPowerOnTimerChannel[DxNAME_LEN] ={0,};
	HCHAR					pszMac[APKD_NETWORK_MAX_MACADDR] ={0,};
	HCHAR					szPowerOffTimerTime[32] ={0,};
	HCHAR					*pszLog = NULL;
	const HCHAR 			*pszLanguage,*pszAudio,*pszSublang;


	(void)APK_SCENARIO_GetMaturityRating(&eRating);
	(void)APK_SCENARIO_GetMainLanguage(szLang639);
	pszLanguage = HLIB_LANG_IdTo639(HLIB_LANG_639ToId(szLang639));

	(void)APK_SCENARIO_GetAudioLanguage(0,szLang639);
	pszAudio = HLIB_LANG_IdTo639(HLIB_LANG_639ToId(szLang639));

	(void)APK_SCENARIO_GetSubtitleLanguage(0,szLang639);
	pszSublang = HLIB_LANG_IdTo639(HLIB_LANG_639ToId(szLang639));

	(void)APK_SCENARIO_GetStartPadding(&nStartPadding);
	(void)APK_SCENARIO_GetEndPadding(&nEndPadding);

	(void)APK_OUTPUT_VIDEO_GetTvAspectRatio(&eTvAspectRatio);
	(void)APK_OUTPUT_VIDEO_GetDfcSelect(&eDfc);
	(void)APK_OUTPUT_VIDEO_GetResolutionSelect(&eResolutionSelect);

	(void)APK_OUTPUT_AUDIO_GetSpdifFormat(&eSpdifOut);
	(void)APK_OUTPUT_AUDIO_GetHdmiFormat(&eHdmiOut);
	(void)APK_OUTPUT_AUDIO_GetStereoSelect(&eStereoSelect);
	(void)APK_OUTPUT_AUDIO_GetLipSyncDelay(&ulLipSyncDelay);

	(void)APK_POWER_GetStandbyPowerMode(&ePowerMode);

	(void)APK_DB_GetInt(USERDB_SCREENDISP_INFO_DISP_TIME,&nInfiDispTimeout);
	(void)APK_DB_GetInt(USERDB_SUBTITLE_FONT,&nFont);
	(void)APK_DB_GetInt(USERDB_DLNA_DMS_ACTIVATION,&nDMSActive);
	(void)APK_DB_GetInt(USERDB_SAMBASERVER_ACTIVATION,&nSambaActive);
	(void)APK_DB_GetInt(USERDB_FTPSERVER_ACTIVATION,&nFTPActive);
	(void)APK_DB_GetInt(USERDB_IPEPG_ONOFF,&nIPEPGOnOff);
	(void)APK_DB_GetInt(USERDB_SERIES_PLAYBACK,&nSeriesPlay);
	(void)APK_DB_GetInt(USERDB_RECORDING_INSTANT_REPLAY_TIME,&nInstantreplay);
	(void)APK_DB_GetInt(USERDB_RECORDING_SKIP_FORWARD_TIME,&nSkipforward);
 	(void)APK_DB_GetInt(USERDB_SYSTEM_AUTOMATIC_POWER_DOWN, &nAutoStandby);

	if(APK_META_ANTENNA_Count() > 0)
		pstAntInfo = APK_META_ANTENNA_GetInfo(APK_META_ANTENNA_GetUidAt(0));

	HxSTD_MemSet(&stSchedule,0x0,sizeof(DxSchedule_t));
	if(SAPI_GetScheduleByType(DxRSVTYPE_ACTIVATION_TIME,0,&stSchedule) == ERR_OK)
	{
		HUINT32	ulStartTime = 0 ,ulDuration = 0;

		stStartTime =	stSchedule.stStartTime;
		ulDuration = stSchedule.ulDuration;

		HxSTD_snprintf(pszStartTime,24,"%02d:%02d",stStartTime.stTime.ucHour,stStartTime.stTime.ucMinute);
		(void)HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartTime,&ulStartTime);
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime + ulDuration, &stStartTime);
		HxSTD_snprintf(pszEndTime,24,"%02d:%02d",stStartTime.stTime.ucHour,stStartTime.stTime.ucMinute);
	}
	else
	{
		HxSTD_snprintf(pszStartTime,24,"Off");
		HxSTD_snprintf(pszEndTime,24,"Off");
	}

	nRemoteProgram = TRAUMA_GetRemoteInfo(&stStartTime,&stEndTime);
	if(nRemoteProgram == TRUE)
	{
		HxSTD_snprintf(pszRpPeriodStartTime,24,"%02d:%02d",stStartTime.stTime.ucHour,stStartTime.stTime.ucMinute);
		HxSTD_snprintf(pszRpPeriodEndTime,24,"%02d:%02d",stEndTime.stTime.ucHour,stEndTime.stTime.ucMinute);
	}

	bPowerOntimer = TRAUMA_GetPowerOnTimerInfo(szPowerOnTimerChannel,&nPowerOnTimerVolume,&eRepeat);
	HxSTD_MemSet(&stSchedule,0x0,sizeof(DxSchedule_t));
	if(SAPI_GetScheduleByType(DxRSVTYPE_POWER_OFF,0,&stSchedule) == ERR_OK)
	{
		stStartTime =	stSchedule.stStartTime;
		HxSTD_snprintf(szPowerOffTimerTime,24,"%02d:%02d",stStartTime.stTime.ucHour,stStartTime.stTime.ucMinute);
		bPowerOffTimer = TRUE;
	}

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);

	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"SETINFO\",");
	HLIB_RWSTREAM_Print(nStream,"\"MATURITY\":%d,",eRating);
	if(HxSTD_StrEmpty(pszLanguage)== FALSE)
		HLIB_RWSTREAM_Print(nStream,"\"SYS_LANG\":\"%s\",",pszLanguage);
	if(HxSTD_StrEmpty(pszAudio)== FALSE)
		HLIB_RWSTREAM_Print(nStream,"\"AUD_LANG\":\"%s\",",pszAudio);
	if(HxSTD_StrEmpty(pszSublang)== FALSE)
		HLIB_RWSTREAM_Print(nStream,"\"SUB_LANG\":\"%s\",",pszSublang);
	HLIB_RWSTREAM_Print(nStream,"\"SCREEN_RATIO\":\"%s\",",TRAUMA_TOOL_AspectRatioConvertString(eTvAspectRatio));
	HLIB_RWSTREAM_Print(nStream,"\"DIS_FORMAT\":\"%s\",",TRAUMA_TOOL_DFCConvertString(eDfc));
	HLIB_RWSTREAM_Print(nStream,"\"RESOLUTION\":\"%s\",",TRAUMA_TOOL_ResolutionConvertString(eResolutionSelect));
	HLIB_RWSTREAM_Print(nStream,"\"S/PDIF\":\"%s\",",TRAUMA_TOOL_DigitalAudioFormatConvertString(eSpdifOut));
	HLIB_RWSTREAM_Print(nStream,"\"HDMI\":\"%s\",",TRAUMA_TOOL_DigitalAudioFormatConvertString(eHdmiOut));
	HLIB_RWSTREAM_Print(nStream,"\"AUD_TYPE\":\"%s\",",TRAUMA_TOOL_SoundModeConvertString(eStereoSelect));
	HLIB_RWSTREAM_Print(nStream,"\"LIP-SYNC_DELAY\":%d,",ulLipSyncDelay);
	HLIB_RWSTREAM_Print(nStream,"\"INFO_DISPLAY_TIME\":%d,",nInfiDispTimeout);
	HLIB_RWSTREAM_Print(nStream,"\"SUBTITLE\":%d,",nFont);
	HLIB_RWSTREAM_Print(nStream,"\"NETWORK_SERVER\":%s,",TRAUMA_TOOL_BoolConvertString(nDMSActive));
	HLIB_RWSTREAM_Print(nStream,"\"SERVER_ACT_TIME_START\":\"%s\",",pszStartTime);
	HLIB_RWSTREAM_Print(nStream,"\"SERVER_ACT_TIME_END\":\"%s\",",pszEndTime);
	HLIB_RWSTREAM_Print(nStream,"\"SAMBA_SERVER\":%s,",TRAUMA_TOOL_BoolConvertString(nSambaActive));
	HLIB_RWSTREAM_Print(nStream,"\"FTP_SERVER\":%s,",TRAUMA_TOOL_BoolConvertString(nFTPActive));
	HLIB_RWSTREAM_Print(nStream,"\"IP_EPG\":%s,",TRAUMA_TOOL_BoolConvertString(nIPEPGOnOff));
	HLIB_RWSTREAM_Print(nStream,"\"REMOTE_REC\":%s,",TRAUMA_TOOL_BoolConvertString(nRemoteProgram));
	if(nRemoteProgram == TRUE)
	{
		HLIB_RWSTREAM_Print(nStream,"\"REC_UPDATE_PERIOD_START\":\"%s\",",pszRpPeriodStartTime);
		HLIB_RWSTREAM_Print(nStream,"\"REC_UPDATE_PERIOD_END\":\"%s\",",pszRpPeriodEndTime);
	}
	HLIB_RWSTREAM_Print(nStream,"\"START_PADDING_TIME\":%d,",nStartPadding > 0 ? nStartPadding / 60 : 0);
	HLIB_RWSTREAM_Print(nStream,"\"END_PADDING_TIME\":%d,",nEndPadding > 0 ? nEndPadding / 60 : 0);
	HLIB_RWSTREAM_Print(nStream,"\"SERIES_PLAY\":%s,",TRAUMA_TOOL_BoolConvertString(nSeriesPlay));
	HLIB_RWSTREAM_Print(nStream,"\"INSTANT_REPLAY_TIME\":%d,",nInstantreplay);
	HLIB_RWSTREAM_Print(nStream,"\"SKIP_FORWARD_TIME\":%d,",nSkipforward);
	HLIB_RWSTREAM_Print(nStream,"\"POWER_ON_TIME\":%s,",TRAUMA_TOOL_BoolConvertString(bPowerOntimer));
	if(bPowerOntimer == TRUE)
	{
		HLIB_RWSTREAM_Print(nStream,"\"POWER_ON_TIME_CH\":\"%s\",",szPowerOnTimerChannel);
		HLIB_RWSTREAM_Print(nStream,"\"POWER_ON_TIME_VOL\":%d,",nPowerOnTimerVolume);
		HLIB_RWSTREAM_Print(nStream,"\"POWER_ON_TIME_REPEAT\":\"%s\",",TRAUMA_TOOL_RepeatConvertString(eRepeat));
	}
	if(bPowerOffTimer == TRUE)
		HLIB_RWSTREAM_Print(nStream,"\"POWER_OFF_TIME\":\"%s\",",szPowerOffTimerTime);
	HLIB_RWSTREAM_Print(nStream,"\"AUTO_POWER_DOWN\":%s,",TRAUMA_TOOL_BoolConvertString(nAutoStandby));
	HLIB_RWSTREAM_Print(nStream,"\"POWER_SAVING_IN_STANDBY\":%d,",ePowerMode);
	HLIB_RWSTREAM_Print(nStream,"\"PRIVACY_POLICY\":%s",TRAUMA_TOOL_BoolConvertString(TRAUMA_TVPORTAL_GetAcceptPrivacy()));

	if(TRAUMA_GetWLanMac(pszMac) == TRUE)
		HLIB_RWSTREAM_Print(nStream,",\"WLAN_MAC\":\"%s\"",pszMac);
	if(pstAntInfo)
		HLIB_RWSTREAM_Print(nStream,",\"SWITCH_TYPE\":\"%s\"",TRAUMA_TOOL_AntennaTypeConvertString(pstAntInfo->antType));

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_sstart(void)
{
	HINT32 				nStream =0,nDescStream = 0;
	HCHAR				szSWVer[64] ={0,},szLoaderVer[64] ={0,},szModelname[64]={0,};
	HCHAR				szSn[64]={0,},szUpdate[64]={0,};
	DxWakeUpReason_e	eWakeup;
	HCHAR				*pszLog = NULL;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);

	(void)APK_SYSINFO_STB_GetSoftwareVersion(szSWVer, 64); // is string... "SASNAA 0.00.01"
	(void)APK_SYSINFO_STB_GetLoaderVersion(szLoaderVer,64);
	(void)APK_SYSINFO_STB_GetModelName(szModelname,64);
	(void)APK_SYSINFO_STB_GetSerialNumber(szSn,64);
	(void)APK_SYSINFO_STB_GetUpdateDate(szUpdate,64);
	(void)APK_POWER_GetWakeupReason(&eWakeup);

	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"SSTART\",");
	HLIB_RWSTREAM_Print(nStream,"\"SW_VER\":\"%s\",",szSWVer);
	HLIB_RWSTREAM_Print(nStream,"\"LOADER_VER\":\"%s\",",szLoaderVer);
	HLIB_RWSTREAM_Print(nStream,"\"MODEL_NAME\":\"%s\",",szModelname);
	HLIB_RWSTREAM_Print(nStream,"\"SN\":\"%s\",",szSn);
	HLIB_RWSTREAM_Print(nStream,"\"UPDATE_DATE\":\"%s\",",szUpdate);

	switch(eWakeup)
	{
	case eDxWAKEUP_KEY:
		HLIB_RWSTREAM_Print(nStream,"\"WAKEUP\":\"BY_KEY\"");
		break;
	case eDxWAKEUP_TIMER:
		HLIB_RWSTREAM_Print(nStream,"\"WAKEUP\":\"BY_TIMER\"");
		break;
	case eDxWAKEUP_ACPOWER:
		HLIB_RWSTREAM_Print(nStream,"\"WAKEUP\":\"BY_ACPOWER\"");
		break;
	case eDxWAKEUP_HDMICEC:
		HLIB_RWSTREAM_Print(nStream,"\"WAKEUP\":\"BY_HDMICEC\"");
		break;
	case eDxWAKEUP_REBOOT:
		HLIB_RWSTREAM_Print(nStream,"\"WAKEUP\":\"BY_REBOOT\"");
		break;
	default:
		break;
	}

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_sstand(void)
{
	HINT32 	nStream = 0,nDescStream = 0;
	HCHAR	*pszLog = NULL;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"SSTAND\"");

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_salive(void)
{
	HCHAR 		szCCID[128] ={0,};
	HINT32		uid=0,onid=0,tsid=0,sid=0,eid =0;
	HINT32 		nStream = 0,nDescStream = 0;
	HCHAR		*pszLog = NULL,*pszEventName = NULL;
	HxVector_t 	*pList = NULL;
	DxEvent_t	*pEvent= NULL;
	DxService_t	*pService = NULL;

	if (APK_DB_GetStr(USERDB_SVC_LAST_CHANNEL, szCCID, 128) != ERR_OK)
	{
		HxLOG_Critical("\n[%s.%d] Not Found Last Channel inform!!!\n",__FUNCTION__,__LINE__);
		return;
	}
	sscanf(szCCID,"ccid:%d.%d.%d.%d",&uid, &onid, &tsid, &sid);

	pList = APK_META_SVC_FindServiceBySvcId(sid);
	pService = HLIB_VECTOR_First(pList);

	if(pService)
	{
		pList = APK_META_QUERY_GetPF(pService->uid);
		if(pList && HLIB_VECTOR_Length(pList) > 0)
		{
			pEvent = HLIB_VECTOR_First(pList);
			eid 			= pEvent->eventId;
			pszEventName 	= HLIB_STD_StrDup(((DxEventString_t *)HLIB_VECTOR_First(pEvent->name))->string);
			HLIB_VECTOR_Delete(pList);
		}
	}

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"SALIVE\",");
	HLIB_RWSTREAM_Print(nStream,"\"ONID\":%d,",onid);
	HLIB_RWSTREAM_Print(nStream,"\"TSID\":%d,",tsid);
	HLIB_RWSTREAM_Print(nStream,"\"SID\":%d,",sid);
	HLIB_RWSTREAM_Print(nStream,"\"EID\":%d,",eid);
	HLIB_RWSTREAM_Print(nStream,"\"CHNAME\":\"%s\",",pService ? pService->name :  "null");
	HLIB_RWSTREAM_Print(nStream,"\"EVTNAME\":\"%s\"",HxSTD_StrEmpty(pszEventName) ==TRUE? "null" : pszEventName);

	if(pszEventName)
		HLIB_MEM_Free(pszEventName);

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_ki(TRAUMA_MSG_t *pstMsgData)
{
	HINT32		nSymbol,nRetSymbol=0;
	HINT32 		nStream = 0,nDescStream = 0;
	HCHAR		*pszLog = NULL;

	nSymbol = pstMsgData->param1;
	nRetSymbol = TRAUMA_TOOL_DFBKeyConvertGogiKey(nSymbol);

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"KI\",");
	HLIB_RWSTREAM_Print(nStream,"\"KEY\":%d",nRetSymbol);

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_vol(TRAUMA_MSG_t *pstMsgData)
{
	HUINT32		uVolume;
	HINT32 		nStream = 0,nDescStream = 0;
	HCHAR		*pszLog = NULL;

	uVolume = pstMsgData->param1;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"VOL\",");
	HLIB_RWSTREAM_Print(nStream,"\"VOLUME\":%d",uVolume);

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_usb(void)
{
	HUINT32 			ulListCount = 0,i,index;
	HINT32 				nStream = 0,nDescStream = 0;
	HCHAR				*pszLog = NULL;
	DxStorage_Info_t 	*pDevinfoList = NULL,*pDevinfo = NULL;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"SUSB\"");

	if(APK_FILEMGR_STORAGE_GetList(eDxSTORAGE_FILTER_Mem,&pDevinfoList,&ulListCount) == ERR_OK)
	{
		if(ulListCount > 0)
		{
			for(i = 0, index=1 ; i < ulListCount ; i++,index++)
			{
				HFLOAT32 		fGbSize;

				pDevinfo = &pDevinfoList[i];
				fGbSize	= pDevinfo->ullTotalSizeKb / (1024.0f * 1024.0f);
				HLIB_RWSTREAM_Print(nStream,",\"USB%d\":\"%s\"",index,pDevinfo->szLabel);
				HLIB_RWSTREAM_Print(nStream,",\"USB%d_AMOUNT\":\"%.1fGB\"",index,fGbSize);
				//HLIB_RWSTREAM_Print(nStream,",\"USB%d_LOCATION\":\"%s\"",index,????); // unknown.....rear or front
			}
		}
		APK_FILEMGR_STORAGE_FreeList(pDevinfoList, ulListCount);
	}
	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_hdmi(void)
{
	HINT32 				nStream = 0,nDescStream = 0;
	HCHAR				*pszLog = NULL,szEdid[512] ={0,};
	HxJSON_t			jsonRoot,jsonitem;

	if(APK_OUTPUT_HDMI_GetEdidInfo(szEdid) == ERR_OK)
	{
		jsonRoot = HLIB_JSON_Decode(szEdid);
		if(jsonRoot == NULL)
			return;

		jsonitem = HLIB_JSON_Object_GetObject(jsonRoot,"HDMI_EDID_INFO");
		if(jsonitem == NULL)
		{
			HLIB_JSON_Delete(jsonRoot);
			return;
		}
		nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
		HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"SHDMI\",");
		HLIB_RWSTREAM_Print(nStream,"\"VENDOR_ID\":\"%s\",",HLIB_JSON_Object_GetString(jsonitem,"VENDOR_ID"));
		HLIB_RWSTREAM_Print(nStream,"\"PRODUCT_ID\":\"%s\",",HLIB_JSON_Object_GetString(jsonitem,"PRODUCT_ID"));
		HLIB_RWSTREAM_Print(nStream,"\"SERIAL_NUM\":%d,",HLIB_JSON_Object_GetInteger(jsonitem,"SERIAL_NUM"));
		HLIB_RWSTREAM_Print(nStream,"\"MONITOR_NAME\":\"%s\",",HLIB_JSON_Object_GetString(jsonitem,"MONITOR_NAME"));
		HLIB_RWSTREAM_Print(nStream,"\"MANUF_WEEK\":%d,",HLIB_JSON_Object_GetInteger(jsonitem,"MANUF_WEEK"));
		HLIB_RWSTREAM_Print(nStream,"\"MANUF_YEAR\":%d,",HLIB_JSON_Object_GetInteger(jsonitem,"MANUF_YEAR"));
		HLIB_RWSTREAM_Print(nStream,"\"EDID_VERSION\":%d,",HLIB_JSON_Object_GetInteger(jsonitem,"EDID_VERSION"));
		HLIB_RWSTREAM_Print(nStream,"\"EDID_REVISION\":%d,",HLIB_JSON_Object_GetString(jsonitem,"EDID_REVISION"));
		HLIB_RWSTREAM_Print(nStream,"\"FIRST_VIDEO_ID\":%d",HLIB_JSON_Object_GetInteger(jsonitem,"FIRST_VIDEO_ID"));
		HLIB_JSON_Delete(jsonRoot);

		nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
		HLIB_RWSTREAM_Close(nStream);
		pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

		trauma_statistics_add(pszLog);
	}
}

static void trauma_statistics_code_ersys(TRAUMA_MSG_t *pstMsgData)
{
	HINT32 	nStream = 0,nDescStream = 0;
	HINT32	nErrorCode;
	HCHAR	*pszLog = NULL;

	nErrorCode = pstMsgData->param1;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"ERSYS\",");
	HLIB_RWSTREAM_Print(nStream,"\"ECODE\":%d",nErrorCode);

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);

	if(nErrorCode == 14 ) //OPLBROADCAST_CHANNEL_ERROR_CAS_BLOCK
	{
		TRAUMA_MSG_t stMsg;

		HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
		stMsg.msg 			= eTRAUMA_MSG_STATISTICS_CODE_ERCAS;
		pstMsgData->param1 	= nErrorCode;

		TRAUMA_MSG_Send(&stMsg);
	}
}

static void trauma_statistics_code_ercas(TRAUMA_MSG_t *pstMsgData)
{
	HINT32 	nStream = 0,nDescStream = 0;
	HINT32	nErrorCode;
	HCHAR	*pszLog = NULL;

	nErrorCode = pstMsgData->param1;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"ERCAS\",");
	HLIB_RWSTREAM_Print(nStream,"\"CAS_ERROR\":%d",nErrorCode);

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}


static void trauma_statistics_code_ges(void)
{
	HINT32 	nStream = 0,nDescStream = 0;
	HCHAR	*pszLog = NULL;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"GES\"");

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_gee(TRAUMA_MSG_t *pstMsgData)
{
	HINT32 	nStream = 0,nDescStream = 0;
	HCHAR	*pszLog = NULL;
	HINT32 	nDownloadEvtCount = 0, nDownloadSvcCount = 0;

	nDownloadSvcCount = pstMsgData->param1;
	nDownloadEvtCount = pstMsgData->param2;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"GEE\",");
	HLIB_RWSTREAM_Print(nStream,"\"GET_SVC_NUM\":%d,",nDownloadSvcCount);
	HLIB_RWSTREAM_Print(nStream,"\"GET_EVT_NUM\":%d",nDownloadEvtCount);

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_ckota(TRAUMA_MSG_t *pstMsgData)
{
	HINT32 	nStream = 0,nDescStream = 0;
	HCHAR	*pszLog = NULL,*pszCurver,*szFoundVer;
	DxSwUpdate_Source_e		eSourceType;

	eSourceType= (DxSwUpdate_Source_e)pstMsgData->param1;
	pszCurver= (HCHAR*)pstMsgData->param2;
	szFoundVer= (HCHAR*)pstMsgData->param3;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"CKOTA\",");
	HLIB_RWSTREAM_Print(nStream,"\"TYPE\":\"%s\"",TRAUMA_TOOL_SourceTypeConvertString(eSourceType));
	if(HxSTD_StrEmpty(pszCurver) == FALSE)
	{
		HLIB_RWSTREAM_Print(nStream,",\"DEV_VER\":\"%s\"",pszCurver);
		HLIB_MEM_Free(pszCurver);
	}
	if(HxSTD_StrEmpty(szFoundVer) == FALSE)
	{
		HLIB_RWSTREAM_Print(nStream,",\"NEW_VER\":\"%s\"",szFoundVer);
		HLIB_MEM_Free(szFoundVer);
	}
	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_dls(void)
{
	HINT32 	nStream = 0,nDescStream = 0;
	HCHAR	*pszLog = NULL;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"DLS\"");

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_dle(TRAUMA_MSG_t *pstMsgData)
{
	HINT32 	nStream = 0,nDescStream = 0;
	HCHAR	*pszLog = NULL;
	HBOOL	bSuccess;

	bSuccess= (HBOOL)pstMsgData->param1;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"DLE\",");
	HLIB_RWSTREAM_Print(nStream,"\"SUCCESS\":%d",bSuccess);

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

//event_live_watch_start
static void trauma_statistics_code_elws(void)
{
	HCHAR 		szCCID[128] ={0,};
	HINT32		uid=0,onid=0,tsid=0,sid=0,eid =0;
	HINT32 		nStream = 0,nDescStream = 0;
	HCHAR		*pszLog = NULL,*pszEventName = NULL;
	HxVector_t 	*pList = NULL;
	DxEvent_t	*pEvent= NULL;
	DxService_t	*pService = NULL;

	if (APK_DB_GetStr(USERDB_SVC_LAST_CHANNEL, szCCID, 128) != ERR_OK)
	{
		HxLOG_Critical("\n[%s.%d] Not Found Last Channel inform!!!\n",__FUNCTION__,__LINE__);
		return;
	}
	sscanf(szCCID,"ccid:%d.%d.%d.%d",&uid, &onid, &tsid, &sid);

	pList = APK_META_SVC_FindServiceBySvcId(sid);
	pService = HLIB_VECTOR_First(pList);

	if(pService)
	{
		pList = APK_META_QUERY_GetPF(pService->uid);
		if(pList && HLIB_VECTOR_Length(pList) > 0)
		{
			pEvent = HLIB_VECTOR_First(pList);
			eid 			= pEvent->eventId;
			pszEventName 	= HLIB_STD_StrDup(((DxEventString_t *)HLIB_VECTOR_First(pEvent->name))->string);
			HLIB_VECTOR_Delete(pList);
		}
	}

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"ELWS\",");
	HLIB_RWSTREAM_Print(nStream,"\"ONID\":%d,",onid);
	HLIB_RWSTREAM_Print(nStream,"\"TSID\":%d,",tsid);
	HLIB_RWSTREAM_Print(nStream,"\"SID\":%d,",sid);
	HLIB_RWSTREAM_Print(nStream,"\"EID\":%d,",eid);
	HLIB_RWSTREAM_Print(nStream,"\"CHNAME\":\"%s\",",pService ? pService->name :  "null");
	HLIB_RWSTREAM_Print(nStream,"\"EVTNAME\":\"%s\"",HxSTD_StrEmpty(pszEventName) ==TRUE? "null" : pszEventName);

	if(pszEventName)
		HLIB_MEM_Free(pszEventName);

	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);
}

static void trauma_statistics_code_schedule(TRAUMA_MSG_t *pstMsgData)
{
	HINT32 			nStream = 0,nDescStream = 0;
	HCHAR			*pszLog = NULL;
	DxSchedule_t	*pstSchedule;
	DxService_t		*pService = NULL;
	HxVector_t 		*pList = NULL;
	HUINT16			usOnId;
	HUINT16			usTsId;
	HUINT16			usSvcId;
	DxRSV_Record_t	*pstRecord;

	pstSchedule= (DxSchedule_t*)pstMsgData->param1;
	if(pstSchedule == NULL)
		return;

	usOnId = pstSchedule->stTripleId.usOnId;
	usTsId = pstSchedule->stTripleId.usTsId;
	usSvcId = pstSchedule->stTripleId.usSvcId;
	pstRecord = &(pstSchedule->uExtInfo.stRec);

	pList = APK_META_SVC_FindServiceByTriplet(usOnId,usTsId,usSvcId);
	pService = HLIB_VECTOR_First(pList);

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);
	switch(pstMsgData->msg)
	{
		case eTRAUMA_MSG_STATISTICS_CODE_ERECS:
			HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"ERECS\",");
			break;
		case eTRAUMA_MSG_STATISTICS_CODE_ERECE:
			HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"ERECE\",");
			break;
		case eTRAUMA_MSG_STATISTICS_CODE_EREMIND:
			HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"EREMIND\",");
			break;
		case eTRAUMA_MSG_STATISTICS_CODE_ELSS:
			HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"ELSS\",");
			break;
		case eTRAUMA_MSG_STATISTICS_CODE_ELSE:
			HLIB_RWSTREAM_Print(nStream,"\"CODE\":\"ELSE\",");
			break;
		default:
			break;
	}

	HLIB_RWSTREAM_Print(nStream,"\"ONID\":%d,",usOnId);
	HLIB_RWSTREAM_Print(nStream,"\"TSID\":%d,",usTsId);
	HLIB_RWSTREAM_Print(nStream,"\"SID\":%d",usSvcId);
	if(pstMsgData->msg != eTRAUMA_MSG_STATISTICS_CODE_EREMIND && pstMsgData->msg != eTRAUMA_MSG_STATISTICS_CODE_ELSE )
	{
		HLIB_RWSTREAM_Print(nStream,",\"EID\":%d",pstRecord->nEvtId);
		HLIB_RWSTREAM_Print(nStream,",\"CHNAME\":\"%s\"",pService ? pService->name :  "null");
		HLIB_RWSTREAM_Print(nStream,",\"EVTNAME\":\"%s\"",pstRecord->szEventName);
	}
	nDescStream = TRAUMA_STATISTICS_MakeHeader(HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);
	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);

	trauma_statistics_add(pszLog);

	HLIB_MEM_Free(pstSchedule);
}

static void trauma_statistics_Log(TRAUMA_MSG_t *pstMsgData)
{
	HCHAR 		*pszLog = (HCHAR*)pstMsgData->param1;
	HINT32 		nDescStream = 0;

	nDescStream = TRAUMA_STATISTICS_MakeHeader(pszLog);
	HLIB_MEM_Free(pszLog);

	pszLog = HLIB_RWSTREAM_CloseWithoutBuf(nDescStream);
	HxLOG_Debug("RECV LOG[%s]\n",pszLog);

	trauma_statistics_add(pszLog);
}


static void trauma_statistics_report_timerCallback (unsigned long ulTimerId, void *pvParam)
{
	TRAUMA_MSG_t stMsg;

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg = eTRAUMA_MSG_STATISTICS_REPORT;

	TRAUMA_MSG_Send(&stMsg);
}

static void trauma_statistics_ttl_timerCallback (unsigned long ulTimerId, void *pvParam)
{
	TRAUMA_MSG_t stMsg;

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg = eTRAUMA_MSG_STATISTICS_REFRESH_LOGFILTER;

	TRAUMA_MSG_Send(&stMsg);
}

static void trauma_statistics_setinfo_timerCallback (unsigned long ulTimerId, void *pvParam)
{
	TRAUMA_MSG_t stMsg;

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg = eTRAUMA_MSG_STATISTICS_CODE_SETINFO;

	TRAUMA_MSG_Send(&stMsg);
}

static void trauma_statistics_alive_timerCallback (unsigned long ulTimerId, void *pvParam)
{
	TRAUMA_MSG_t stMsg;

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg = eTRAUMA_MSG_STATISTICS_CODE_SALIVE;

	TRAUMA_MSG_Send(&stMsg);
}

static void trauma_statistics_ipepg_timerCallback (unsigned long ulTimerId, void *pvParam)
{
	HINT32	nIPEPGOnOff = 0;

	if(APK_DB_GetInt(USERDB_IPEPG_ONOFF,&nIPEPGOnOff) ==ERR_FAIL)
		return;
	if(nIPEPGOnOff == TRUE)
	{
		static HBOOL 	s_bStart = FALSE, s_bEnd = FALSE;
		HINT32 			nDownloadProgress = 0;
		HINT32 			nDownloadEvtCount = 0, nDownloadSvcCount = 0;
		TRAUMA_MSG_t 	stMsg;

		HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
		if(APK_DB_GetInt(USERDB_IPEPG_DOWNLOADPROGRESS,&nDownloadProgress) == ERR_FAIL)
			return;
		if((s_bStart == FALSE) && (nDownloadProgress > 0 || nDownloadProgress < 100))
		{
			s_bStart = TRUE;
			s_bEnd = FALSE;
			stMsg.msg = eTRAUMA_MSG_STATISTICS_CODE_GES;
			TRAUMA_MSG_Send(&stMsg);
		}
		else if(s_bEnd == FALSE && nDownloadProgress == 100)
		{
			s_bStart = FALSE;
			s_bEnd = TRUE;
			(void)APK_DB_GetInt(USERDB_IPEPG_SVC_COUNT, &nDownloadSvcCount);
			(void)APK_DB_GetInt(USERDB_IPEPG_EVT_COUNT, &nDownloadEvtCount);

			stMsg.msg = eTRAUMA_MSG_STATISTICS_CODE_GEE;
			stMsg.param1 = nDownloadSvcCount;
			stMsg.param2 = nDownloadEvtCount;
			TRAUMA_MSG_Send(&stMsg);
		}
	}
}

static void trauma_statistics_checklogfilter(void)
{
	HUINT32 unInterval,unttl;

	unInterval = TRAUMA_TVPORTAL_GetLogFilterInterval();
	unInterval *= 1000;

	unttl = TRAUMA_TVPORTAL_GetLogFilterTTL();
	unttl *= 1000;

	if(s_ulInternval_timer && unInterval != 0)
	{
		VK_TIMER_Cancel(s_ulInternval_timer);
		HxLOG_Debug("Cancel interval-timer\n");
	}

	HxLOG_Debug("Change interval: %d \n",unInterval);
	(void)VK_TIMER_EventEvery(unInterval, trauma_statistics_report_timerCallback ,NULL,0,&s_ulInternval_timer);

	if(s_ulttl_timer && unttl != 0)
	{
		VK_TIMER_Cancel(s_ulttl_timer);
		HxLOG_Debug("Cancel ttl-timer\n");
	}

	HxLOG_Debug("Change ttl: %d \n",unttl);
	(void)VK_TIMER_EventEvery(unttl, trauma_statistics_ttl_timerCallback ,NULL,0,&s_ulttl_timer);

}

static void trauma_statistics_refresh_logfilter(void)
{
	HxLOG_Debug("TRAUMA_TVPORTAL_GetLogFilter !! \n");
	TRAUMA_TVPORTAL_GetLogFilter();
}

static void trauma_statistics_start(void)
{
	TRAUMA_MSG_t stMsg;

	HxSTD_MemSet(&stMsg,0x0,sizeof(TRAUMA_MSG_t));
	stMsg.msg = eTRAUMA_MSG_STATISTICS_CODE_SETINFO;
	TRAUMA_MSG_Send(&stMsg);
}

/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
void TRAUMA_STATISTICS_Init(void)
{
	HxSEMT_Create(&s_ulSemId, "trauma_statistics_sem", HxSEMT_FIFO);
}

void TRAUMA_STATISTICS_Start(void)
{
	static HULONG 		s_ulTimerid = 0;
	if(s_ulTimerid == 0)
	{
		(void)VK_TIMER_EventEvery(TRAUMA_STATISTICS_REPORT_INTERVAL, trauma_statistics_report_timerCallback ,NULL,0,&s_ulInternval_timer);
		(void)VK_TIMER_EventEvery(TRAUMA_STATISTICS_CODE_SETINFO_INTERVAL, trauma_statistics_setinfo_timerCallback ,NULL,0,&s_ulTimerid);
		(void)VK_TIMER_EventEvery(TRAUMA_STATISTICS_CODE_SALIVE_INTERVAL, trauma_statistics_alive_timerCallback ,NULL,0,&s_ulTimerid);
		(void)VK_TIMER_EventEvery(TRAUMA_STATISTICS_CODE_IPEPG_INTERVAL, trauma_statistics_ipepg_timerCallback ,NULL,0,&s_ulTimerid);

		trauma_statistics_start();// send to system report when operate system
	}
}

void TRAUMA_STATISTICS_Process(TRAUMA_MSG_t *pstMsgData)
{
	switch(pstMsgData->msg)
	{
	case eTRAUMA_MSG_STATISTICS_REPORT:
		trauma_statistics_report();
		break;
	case eTRAUMA_MSG_STATISTICS_SET_ACCEPT_PRIVACY:
		trauma_statistics_set_AcceptPrivacy(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_LOG:
		trauma_statistics_Log(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_SETINFO:
		trauma_statistics_code_setinfo();
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_SSTART:
		trauma_statistics_code_sstart();
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_SSTAND:
		trauma_statistics_code_sstand();
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_SALIVE:
		trauma_statistics_code_salive();
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_KI:
		trauma_statistics_code_ki(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_VOL:
		trauma_statistics_code_vol(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_SUSB:
		trauma_statistics_code_usb();
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_SHDMI:
		trauma_statistics_code_hdmi();
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_ERSYS:
		trauma_statistics_code_ersys(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_ERCAS:
		trauma_statistics_code_ercas(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_GES:
		trauma_statistics_code_ges();
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_GEE:
		trauma_statistics_code_gee(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_CKOTA:
		trauma_statistics_code_ckota(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_DLS:
		trauma_statistics_code_dls();
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_DLE:
		trauma_statistics_code_dle(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_ELWS:
		trauma_statistics_code_elws();
		break;
	case eTRAUMA_MSG_STATISTICS_CODE_ERECS:
	case eTRAUMA_MSG_STATISTICS_CODE_ERECE:
	case eTRAUMA_MSG_STATISTICS_CODE_EREMIND:
	case eTRAUMA_MSG_STATISTICS_CODE_ELSS:
	case eTRAUMA_MSG_STATISTICS_CODE_ELSE:
		trauma_statistics_code_schedule(pstMsgData);
		break;
	case eTRAUMA_MSG_STATISTICS_CHECK_LOGFILTER:
		trauma_statistics_checklogfilter();
		break;
	case eTRAUMA_MSG_STATISTICS_REFRESH_LOGFILTER:
		trauma_statistics_refresh_logfilter();
		break;
	default:
		HxLOG_Error("Wrong message type...[0x%x]\n",pstMsgData->msg);
		break;
	}
}

HINT32 TRAUMA_STATISTICS_MakeHeader(HCHAR *pszDesc)
{
	static HUINT32 		s_ulOrder =0;
	UNIXTIME 			uxcurtime;
	HxDATETIME_t		datatime;
	HxDATETIME_Date_t	stDate;
	HxDATETIME_Time_t	stTime;
	HINT32				nSysOffsetHour = 0, nSysOffsetMin = 0;
	HCHAR 				*pszSysId,*pszSysVer;
	HINT32 				nRetStream =0;

	uxcurtime = HLIB_STD_GetSystemTime();
	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(uxcurtime,&datatime) == ERR_FAIL)
		return nRetStream;
	if(HLIB_DATETIME_GetOffsetTime (&nSysOffsetHour, &nSysOffsetMin)==ERR_FAIL)
		return nRetStream;
	if(TRAUMA_GetSystemInfo(&pszSysId,&pszSysVer)==ERR_FAIL)
		return nRetStream;

	s_ulOrder++;
	stDate = datatime.stDate ;
	stTime = datatime.stTime ;

	nRetStream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_MEM_Malloc_CB, HLIB_MEM_Free_CB);

	HLIB_RWSTREAM_Print(nRetStream,"{");
	HLIB_RWSTREAM_Print(nRetStream,"\"TIMESTAMP\":\"%04d-%02d-%02d %02d:%02d:%02d.%03d+%02d:%02d\",",
		stDate.usYear,stDate.ucMonth,stDate.ucDay,
		stTime.ucHour,stTime.ucMinute,stTime.ucSecond,stTime.usMillisecond,
		nSysOffsetHour,nSysOffsetMin);
	HLIB_RWSTREAM_Print(nRetStream,"\"MAC\":\"%s\",",TRAUMA_GetLanMac());
	HLIB_RWSTREAM_Print(nRetStream,"\"SYSTEM_ID\":\"%s\",",pszSysId);
	HLIB_RWSTREAM_Print(nRetStream,"\"NO\":%d,",s_ulOrder);
	HLIB_RWSTREAM_Print(nRetStream,"\"IP_ADDR\":\"%s\",",TRAUMA_GetIPaddress());
#ifdef CONFIG_DEBUG
	HLIB_RWSTREAM_Print(nRetStream,"\"MODE\":\"test\",");
#else
	HLIB_RWSTREAM_Print(nRetStream,"\"MODE\":\"release\",");
#endif
	HLIB_RWSTREAM_Print(nRetStream,"%s",pszDesc); /* Write description */

	HLIB_RWSTREAM_Print(nRetStream,"}");

	return nRetStream;
}

void TRAUMA_STATISTICS_AddReport(HCHAR *pszReport)
{
	HxSEMT_Get(s_ulSemId);
	s_pStatisticsLogs = HLIB_LIST_Append(s_pStatisticsLogs,pszReport);
	HxSEMT_Release(s_ulSemId);

}

HxList_t*	TRAUMA_STATISTICS_GetLogs(void)
{
	HxList_t			*pList, *pRetList= NULL;
	HxSEMT_Get(s_ulSemId);
	pList = s_pStatisticsLogs;

	while (pList)
	{
		HCHAR *pData = (HCHAR*)HLIB_LIST_Data(pList);

		if(pData)
		{
			pRetList = HLIB_LIST_Append(pRetList,HLIB_STD_StrDup(pData));
		}
		pList = HLIB_LIST_Next(pList);
	}
	HxSEMT_Release(s_ulSemId);
	return pRetList;
}

