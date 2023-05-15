/*
*@ File: tc_appkit_output.c
*@ Author: Peter, vdthe@humaxdigital.com
*@ Date: 07-Feb-2017 2:35 PM
*@ Description: TC for appkit_output: audio, video, HDMI
*/

#define ___HEADER______________________________________________________________________

#include "atkit.h"

#define ENUM2STR(enumVal) 			#enumVal

#define ___STATIC_VARIABLES____________________________________________________________

static HxJSON_t s_manifest = NULL;

HBOOL TC_APK_OUTPUT_AUDIO_MasterMute(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;
	HBOOL bOriginStatus = FALSE;
	HBOOL bSetStatus = FALSE;
	HBOOL bGetStatus = TRUE;
	HUINT32 ulTime = 0;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		bSetStatus = HLIB_JSON_Object_GetBoolean(j_argument, "MasterMute");
		ulTime = HLIB_JSON_Object_GetInteger(j_argument, "timeset");

	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);
	}

	hErr = APK_OUTPUT_AUDIO_GetMasterMute(&bOriginStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_AUDIO_GetMasterMute()-Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	if(bOriginStatus)
	{
		printf("Origin Mastermute status: TRUE \n");
	}
	else
	{
		printf("Origin Mastermute status: FALSE \n");
	}
	hErr = APK_OUTPUT_AUDIO_SetMasterMute(bSetStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_AUDIO_SetMasterMute()-Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("set mute for %d seconds\n", ulTime);
	HLIB_STD_TaskSleep(ulTime*1000);; // Mute for time (s)
	hErr = APK_OUTPUT_AUDIO_GetMasterMute(&bGetStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_AUDIO_GetMasterMute()-Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	if(bGetStatus != bSetStatus)
	{
		printf("[%s] TEST_FAIL: Fail to set/get MasterMute()-Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	APK_OUTPUT_AUDIO_SetMasterMute(bOriginStatus);
	return TRUE;
}


HBOOL TC_APK_OUTPUT_VIDEO_BackGroundColor(HxJSON_t j_args)
{
	HUINT8 ucOriginRed = 0;
	HUINT8 ucOriginGreeen = 0;
	HUINT8 ucOriginBlue = 0;
	HUINT8 ucSetRed = 0;
	HUINT8 ucSetGreen = 0;
	HUINT8 ucSetBlue = 0;
	HUINT8 ucGetRed = 0;
	HUINT8 ucGetGreen = 0;
	HUINT8 ucGetBlue = 0;
	HUINT32 ulTime = 0;
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;


	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		ucSetRed = HLIB_JSON_Object_GetInteger(j_argument, "Red");
		ucSetGreen = HLIB_JSON_Object_GetInteger(j_argument, "Green");
		ucSetBlue = HLIB_JSON_Object_GetInteger(j_argument, "Blue");
		ulTime = HLIB_JSON_Object_GetInteger(j_argument, "timeset");

	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);

	}

	hErr = APK_OUTPUT_VIDEO_GetBackGroundColor(&ucOriginRed, &ucOriginGreeen, &ucOriginBlue);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_VIDEO_GetBackGroundColor() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("Origin Background color, R: %d, G: %d, B: %d\n",ucOriginRed, ucOriginGreeen, ucOriginBlue);

	hErr = APK_OUTPUT_VIDEO_SetBackGroundColor(ucSetRed, ucSetGreen, ucSetBlue);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_VIDEO_SetBackGroundColor() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[%s] Waiting for %d seconds\n", __FUNCTION__, ulTime);
	HLIB_STD_TaskSleep(ulTime*1000);

	hErr = APK_OUTPUT_VIDEO_GetBackGroundColor(&ucGetRed, &ucGetGreen, &ucGetBlue);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_VIDEO_GetBackGroundColor() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	if((ucGetRed != ucSetRed) || (ucGetGreen != ucSetGreen) || (ucGetBlue != ucSetBlue))
	{
		printf("[%s] TEST_FAIL: Fail to Set/Get BackGroundColor() \n", __FUNCTION__);
		fflush(stdout);
		return FALSE;
	}
	APK_OUTPUT_VIDEO_SetBackGroundColor(ucOriginRed, ucOriginGreeen, ucOriginBlue);

	return TRUE;
}

HBOOL TC_APK_OUTPUT_VIDEO_AvEnable(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;
	HBOOL bOriginStatus = FALSE;
	HBOOL bSetStatus = FALSE;
	HBOOL bGetStatus = FALSE;
	HUINT32 ulTime = 0;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		bSetStatus = HLIB_JSON_Object_GetBoolean(j_argument, "AvEnable");
		ulTime = HLIB_JSON_Object_GetInteger(j_argument, "timeset");

	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);

	}

	hErr = APK_OUTPUT_VIDEO_GetAvEnable(&bOriginStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_VIDEO_GetAvEnable() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);		
		return FALSE;
	}
	if(bOriginStatus)
	{
		printf("Original AvEnable status: TRUE\n");
	}
	else
	{
		printf("Original AvEnable status: FALSE\n");
	}
	

	hErr = APK_OUTPUT_VIDEO_SetAvEnable(bSetStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_VIDEO_SetAvEnable() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);		
		return FALSE;
	}
	HLIB_STD_TaskSleep(ulTime*1000);
	hErr = APK_OUTPUT_VIDEO_GetAvEnable(&bGetStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_VIDEO_GetAvEnable() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);		
		return FALSE;
	}
	if(bGetStatus != bSetStatus)
	{
		printf("[%s] TEST_FAIL: Fail to Set/Set AvEnable()\n", __FUNCTION__);
		fflush(stdout);		
		return FALSE;
	}
	APK_OUTPUT_VIDEO_SetAvEnable(bOriginStatus);
	return TRUE;
}

