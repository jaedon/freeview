/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  tc_appkit_meta_recording.c
	@brief

	Description:  									\n
	Module: TC ATKIT		\n

	Author: Richard <nmdung@humaxdigital.com>
	Created date: 10 Feb 2017
*/

#include "atkit.h"

static HxJSON_t s_manifest = NULL;
static HUINT32 ulGetDuration;

void getInfo_To_Start(OxMediaRec_StartInfo_t *pstStartInfo, HxJSON_t j_StartInfo_argument, HUINT32 ulRequestId);
DxRecListData_t *getInfo_Of_RecordingSession(void);

HBOOL TC_APPKIT_Meta_Recording_Rename(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_StartInfo_argument = NULL;
	HBOOL bRet = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	HUINT32 ulViewId, ulRequestId;

	OxMediaRec_StartInfo_t 	*pstStartInfo = NULL;
	DxRecListData_t *pstRecData = NULL;


	HCHAR szRecordID[256];
	HUINT32 ulRecSessionId = 0;

	HxJSON_t rename_argument = NULL;
	APK_META_REC_RENAME_t		stRenameRec = {0};
	APK_META_REC_STOP_REC_t	stStopRec = {0};

	printf("**************************************************************************\n");
	printf("************************** %s **************************\n", __FUNCTION__);
	printf("**************************************************************************\n");

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_StartInfo_argument = HLIB_JSON_Object_GetObject(j_argument, "StartInfo");
	
	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	APK_MEDIA_PLAY_GetRequestId(ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&ulRequestId);
	// printf("ulViewId = %d, ulRequestId = %d\n", ulViewId, ulRequestId);
	pstStartInfo = (OxMediaRec_StartInfo_t*)HLIB_STD_MemCalloc(sizeof(OxMediaRec_StartInfo_t));
	DONE_ERR(pstStartInfo == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet( pstStartInfo, 0, sizeof(OxMediaRec_StartInfo_t) );

	getInfo_To_Start(pstStartInfo, j_StartInfo_argument, ulRequestId);

	printf("*************** APPKIT_Recording_Start ***************\n");
	ERR_ret = APK_MEDIA_RECORD_Start (pstStartInfo, &ulRecSessionId, szRecordID);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_Start");
	HLIB_STD_TaskSleep(ulGetDuration*1000);

	pstRecData = (DxRecListData_t*)HLIB_STD_MemCalloc(sizeof(DxRecListData_t));
	DONE_ERR(pstRecData == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet( pstRecData, 0, sizeof(DxRecListData_t) );

	pstRecData = getInfo_Of_RecordingSession();

	printf("\n*************** Renaming the name of recording video ***************\n");
	rename_argument = HLIB_JSON_Object_GetObject(j_argument, "rename");
	stRenameRec.ulParamCount = 1;
	stRenameRec.paramList = (APK_META_REC_RENAME_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_RENAME_PARAM_t) * stRenameRec.ulParamCount);
	HxSTD_StrNCpy(stRenameRec.paramList[0].szUrl, pstRecData->szUrl, DxRECLIST_URL);
	HxSTD_StrNCpy(stRenameRec.paramList[0].szRename, (HCHAR*)HLIB_JSON_Object_GetString(rename_argument, "new_name"), DxRECLIST_NAME_LEN);
	ERR_ret = APK_META_RECORD_Rename(&stRenameRec, TRUE);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_Rename");

	printf("New name of Recording file : %s\n", HLIB_JSON_Object_GetString(rename_argument, "new_name"));
	printf("......Rename successfully the video recording......\n");
	
	HLIB_STD_TaskSleep(10*1000);
	HLIB_STD_MemFree(stRenameRec.paramList);


	printf("\n*************** Stop the recording session ***************\n");
	stStopRec.ulParamCount	= 1;
	stStopRec.paramList		= (APK_META_REC_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_PARAM_t) * stStopRec.ulParamCount);
	DONE_ERR(stStopRec.paramList == NULL, "HLIB_STD_MemCalloc");
	HxSTD_StrNCpy(stStopRec.paramList[0].szUrl, pstRecData->szUrl, DxRECLIST_URL);
	ERR_ret = APK_META_RECORD_StopRecording(&stStopRec, TRUE);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_StopRecording");
	HLIB_STD_TaskSleep(1*1000);
	HLIB_STD_MemFree(stStopRec.paramList);
	
	bRet = TRUE;
done:
	HLIB_STD_MemFree(pstStartInfo);
	HLIB_STD_MemFree(pstRecData);
	pstStartInfo = NULL;
	pstRecData = NULL;
	return bRet;
}

