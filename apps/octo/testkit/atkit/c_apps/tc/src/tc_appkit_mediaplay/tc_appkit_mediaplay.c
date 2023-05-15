/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  tc_appkit_mediaplay.c
	@brief

	Description:  									\n
	Module: TC ATKIT		\n

	Author: Richard <nmdung@humaxdigital.com>
	Created date: 06 Feb 2017
*/

#include "atkit.h"

static HxJSON_t s_manifest = NULL;
static HUINT32 s_ulViewId;
static HUINT32 s_ulRequestId;
static HUINT32 s_ulSessionId;

void getStartInfo(OxMediaPlay_StartInfo_t *utStartInfo, HxJSON_t j_StartInfo_argument, HUINT32 ulRequestId);

HBOOL TC_APPKIT_MediaPlay_Normal(HxJSON_t j_args)
{
	HBOOL bRet = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	OxMediaPlay_StartInfo_t 	 utStartInfo;
	HxJSON_t j_argument = NULL;
	HxJSON_t j_StartInfo_argument = NULL;

	printf("**************************************************************************\n");
	printf("************************** %s **************************\n", __FUNCTION__);
	printf("**************************************************************************\n");
	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_StartInfo_argument = HLIB_JSON_Object_GetObject(j_argument, "StartInfo");

	ERR_ret = APK_MEDIA_PLAY_GetMainViewId(&s_ulViewId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetMainViewId");

	ERR_ret = APK_MEDIA_PLAY_GetRequestId(s_ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&s_ulRequestId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetRequestId");
	printf("\ns_ulViewId = %d, s_ulRequestId = %d\n", s_ulViewId, s_ulRequestId);

	getStartInfo(&utStartInfo, j_StartInfo_argument, s_ulRequestId);

	ERR_ret = APK_MEDIA_PLAY_Start(s_ulViewId, &utStartInfo, &s_ulSessionId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_Start");

	// HLIB_STD_TaskSleep(2*1000);
	ERR_ret = APK_MEDIA_PLAY_Stop(s_ulViewId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_Stop");
	HLIB_STD_TaskSleep(10*1000);

	ERR_ret = APK_MEDIA_PLAY_Start(s_ulViewId, &utStartInfo, &s_ulSessionId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_Start");
	HLIB_STD_TaskSleep(2*1000);

	bRet = TRUE;
done:
	return bRet;
}

HBOOL TC_APPKIT_MediaPlay_Subtitle(HxJSON_t j_args)
{
	HBOOL bRet = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	OxMediaPlay_StartInfo_t 	 utStartInfo;
	HBOOL IsEnableSubtitle = FALSE;
	HUINT32 ulSizeFont;
	HxJSON_t j_argument = NULL;
	HxJSON_t j_StartInfo_argument = NULL;

	printf("**************************************************************************\n");
	printf("************************** %s **************************\n", __FUNCTION__);
	printf("**************************************************************************\n");
	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_StartInfo_argument = HLIB_JSON_Object_GetObject(j_argument, "StartInfo");

	ERR_ret = APK_MEDIA_PLAY_GetMainViewId(&s_ulViewId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetMainViewId");
	ERR_ret = APK_MEDIA_PLAY_GetRequestId(s_ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&s_ulRequestId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetRequestId");
	printf("\ns_ulViewId = %d, s_ulRequestId = %d\n", s_ulViewId, s_ulRequestId);

	getStartInfo(&utStartInfo, j_StartInfo_argument, s_ulRequestId);

	ERR_ret = APK_MEDIA_PLAY_Start(s_ulViewId, &utStartInfo, &s_ulSessionId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_Start");

	ERR_ret = APK_MEDIA_PLAY_GetSubtitleEnable(s_ulViewId, &IsEnableSubtitle);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetSubtitleEnable");
	if(IsEnableSubtitle == TRUE)
	{
		printf("[%s] Subtitle is Enable\n", __FUNCTION__);
	}
	else
	{
		printf("[%s] Subtitle is Disable\n", __FUNCTION__);
	}

	ERR_ret = APK_MEDIA_PLAY_SetSubtitleEnable(s_ulViewId, TRUE);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_SetSubtitleEnable");
	printf("[%s] Enable subtitle...\n", __FUNCTION__);
	HLIB_STD_TaskSleep(10*1000);

	ERR_ret = APK_MEDIA_PLAY_GetSubtitleFontSize(s_ulViewId, &ulSizeFont);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetSubtitleFontSize");
	printf("[%s] ulSizeFont = %d\n", "GetSubtitleFontSize",ulSizeFont);

	ulSizeFont = HLIB_JSON_Object_GetInteger(j_argument, "sizefont");
	ERR_ret = APK_MEDIA_PLAY_SetSubtitleFontSize(s_ulViewId, ulSizeFont);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_SetSubtitleFontSize");
	printf("[%s] Set sizefont = %d \n", "SetSubtitleFontSize",ulSizeFont);

	ERR_ret = APK_MEDIA_PLAY_SetSubtitleEnable(s_ulViewId, FALSE);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_SetSubtitleEnable");
	printf("[%s] Disable subtitle...\n", __FUNCTION__);

	bRet = TRUE;
done:
	return bRet;
}

HBOOL TC_APPKIT_MediaPlay_Audio(HxJSON_t j_args)
{
	HBOOL bRet = FALSE;
	HERROR ERR_ret = ERR_FAIL;
	OxMediaPlay_StartInfo_t 	 utStartInfo;
	HxJSON_t j_argument = NULL;
	HxJSON_t j_StartInfo_argument = NULL;

	printf("**************************************************************************\n");
	printf("************************** %s **************************\n", __FUNCTION__);
	printf("**************************************************************************\n");
	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	j_StartInfo_argument = HLIB_JSON_Object_GetObject(j_argument, "StartInfo");
	
	ERR_ret = APK_MEDIA_PLAY_GetMainViewId(&s_ulViewId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetMainViewId");
	ERR_ret = APK_MEDIA_PLAY_GetRequestId(s_ulViewId, eOxMP_MEDIATYPE_LIVE, (HUINT32*)&s_ulRequestId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_GetRequestId");
	printf("\ns_ulViewId = %d, s_ulRequestId = %d\n", s_ulViewId, s_ulRequestId);

	getStartInfo(&utStartInfo, j_StartInfo_argument, s_ulRequestId);

	ERR_ret = APK_MEDIA_PLAY_Start(s_ulViewId, &utStartInfo, &s_ulSessionId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_Start");

	ERR_ret = APK_MEDIA_PLAY_PauseAudioClip(s_ulRequestId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_PauseAudioClip");
	HLIB_STD_TaskSleep(10*1000);

	ERR_ret = APK_MEDIA_PLAY_ResumeAudioClip(s_ulRequestId);
	DONE_ERR(ERR_ret != ERR_OK, "APK_MEDIA_PLAY_ResumeAudioClip");

	bRet = TRUE;
done:
	return bRet;
}

void TC_MEDIAPLAY_Init(HxJSON_t jManifest)
{
	printf("\n*************** TC_RECORDING_Init ***************\n");
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
	s_manifest = jManifest;

	APK_MEDIA_PLAY_Init();
	HLIB_STD_TaskSleep(1000);

	ADD_TC(TC_APPKIT_MediaPlay_Normal);
	ADD_TC(TC_APPKIT_MediaPlay_Subtitle);
	ADD_TC(TC_APPKIT_MediaPlay_Audio);
}

void TC_MEDIAPLAY_DeInit(void)
{
	printf("\n*************** TC_RECORDING_DeInit ***************\n");
	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}

void getStartInfo(OxMediaPlay_StartInfo_t *utStartInfo, HxJSON_t j_StartInfo_argument, HUINT32 ulRequestId)
{

	utStartInfo->stLive.eType			= HLIB_JSON_Object_GetInteger(j_StartInfo_argument, "eType");
	utStartInfo->stLive.eLiveType		= HLIB_JSON_Object_GetInteger(j_StartInfo_argument, "eLiveType");
	utStartInfo->stLive.ulMasterSvcUid	= ulRequestId;
	utStartInfo->stLive.ulSuppleSvcUid	= ulRequestId;
	utStartInfo->stLive.eSuppSvcType		= HLIB_JSON_Object_GetInteger(j_StartInfo_argument, "eSuppSvcType");
	utStartInfo->stLive.eTuneMode		= HLIB_JSON_Object_GetInteger(j_StartInfo_argument, "eTuneMode");
	utStartInfo->stLive.ulMajorCHNum   = HLIB_JSON_Object_GetInteger(j_StartInfo_argument, "ulMajorCHNum");
#if 0
	printf("in getInfo.....\n");
	printf("utStartInfo.stLive.eType = %d\n", utStartInfo->stLive.eType);
	printf("utStartInfo.stLive.eLiveType = %d\n", utStartInfo->stLive.eLiveType);
	printf("utStartInfo.stLive.ulMasterSvcUid = %d\n", utStartInfo->stLive.ulMasterSvcUid);
	printf("utStartInfo.stLive.ulSuppleSvcUid = %d\n", utStartInfo->stLive.ulSuppleSvcUid);
	printf("utStartInfo.stLive.eSuppSvcType = %d\n", utStartInfo->stLive.eSuppSvcType);
	printf("utStartInfo.stLive.eTuneMode = %d\n", utStartInfo->stLive.eTuneMode);
	printf("utStartInfo.stLive.ulMajorCHNum = %d\n", utStartInfo->stLive.ulMajorCHNum);
#endif
}