HBOOL TC_APK_OUTPUT_VIDEO_GetPictureRate(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	DxPictureRate_e ePictureRate	=	eDxPICTURERATE_UNKNOWN;
	hErr = APK_OUTPUT_VIDEO_GetPictureRate (&ePictureRate);
	if (hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_VIDEO_GetPictureRate() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);		
		return FALSE;
	}

#if 0    
	switch(ePictureRate)
	{
		case eDxPICTURERATE_24P:
			printf("PictureRate is 24P\n");
			break;
		case eDxPICTURERATE_25P:
			printf("PictureRate is 25P\n");
			break;
		case eDxPICTURERATE_30P:
			printf("PictureRate is 30P\n");
			break;
		case eDxPICTURERATE_50P:
			printf("PictureRate is 50P\n");
			break;
		case eDxPICTURERATE_60P:
			printf("PictureRate is 60P\n");
			break;
		case eDxPICTURERATE_50I:
			printf("PictureRate is 50I\n");
			break;
		case eDxPICTURERATE_60I:
			printf("PictureRate is 60I\n");
			break;
		default:
			printf("Unknown PictureRate\n");
			break;		
	}
#endif

    printf("PictureRate is %s\n", ENUM2STR(ePictureRate));
	return TRUE;
}

HBOOL TC_APK_OUTPUT_VIDEO_GetResolutionStatus(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	DxResolutionStatus_e eResolutionStatus	=	eDxRESOLUTION_STATUS_UNKNOWN;
	hErr = APK_OUTPUT_VIDEO_GetResolutionStatus (&eResolutionStatus);
	if (hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_VIDEO_GetResolutionStatus() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);		
		return FALSE;
	}
	switch(eResolutionStatus)
	{
		case eDxRESOLUTION_STATUS_480I:
			printf("Resolution status is 480I\n");
			break;
		case eDxRESOLUTION_STATUS_576I:
			printf("Resolution status is 576I\n");
			break;
		case eDxRESOLUTION_STATUS_480P:
			printf("Resolution status is 480P\n");
			break;
		case eDxRESOLUTION_STATUS_576P:
			printf("Resolution status is 576P\n");
			break;
		case eDxRESOLUTION_STATUS_720P:
			printf("Resolution status is 720P\n");
			break;
		case eDxRESOLUTION_STATUS_1080I:
			printf("Resolution status is 1080I\n");
			break;
		case eDxRESOLUTION_STATUS_1080P:
			printf("Resolution status is 1080P\n");
			break;
		case eDxRESOLUTION_STATUS_2160P:
			printf("Resolution status is 2160P\n");
			break;
		default:
			printf("Unknown Resolution status !\n");
			break;
	}
	return TRUE;
}

