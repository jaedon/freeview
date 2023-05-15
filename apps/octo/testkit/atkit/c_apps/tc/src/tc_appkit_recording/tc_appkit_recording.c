/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  tc_appkit_recording.c
	@brief

	Description:  									\n
	Module: TC ATKIT		\n

	Author: Richard <nmdung@humaxdigital.com>
	Created date: Jan 2017
*/

#include "atkit.h"

static HxJSON_t s_manifest = NULL;
static HUINT32 ulGetDuration;

void getStartInfo(OxMediaRec_StartInfo_t *pstStartInfo, HxJSON_t j_StartInfo_argument, HUINT32 ulRequestId);
HBOOL TC_APPKIT_Recording_NormalCase(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_StartInfo_argument = NULL;

	HBOOL ret = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	HUINT32 ulViewId, ulRequestId, ulContentId, ulRecTime;
	OxMediaRec_StartInfo_t 	*pstStartInfo = NULL;
	HUINT32 ulRecSessionId = 0;
	HCHAR szRecordID[256];
	HBOOL bIsRecording = FALSE;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_StartInfo_argument = HLIB_JSON_Object_GetObject(j_argument, "StartInfo");

	printf("*************** TC_APPKIT_Recording_Start ***************\n");
	ERR_ret = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetMainViewId");
	ERR_ret = APK_MEDIA_PLAY_GetRequestId(ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&ulRequestId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetRequestId");

	// printf("ulViewId = %d, ulRequestId = %d\n", ulViewId, ulRequestId);
	pstStartInfo = (OxMediaRec_StartInfo_t*)HLIB_STD_MemCalloc(sizeof(OxMediaRec_StartInfo_t));
	DONE_ERR(pstStartInfo == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet( pstStartInfo, 0, sizeof(OxMediaRec_StartInfo_t) );

	getStartInfo(pstStartInfo, j_StartInfo_argument, ulRequestId);
	
	ERR_ret = APK_MEDIA_RECORD_Start (pstStartInfo, &ulRecSessionId, szRecordID);
	printf("Id of Recording Session = %d\n", ulRecSessionId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_Start");
	HLIB_STD_TaskSleep(1*1000);

	ERR_ret = APK_MEDIA_RECORD_IsRecording(pstStartInfo->ulMasterSvcUid, &bIsRecording);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_IsRecording");
	if(bIsRecording == TRUE)
	{
		printf("[%s] Recording Session is Running\n", __FUNCTION__);
		HLIB_STD_TaskSleep(ulGetDuration*1000);

		ERR_ret = APK_MEDIA_RECORD_GetRecTime(ulRecSessionId, &ulRecTime);
		DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_GetRecTime");
		printf("Get Instant Recording Time (GetRecTime): Rectime=  %d (seconds)\n", ulRecTime);
		ERR_ret = APK_MEDIA_RECORD_GetContentId(ulRecSessionId, &ulContentId);
		DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_GetContentId");
		printf("Id of Content (GetContentId) : %d\n", ulContentId);

		printf("\n*************** TC_APPKIT_Recording_Stop ***************\n");
		ERR_ret = APK_MEDIA_RECORD_StopByUID(pstStartInfo->ulMasterSvcUid);
		DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_StopByUID");
		HLIB_STD_TaskSleep(1*1000);
	}
	else
	{
		printf("[%s]: Recording is not Running\n", __FUNCTION__);
	}
	ret = TRUE;
done:
	return ret;
}

HBOOL TC_APPKIT_Recording_RepeatlyCase(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_StartInfo_argument = NULL;

	HBOOL ret = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	HUINT32 ulViewId, ulRequestId, ulContentId, ulRecTime, ulNumberLoop;
	OxMediaRec_StartInfo_t 	*pstStartInfo = NULL;
	HUINT32 ulRecSessionId = 0;
	HCHAR szRecordID[256];
	HBOOL bIsRecording = FALSE;
	HUINT32 i;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_StartInfo_argument = HLIB_JSON_Object_GetObject(j_argument, "StartInfo");
	ulNumberLoop = HLIB_JSON_Object_GetInteger(j_argument, "Number_of_loop");
	printf("Number of Recording sessions : %d\n", ulNumberLoop);

	printf("*************** TC_APPKIT_Recording_Start ***************\n");
	ERR_ret = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetMainViewId");
	ERR_ret = APK_MEDIA_PLAY_GetRequestId(ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&ulRequestId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetRequestId");

	// printf("[%s] ulViewId = %d, ulRequestId = %d\n", __FUNCTION__, ulViewId, ulRequestId);
	pstStartInfo = (OxMediaRec_StartInfo_t*)HLIB_STD_MemCalloc(sizeof(OxMediaRec_StartInfo_t));
	DONE_ERR(pstStartInfo == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet(pstStartInfo, 0, sizeof(OxMediaRec_StartInfo_t) );
	getStartInfo(pstStartInfo, j_StartInfo_argument, ulRequestId);

	for(i = 0; i < ulNumberLoop; i++)
	{
		ulRecSessionId = 0;
		HLIB_STD_StrNCpySafe(szRecordID, "", 256);

		printf("\n*************** APK_MEDIA_RECORD_Start Session %d ***************\n", (i+1));
		ERR_ret = APK_MEDIA_RECORD_Start (pstStartInfo, &ulRecSessionId, szRecordID);
		printf("[%s] [Recording Session: %d] ulRecSessionId = %d\n", __FUNCTION__, (i+1), ulRecSessionId);
		DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_Start");
		
		HLIB_STD_TaskSleep(1*1000);

		ERR_ret = APK_MEDIA_RECORD_IsRecording(pstStartInfo->ulMasterSvcUid, &bIsRecording);
		DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_IsRecording");
		if(bIsRecording == TRUE)
		{
			printf("[%s] [Recording Session: %d] Recording is Running\n", __FUNCTION__, (i+1));
			
			HLIB_STD_TaskSleep(ulGetDuration*1000);

			ERR_ret = APK_MEDIA_RECORD_GetRecTime(ulRecSessionId, &ulRecTime);
			DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_GetRecTime");
			printf("[%s] [Recording Session: %d] Get Instant Recording Time (GetRecTime): Rectime=  %d (seconds)\n",__FUNCTION__, (i+1), ulRecTime);
			ERR_ret = APK_MEDIA_RECORD_GetContentId(ulRecSessionId, &ulContentId);
			DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_GetContentId");
			printf("[%s] [Recording Session: %d] Id of Content (GetContentId) = %d\n", __FUNCTION__, (i+1), ulContentId);

			printf("\n*************** TC_APPKIT_Recording_Stop Session %d ***************\n", (i+1));
			ERR_ret = APK_MEDIA_RECORD_StopByUID(pstStartInfo->ulMasterSvcUid);
			DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_StopByUID");
			HLIB_STD_TaskSleep(1*1000);
		}
		else
		{
			printf("[%s]: Recording is not Running\n", __FUNCTION__);
			break;
		}

	}
	ret = TRUE;
done:
	return ret;
}

void getStartInfo(OxMediaRec_StartInfo_t *pstStartInfo, HxJSON_t j_StartInfo_argument, HUINT32 ulRequestId)
{
	HUINT32 	ulCurrTime = 0, ulStartTime = 0, ulRecTime = 100;

	pstStartInfo->ulMasterSvcUid = ulRequestId;
	pstStartInfo->ulSuppleSvcUid = ulRequestId;
	pstStartInfo->eSuppSvcType			= HLIB_JSON_Object_GetInteger(j_StartInfo_argument, "eSuppSvcType");
	pstStartInfo->nStartOffset			= HLIB_JSON_Object_GetInteger(j_StartInfo_argument, "nStartOffset");

	ulCurrTime = HLIB_STD_GetSystemTime();

	if (pstStartInfo->nStartOffset <= 0)
	{
		// Instant Rec ...
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurrTime, &(pstStartInfo->stStartTime));
		ulStartTime = ulCurrTime;
	}
	else
	{
		// Delayed Rec ...
		ulStartTime			= ulCurrTime - ulRecTime + pstStartInfo->nStartOffset;
		HxLOG_Debug("[%s:%d] ulRecTime(%d), nStartOffset(%d), ulStartTime(%d)\n", __FUNCTION__, __LINE__, ulRecTime, pstStartInfo->nStartOffset, ulStartTime);
		(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &(pstStartInfo->stStartTime));
	}

	ulGetDuration = HLIB_JSON_Object_GetInteger(j_StartInfo_argument, "ulDuration");

	pstStartInfo->ulDuration			= ulGetDuration + 720000;
	pstStartInfo->eStorageType			= HLIB_JSON_Object_GetInteger(j_StartInfo_argument, "eStorageType");

	pstStartInfo->nEvtId = 0;

	HLIB_STD_StrNCpySafe(pstStartInfo->szName, (HCHAR*)HLIB_JSON_Object_GetString(j_StartInfo_argument, "szName"), sizeof(pstStartInfo->szName));
}


void TC_RECORDING_Init(HxJSON_t jManifest)
{
	printf("\n*************** TC_RECORDING_Init ***************\n");
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);

	s_manifest = jManifest;
	
	APK_Init();
	APK_META_RECORD_Init();
	APK_META_SVC_Init(TRUE);
	APK_MEDIA_PLAY_Init();
	APK_MEDIA_RECORD_Init();
	APK_META_SCHEDULE_GetInstance();

	HLIB_STD_TaskSleep(2*1000);

	ADD_TC(TC_APPKIT_Recording_NormalCase);
	ADD_TC(TC_APPKIT_Recording_RepeatlyCase);
}

void TC_RECORDING_DeInit(void)
{
	printf("\n*************** TC_RECORDING_DeInit ***************\n");
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}