HBOOL TC_APPKIT_Meta_Recording_Copy(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_StartInfo_argument = NULL;

	HBOOL bRet = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	HUINT32 ulViewId, ulRequestId;

	OxMediaRec_StartInfo_t 	*pstStartInfo = NULL;
	DxRecListData_t *pstRecData = NULL;

	HCHAR szRecordID[256];
	HUINT32 ulRecSessionId = 0;
	
	APK_META_REC_STOP_REC_t	stStopRec = {0};
	HxJSON_t copy_argument = NULL;
	HCHAR szPathCopyTo[DxRECLIST_PATH+1];

	printf("**************************************************************************\n");
	printf("************************** %s **************************\n", __FUNCTION__);
	printf("**************************************************************************\n");

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_StartInfo_argument = HLIB_JSON_Object_GetObject(j_argument, "StartInfo");
	
	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	APK_MEDIA_PLAY_GetRequestId(ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&ulRequestId);
	pstStartInfo = (OxMediaRec_StartInfo_t*)HLIB_STD_MemCalloc(sizeof(OxMediaRec_StartInfo_t));
	DONE_ERR(pstStartInfo == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet( pstStartInfo, 0, sizeof(OxMediaRec_StartInfo_t) );

	getInfo_To_Start(pstStartInfo, j_StartInfo_argument, ulRequestId);

	printf("*************** APPKIT_Recording_Start ***************\n");
	ERR_ret = APK_MEDIA_RECORD_Start (pstStartInfo, &ulRecSessionId, szRecordID);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_Start");
	HLIB_STD_TaskSleep(ulGetDuration*1000);

	pstRecData = (DxRecListData_t*)HLIB_STD_MemCalloc(sizeof(DxRecListData_t));
	DONE_ERR(pstRecData == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet( pstRecData, 0, sizeof(DxRecListData_t) );

	pstRecData = getInfo_Of_RecordingSession();

	printf("\n*************** Stop the recording session ***************\n");
	stStopRec.ulParamCount	= 1;
	stStopRec.paramList		= (APK_META_REC_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_PARAM_t) * stStopRec.ulParamCount);
	DONE_ERR(stStopRec.paramList == NULL, "HLIB_STD_MemCalloc");
	HxSTD_StrNCpy(stStopRec.paramList[0].szUrl, pstRecData->szUrl, DxRECLIST_URL);
	ERR_ret = APK_META_RECORD_StopRecording(&stStopRec, TRUE);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_StopRecording");
	HLIB_STD_TaskSleep(10*1000);
	HLIB_STD_MemFree(stStopRec.paramList);

	printf("\n*************** Copying the recording video ***************\n");
	copy_argument = HLIB_JSON_Object_GetObject(j_argument, "copy");
	HxSTD_StrNCpy(szPathCopyTo, (HCHAR*)HLIB_JSON_Object_GetString(copy_argument, "PathCopyto"), DxRECLIST_PATH);
	printf("[META_RECORD_Copy] ContentId : %d\n", pstRecData->ulContentId);
	printf("[META_RECORD_Copy] szPathCopyTo : %s\n", szPathCopyTo);
	ERR_ret = APK_META_RECORD_Copy(pstRecData->ulContentId, szPathCopyTo, 0, NULL, NULL);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_Copy");
	printf("...Copying successfully...\n");
	HLIB_STD_TaskSleep(2*1000);

	bRet = TRUE;
done:
	HLIB_STD_MemFree(pstStartInfo);
	HLIB_STD_MemFree(pstRecData);
	pstStartInfo = NULL;
	pstRecData = NULL;
	return bRet;
}

HBOOL TC_APPKIT_Meta_Recording_Delete(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_StartInfo_argument = NULL;


	HBOOL bRet = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	HUINT32 ulViewId, ulRequestId;

	
	OxMediaRec_StartInfo_t 	*pstStartInfo = NULL;
	DxRecListData_t *pstRecData = NULL;


	HCHAR szRecordID[256];
	HUINT32 ulRecSessionId = 0;

	APK_META_REC_STOP_REC_t	stStopRec = {0};
	APK_META_REC_DELETE_t	stDelete	= {0};

	printf("**************************************************************************\n");
	printf("************************** %s **************************\n", __FUNCTION__);
	printf("**************************************************************************\n");

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_StartInfo_argument = HLIB_JSON_Object_GetObject(j_argument, "StartInfo");
	
	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	APK_MEDIA_PLAY_GetRequestId(ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&ulRequestId);
	pstStartInfo = (OxMediaRec_StartInfo_t*)HLIB_STD_MemCalloc(sizeof(OxMediaRec_StartInfo_t));
	DONE_ERR(pstStartInfo == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet( pstStartInfo, 0, sizeof(OxMediaRec_StartInfo_t) );

	getInfo_To_Start(pstStartInfo, j_StartInfo_argument, ulRequestId);

	printf("*************** APPKIT_Recording_Start ***************\n");
	ERR_ret = APK_MEDIA_RECORD_Start (pstStartInfo, &ulRecSessionId, szRecordID);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_Start");
	HLIB_STD_TaskSleep(ulGetDuration*1000);

	pstRecData = (DxRecListData_t*)HLIB_STD_MemCalloc(sizeof(DxRecListData_t));
	DONE_ERR(pstRecData == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet( pstRecData, 0, sizeof(DxRecListData_t) );

	pstRecData = getInfo_Of_RecordingSession();

	
	printf("\n*************** Stop the recording session ***************\n");
	stStopRec.ulParamCount	= 1;
	stStopRec.paramList		= (APK_META_REC_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_PARAM_t) * stStopRec.ulParamCount);
	DONE_ERR(stStopRec.paramList == NULL, "HLIB_STD_MemCalloc");
	HxSTD_StrNCpy(stStopRec.paramList[0].szUrl, pstRecData->szUrl, DxRECLIST_URL);
	ERR_ret = APK_META_RECORD_StopRecording(&stStopRec, TRUE);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_StopRecording");
	HLIB_STD_TaskSleep(10*1000);
	HLIB_STD_MemFree(stStopRec.paramList);

	printf("\n*************** Deleting the recording video ***************\n");
	
	stDelete.ulParamCount	= 1;
	stDelete.paramList		= (APK_META_REC_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_PARAM_t) * stDelete.ulParamCount);
	DONE_ERR(stDelete.paramList == NULL, "HLIB_STD_MemCalloc");
	HLIB_STD_StrNCpySafe(stDelete.paramList[0].szUrl, pstRecData->szUrl, DxRECLIST_URL);
	printf("[Unittest] stDelete.paramList[0].szUrl = %s\n", stDelete.paramList[0].szUrl);

	ERR_ret = APK_META_RECORD_DeleteAsync(&stDelete);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_DeleteAsync");
	printf("...Delete successfully...\n");
	HLIB_STD_TaskSleep(2*1000);
	HLIB_STD_MemFree(stDelete.paramList);
	
	bRet = TRUE;
done:
	HLIB_STD_MemFree(pstStartInfo);
	HLIB_STD_MemFree(pstRecData);
	pstStartInfo = NULL;
	pstRecData = NULL;
	return bRet;
}

HBOOL TC_APPKIT_Meta_Recording_All(HxJSON_t j_args)
{
	HxJSON_t j_argument = NULL;
	HxJSON_t j_StartInfo_argument = NULL;

	HBOOL bRet = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	HUINT32 ulViewId, ulRequestId;

	
	OxMediaRec_StartInfo_t 	*pstStartInfo = NULL;
	DxRecListData_t *pstRecData = NULL;


	HCHAR szRecordID[256];
	HUINT32 ulRecSessionId = 0;

	HxJSON_t rename_argument = NULL;
	
	APK_META_REC_RENAME_t		stRenameRec = {0};
	APK_META_REC_STOP_REC_t	stStopRec = {0};
	APK_META_REC_DELETE_t	stDelete	= {0};

	HxJSON_t copy_argument = NULL;
	HCHAR szPathCopyTo[DxRECLIST_PATH+1];

	printf("**************************************************************************\n");
	printf("************************** %s **************************\n", __FUNCTION__);
	printf("**************************************************************************\n");

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_StartInfo_argument = HLIB_JSON_Object_GetObject(j_argument, "StartInfo");
	
	APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	APK_MEDIA_PLAY_GetRequestId(ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&ulRequestId);
	pstStartInfo = (OxMediaRec_StartInfo_t*)HLIB_STD_MemCalloc(sizeof(OxMediaRec_StartInfo_t));
	DONE_ERR(pstStartInfo == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet( pstStartInfo, 0, sizeof(OxMediaRec_StartInfo_t) );

	getInfo_To_Start(pstStartInfo, j_StartInfo_argument, ulRequestId);

	printf("*************** APPKIT_Recording_Start ***************\n");
	ERR_ret = APK_MEDIA_RECORD_Start (pstStartInfo, &ulRecSessionId, szRecordID);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_RECORD_Start");
	HLIB_STD_TaskSleep(ulGetDuration*1000);

	pstRecData = (DxRecListData_t*)HLIB_STD_MemCalloc(sizeof(DxRecListData_t));
	DONE_ERR(pstRecData == NULL, "HLIB_STD_MemCalloc");
	HxSTD_MemSet( pstRecData, 0, sizeof(DxRecListData_t) );

	pstRecData = getInfo_Of_RecordingSession();

	printf("\n..........Testing Renaming........\n");
	printf("\n*************** Renaming the name of recording video ***************\n");
	rename_argument = HLIB_JSON_Object_GetObject(j_argument, "rename");

	stRenameRec.ulParamCount = 1;
	stRenameRec.paramList = (APK_META_REC_RENAME_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_RENAME_PARAM_t) * stRenameRec.ulParamCount);
	HxSTD_StrNCpy(stRenameRec.paramList[0].szUrl, pstRecData->szUrl, DxRECLIST_URL);
	HxSTD_StrNCpy(stRenameRec.paramList[0].szRename, (HCHAR*)HLIB_JSON_Object_GetString(rename_argument, "new_name"), DxRECLIST_NAME_LEN);
	ERR_ret = APK_META_RECORD_Rename(&stRenameRec, TRUE);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_Rename");
	printf("New name of Recording file : %s\n", HLIB_JSON_Object_GetString(rename_argument, "new_name"));
	printf("......Rename successfully the video recording......\n");
	
	HLIB_STD_TaskSleep(10*1000);
	HLIB_STD_MemFree(stRenameRec.paramList);

	printf("\n*************** Stop the recording session ***************\n");
	stStopRec.ulParamCount	= 1;
	stStopRec.paramList		= (APK_META_REC_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_PARAM_t) * stStopRec.ulParamCount);
	DONE_ERR(stStopRec.paramList == NULL, "HLIB_STD_MemCalloc");
	HxSTD_StrNCpy(stStopRec.paramList[0].szUrl, pstRecData->szUrl, DxRECLIST_URL);
	ERR_ret = APK_META_RECORD_StopRecording(&stStopRec, TRUE);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_StopRecording");
	HLIB_STD_TaskSleep(10*1000);
	HLIB_STD_MemFree(stStopRec.paramList);

	printf("\n..........Testing Copying........\n");
	printf("\n*************** Copying the recording video ***************\n");
	copy_argument = HLIB_JSON_Object_GetObject(j_argument, "copy");
	HxSTD_StrNCpy(szPathCopyTo, (HCHAR*)HLIB_JSON_Object_GetString(copy_argument, "PathCopyto"), DxRECLIST_PATH);
	printf("[META_RECORD_Copy] ContentId : %d\n", pstRecData->ulContentId);
	printf("[META_RECORD_Copy] szPathCopyTo : %s\n", szPathCopyTo);
	ERR_ret = APK_META_RECORD_Copy(pstRecData->ulContentId, szPathCopyTo, 0, NULL, NULL);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_Copy");
	printf("...Copying successfully...\n");
	HLIB_STD_TaskSleep(50*1000);

	printf("\n..........Testing Deleting........\n");
	printf("\n*************** Deleting the recording video ***************\n");
	
	stDelete.ulParamCount	= 1;
	stDelete.paramList		= (APK_META_REC_PARAM_t*)HLIB_STD_MemCalloc(sizeof(APK_META_REC_PARAM_t) * stDelete.ulParamCount);
	DONE_ERR(stDelete.paramList == NULL, "HLIB_STD_MemCalloc");
	HLIB_STD_StrNCpySafe(stDelete.paramList[0].szUrl, pstRecData->szUrl, DxRECLIST_URL);
	printf("[Unittest] stDelete.paramList[0].szUrl = %s\n", stDelete.paramList[0].szUrl);

	ERR_ret = APK_META_RECORD_DeleteAsync(&stDelete);
	DONE_ERR(ERR_ret != ERR_OK, "APK_META_RECORD_DeleteAsync");
	printf("...Delete successfully...\n");
	HLIB_STD_TaskSleep(2*1000);
	HLIB_STD_MemFree(stDelete.paramList);

	bRet = TRUE;
done:
	HLIB_STD_MemFree(pstStartInfo);
	HLIB_STD_MemFree(pstRecData);
	pstStartInfo = NULL;
	pstRecData = NULL;
	return bRet;
}