HBOOL TC_APK_OUTPUT_HDMI_CecEnable(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;
	HBOOL bOriginStatus = FALSE;
	HBOOL bSetStatus = FALSE;
	HBOOL bGetStatus = FALSE;
	HUINT32 ulTime = 0;

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		bSetStatus = HLIB_JSON_Object_GetBoolean(j_argument, "CecEnable");
		ulTime = HLIB_JSON_Object_GetInteger(j_argument, "timeset");

	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);

	}

	hErr = APK_OUTPUT_HDMI_GetCecEnable(&bOriginStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_HDMI_GetCecEnable() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);			
		return FALSE;
	}
	if(bOriginStatus)
	{
		printf("Original CecEnable status: TRUE\n");
	}
	else
	{
		printf("Original CecEnable status: FALSE\n");
	}
	hErr = APK_OUTPUT_HDMI_SetCecEnable(bSetStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_HDMI_SetCecEnable() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);			
		return FALSE;
	}
	HLIB_STD_TaskSleep(ulTime * 1000); // Waiting for timeset value

	hErr = APK_OUTPUT_HDMI_GetCecEnable(&bGetStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_HDMI_GetCecEnable() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);			
		return FALSE;
	}
	if(bGetStatus != bSetStatus)
	{
	    printf("[%s] TEST_FAIL: Fail to Set/Get CecEnable() \n", __FUNCTION__);
		fflush(stdout);			
		return FALSE;
	}
	APK_OUTPUT_HDMI_SetCecEnable(bOriginStatus);
	return TRUE;
}

HBOOL TC_APK_OUTPUT_HDMI_GetHdcpInfo(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	DxHdcpStatus_e eHdcpStatus	=	eDxHDCPSTATUS_OFF;
	DxHdcpVersion_e eHdcpVersion = eDxHDCP_VERSION_MAX;

	hErr = APK_OUTPUT_HDMI_GetHdcpStatus (&eHdcpStatus);
	if (hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_HDMI_GetHdcpStatus() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);	
		return FALSE;
	}
	switch(eHdcpStatus)
	{
		case eDxHDCPSTATUS_TRY:
			printf("HDCP status: TRY\n");
			break;
		case eDxHDCPSTATUS_OK:
			printf("HDCP status: OK\n");
			break;
		case eDxHDCPSTATUS_FAIL:
			printf("HDCP status: FAIL\n");
			break;
		case eDxHDCPSTATUS_NODEVICE:
			printf("HDCP status: NODEVICE\n");
			break;
		default:
			printf("HDCP status: UNKNOWN STATUS !\n");
			break;		
	};

	hErr = APK_OUTPUT_HDMI_GetHdcpVersion(&eHdcpVersion);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_HDMI_GetHdcpVersion() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);	
		return FALSE;
	}
	switch(eHdcpVersion)
	{
		case eDxHDCP_VERSION_1_0:
			printf("HDCP vesion: 1.0 \n");
			break;
		case eDxHDCP_VERSION_1_1:
			printf("HDCP vesion: 1.1 \n");
			break;
		case eDxHDCP_VERSION_1_2:
			printf("HDCP vesion: 1.2 \n");
			break;
		case eDxHDCP_VERSION_1_3:
			printf("HDCP vesion: 1.3 \n");
			break;
		case eDxHDCP_VERSION_1_4:
			printf("HDCP vesion: 1.4 \n");
			break;
		case eDxHDCP_VERSION_2_0:
			printf("HDCP vesion: 2.0 \n");
			break;
		case eDxHDCP_VERSION_2_1:
			printf("HDCP vesion: 2.1 \n");
			break;
		case eDxHDCP_VERSION_2_2:
			printf("HDCP vesion: 2.2 \n");
			break;
		default:
			printf("UNKNOWN HDCP Version !\n");
			break;	
	}
	return TRUE;
}

HBOOL TC_APK_OUTPUT_HDMI_GetEdidInfo(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HCHAR szEdid[512] ={0,};
	hErr = APK_OUTPUT_HDMI_GetEdidInfo(szEdid);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FAIL: Fail to APK_OUTPUT_HDMI_GetEdidInfo() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);	
		return FALSE;		
	}
	printf("%s\n", szEdid );
	return TRUE;
}

void TC_APPKIT_OUTPUT_Init(HxJSON_t jManifest)
{

	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
	s_manifest = jManifest;
	APK_OUTPUT_AUDIO_Init();
	APK_OUTPUT_VIDEO_Init ();
	APK_OUTPUT_HDMI_Init();
	ADD_TC(TC_APK_OUTPUT_AUDIO_MasterMute);
	ADD_TC(TC_APK_OUTPUT_VIDEO_BackGroundColor);
	ADD_TC(TC_APK_OUTPUT_VIDEO_AvEnable);
	ADD_TC(TC_APK_OUTPUT_VIDEO_GetPictureRate);
	ADD_TC(TC_APK_OUTPUT_VIDEO_GetResolutionStatus);
	ADD_TC(TC_APK_OUTPUT_HDMI_CecEnable);
	ADD_TC(TC_APK_OUTPUT_HDMI_GetHdcpInfo);
	ADD_TC(TC_APK_OUTPUT_HDMI_GetEdidInfo);




}

void TC_APPKIT_OUTPUT_DeInit(void)
{
    printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}