void getInfo_To_Start(OxMediaRec_StartInfo_t *pstStartInfo, HxJSON_t j_StartInfo_argument, HUINT32 ulRequestId)
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

DxRecListData_t *getInfo_Of_RecordingSession(void)
{
	HUINT32 ulCountRecording;
	HxVector_t		*pstRecVector = NULL;
	DxRecListData_t *pstRecData = NULL;
	
	HINT32 i;

	printf("\n*************** Get information of recording session ***************\n");
	ulCountRecording = APK_META_RECORD_Count();
	DONE_ERR(ulCountRecording != 1, "APK_META_RECORD_Count");
	printf("Number of recording = %d\n", ulCountRecording);
	pstRecVector = APK_META_RECORD_GetItems();
	DONE_ERR(pstRecVector == NULL, "APK_META_RECORD_GetItems");

	for(i = 0; i < HLIB_VECTOR_Length(pstRecVector) ; ++i)
	{
		pstRecData = HLIB_VECTOR_ItemAt(pstRecVector, (HINT32)i);
		if(pstRecData != NULL)
		{
			printf("Channel name : %s\n", pstRecData->szSvcName);
			printf("Video recording name : %s\n", pstRecData->szName);
			printf("Id of Content : %d\n", pstRecData->ulContentId);
			printf("Current length of recording session: %d\n", pstRecData->ulRecDuration);
			printf("Video recording is %s\n", (pstRecData->bScrambled) ? "Scrambled" : "not Scrambled");
			printf("Video recording is %s\n", (pstRecData->bEncrypted) ? "Encrypted" : "not Encrypted");
			
			printf("URL of Content : %s\n", pstRecData->szUrl);
			printf("Recording session status: %s\n", (pstRecData->eRecStatus == 0) ? "record start" : (pstRecData->eRecStatus == 1) ? "Running" : (pstRecData->eRecStatus == 2) ? "Recorded" : "InCompleted");
			printf("Type of video recording: %s\n", (pstRecData->eVideoType == 1) ? "SD" : (pstRecData->eVideoType == 2) ? "HD" : (pstRecData->eVideoType == 3) ? "UHD" : "Don't Know");
		}
	}
done:
	HLIB_STD_MemFree(pstRecVector);
	pstRecVector = NULL;
	return pstRecData;
}

void TC_META_RECORDING_Init(HxJSON_t jManifest)
{
	printf("\n*************** TC_RECORDING_Init ***************\n");
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
	s_manifest = jManifest;

	APK_Init();
	APK_META_SVC_Init(TRUE);
	APK_MEDIA_PLAY_Init();
	APK_META_RECORD_Init();
	APK_MEDIA_RECORD_Init();
	APK_META_SCHEDULE_GetInstance();

	HLIB_STD_TaskSleep(2*1000);

	ADD_TC(TC_APPKIT_Meta_Recording_Rename);
	ADD_TC(TC_APPKIT_Meta_Recording_Copy);
	ADD_TC(TC_APPKIT_Meta_Recording_Delete);
	ADD_TC(TC_APPKIT_Meta_Recording_All);
}

void TC_META_RECORDING_DeInit(void)
{
	printf("\n*************** TC_RECORDING_DeInit ***************\n");
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}